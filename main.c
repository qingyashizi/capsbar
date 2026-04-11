#define WIN32_LEAN_AND_MEAN
#define UNICODE
#define _UNICODE
#include <windows.h>
#include <shellapi.h>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "advapi32.lib")

#define IDI_APP 1

/* ══════════════════ 可调参数 ══════════════════ */
#define OSD_WIDTH       120
#define OSD_HEIGHT      110
#define OSD_BOTTOM_GAP   50
#define OSD_CORNER      18.0f
#define FADE_STEP_MS     16
#define FADE_STEP_VAL    15

/* ═══ E7 翡翠绿光（暗色） ═══ */
#define DARK_BG         0xE00A1412u
#define DARK_BORDER     0x1F34D399u
#define DARK_TEXT_BIG   0xFF6EE7B7u
#define DARK_TEXT_SMALL 0xFF6EE7B7u

/* ═══ E6 毛玻璃白（浅色） ═══ */
#define LITE_BG         0xBFFFFFFFu
#define LITE_BORDER     0x0F000000u
#define LITE_TEXT_BIG   0xFF1D1D1Fu
#define LITE_TEXT_SMALL 0xFF000000u

/* ══════════ GDI+ Flat C API ══════════ */
typedef int GpStatus;
typedef struct { UINT32 GdiplusVersion; void *DebugEventCallback;
                 BOOL SuppressBackgroundThread; BOOL SuppressExternalCodecs;
} GdiplusStartupInput;
typedef struct { float X, Y, Width, Height; } GpRectF;
typedef void *GpGraphics, *GpSolidFill, *GpPen, *GpPath,
             *GpFont, *GpFontFamily, *GpStringFormat;

#define SmoothingModeAntiAlias       4
#define TextRenderingHintAntiAlias       4
#define TextRenderingHintAntiAliasGridFit 3
#define UnitPixel                    2
#define FillModeAlternate            0
#define StringAlignmentCenter        1

extern GpStatus __stdcall GdiplusStartup(ULONG_PTR*, const GdiplusStartupInput*, void*);
extern void     __stdcall GdiplusShutdown(ULONG_PTR);
extern GpStatus __stdcall GdipCreateFromHDC(HDC, GpGraphics**);
extern GpStatus __stdcall GdipDeleteGraphics(GpGraphics*);
extern GpStatus __stdcall GdipGraphicsClear(GpGraphics*, DWORD);
extern GpStatus __stdcall GdipSetSmoothingMode(GpGraphics*, int);
extern GpStatus __stdcall GdipSetTextRenderingHint(GpGraphics*, int);
extern GpStatus __stdcall GdipCreateSolidFill(DWORD, GpSolidFill**);
extern GpStatus __stdcall GdipDeleteBrush(GpSolidFill*);
extern GpStatus __stdcall GdipCreatePen1(DWORD, float, int, GpPen**);
extern GpStatus __stdcall GdipDeletePen(GpPen*);
extern GpStatus __stdcall GdipCreatePath(int, GpPath**);
extern GpStatus __stdcall GdipDeletePath(GpPath*);
extern GpStatus __stdcall GdipAddPathArc(GpPath*, float, float, float, float, float, float);
extern GpStatus __stdcall GdipClosePathFigure(GpPath*);
extern GpStatus __stdcall GdipFillPath(GpGraphics*, GpSolidFill*, GpPath*);
extern GpStatus __stdcall GdipDrawPath(GpGraphics*, GpPen*, GpPath*);
extern GpStatus __stdcall GdipCreateFontFamilyFromName(const WCHAR*, void*, GpFontFamily**);
extern GpStatus __stdcall GdipDeleteFontFamily(GpFontFamily*);
extern GpStatus __stdcall GdipCreateFont(GpFontFamily*, float, int, int, GpFont**);
extern GpStatus __stdcall GdipDeleteFont(GpFont*);
extern GpStatus __stdcall GdipCreateStringFormat(int, LANGID, GpStringFormat**);
extern GpStatus __stdcall GdipDeleteStringFormat(GpStringFormat*);
extern GpStatus __stdcall GdipSetStringFormatAlign(GpStringFormat*, int);
extern GpStatus __stdcall GdipSetStringFormatLineAlign(GpStringFormat*, int);
extern GpStatus __stdcall GdipDrawString(GpGraphics*, const WCHAR*, int, GpFont*,
                                         const GpRectF*, GpStringFormat*, GpSolidFill*);

/* ══════════ 全局变量 ══════════ */
static HWND      g_hwndOSD   = NULL;
static HHOOK     g_hook      = NULL;
static UINT_PTR  g_timerFade = 0;
static BYTE      g_alpha     = 255;
static BOOL      g_capsState = FALSE;
static BOOL      g_visible   = FALSE;
static BOOL      g_darkMode  = TRUE;
static HICON     g_iconOn    = NULL;
static HICON     g_iconOff   = NULL;
static HINSTANCE g_hInst     = NULL;
static ULONG_PTR g_gdipToken = 0;
static NOTIFYICONDATAW g_nid = {0};
static UINT WM_TASKBARCREATED = 0;

/* OSD 位图缓存（避免 fade 动画每帧重绘） */
static HDC     g_cacheDC     = NULL;
static HBITMAP g_cacheBmp    = NULL;
static HBITMAP g_cacheOldBmp = NULL;

/* OSD 当前 DPI 缩放后的尺寸 */
static int g_osdW   = OSD_WIDTH;
static int g_osdH   = OSD_HEIGHT;
static int g_osdGap = OSD_BOTTOM_GAP;

#define WM_TRAYICON (WM_USER + 1)
#define IDM_EXIT        1001
#define IDM_AUTOSTART   1002
#define IDM_THEME_AUTO  1010
#define IDM_THEME_DARK  1011
#define IDM_THEME_LIGHT 1012
#define IDM_LANG_AUTO   1020
#define IDM_LANG_ZH     1021
#define IDM_LANG_EN     1022
#define TIMER_FADE   1

/* ══════════ 多语言支持 ══════════ */
typedef struct {
    const WCHAR *tip;           /* 托盘提示 */
    const WCHAR *menuAutostart; /* 开机启动 */
    const WCHAR *menuTheme;     /* 主题 */
    const WCHAR *menuThemeAuto; /* 跟随系统 */
    const WCHAR *menuThemeDark; /* 暗色模式 */
    const WCHAR *menuThemeLight;/* 亮色模式 */
    const WCHAR *menuLang;      /* 语言 */
    const WCHAR *menuLangAuto;  /* 跟随系统 */
    const WCHAR *menuLangZH;    /* 中文 */
    const WCHAR *menuLangEN;    /* English */
    const WCHAR *menuExit;      /* 退出 */
} LangStrings;

static const LangStrings g_langZH = {
    L"CapsBar - \x5927\x5C0F\x5199\x6307\x793A\x5668",
    L"\x5F00\x673A\x542F\x52A8",
    L"\x4E3B\x9898",
    L"\x8DDF\x968F\x7CFB\x7EDF",
    L"\x6697\x8272\x6A21\x5F0F",
    L"\x4EAE\x8272\x6A21\x5F0F",
    L"\x8BED\x8A00",
    L"\x8DDF\x968F\x7CFB\x7EDF",
    L"\x4E2D\x6587",
    L"English",
    L"\x9000\x51FA CapsBar"
};

static const LangStrings g_langEN = {
    L"CapsBar - Caps Lock Indicator",
    L"Start with Windows",
    L"Theme",
    L"Follow System",
    L"Dark Mode",
    L"Light Mode",
    L"Language",
    L"Follow System",
    L"\x4E2D\x6587",
    L"English",
    L"Exit CapsBar"
};

static const LangStrings *g_lang = &g_langEN;

/* 语言模式：0=跟随系统, 1=强制中文, 2=强制英文 */
static int g_langMode = 0;

static void ApplyLanguage(void) {
    if (g_langMode == 1) { g_lang = &g_langZH; return; }
    if (g_langMode == 2) { g_lang = &g_langEN; return; }
    LANGID lid = GetUserDefaultUILanguage();
    WORD primary = lid & 0x3FF;
    g_lang = (primary == LANG_CHINESE) ? &g_langZH : &g_langEN;
}

static void InitLanguage(void) {
    ApplyLanguage();
}

/* 主题模式：0=跟随系统, 1=强制暗色, 2=强制亮色 */
static int  g_themeMode = 0;
static WCHAR g_iniPath[MAX_PATH] = {0};

/* ── 配置文件读写 ── */
static void InitConfigPath(void) {
    GetModuleFileNameW(NULL, g_iniPath, MAX_PATH);
    /* 把 .exe 替换为 .ini */
    WCHAR *dot = wcsrchr(g_iniPath, L'.');
    if (dot) lstrcpyW(dot, L".ini");
}

static void LoadConfig(void) {
    g_themeMode = GetPrivateProfileIntW(L"Settings", L"ThemeMode", 0, g_iniPath);
    if (g_themeMode < 0 || g_themeMode > 2) g_themeMode = 0;
    g_langMode = GetPrivateProfileIntW(L"Settings", L"LangMode", 0, g_iniPath);
    if (g_langMode < 0 || g_langMode > 2) g_langMode = 0;
}

static void SaveConfig(void) {
    WCHAR val[4];
    wsprintfW(val, L"%d", g_themeMode);
    WritePrivateProfileStringW(L"Settings", L"ThemeMode", val, g_iniPath);
    wsprintfW(val, L"%d", g_langMode);
    WritePrivateProfileStringW(L"Settings", L"LangMode", val, g_iniPath);
}

/* ── 前向声明 ── */
static void RenderOSD(void);
static void CompositeOSD(void);
static void ShowOSD(void);
static void HideOSD(BOOL animate);
static void UpdateTrayIcon(void);
static LRESULT CALLBACK OSDWndProc(HWND, UINT, WPARAM, LPARAM);
static LRESULT CALLBACK LLKeyboardProc(int, WPARAM, LPARAM);

/* ── 系统主题检测 ── */
static BOOL IsSystemDarkMode(void) {
    DWORD value = 1;
    DWORD size = sizeof(DWORD);
    RegGetValueW(HKEY_CURRENT_USER,
                 L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                 L"AppsUseLightTheme", RRF_RT_DWORD, NULL, &value, &size);
    return value == 0;
}

/* ── 根据主题模式决定暗色/亮色 ── */
static BOOL GetEffectiveDarkMode(void) {
    if (g_themeMode == 1) return TRUE;
    if (g_themeMode == 2) return FALSE;
    return IsSystemDarkMode();
}

/* ── 开机启动 ── */
static const WCHAR *REG_RUN_KEY = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
static const WCHAR *REG_APP_NAME = L"CapsBar";

static BOOL IsAutoStartEnabled(void) {
    HKEY hk;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, REG_RUN_KEY, 0, KEY_READ, &hk) != ERROR_SUCCESS)
        return FALSE;
    DWORD type = 0, size = 0;
    BOOL ok = (RegQueryValueExW(hk, REG_APP_NAME, NULL, &type, NULL, &size) == ERROR_SUCCESS);
    RegCloseKey(hk);
    return ok;
}

static void SetAutoStart(BOOL enable) {
    HKEY hk;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, REG_RUN_KEY, 0, KEY_WRITE, &hk) != ERROR_SUCCESS)
        return;
    if (enable) {
        WCHAR path[MAX_PATH];
        GetModuleFileNameW(NULL, path, MAX_PATH);
        RegSetValueExW(hk, REG_APP_NAME, 0, REG_SZ,
                       (const BYTE *)path, (DWORD)((lstrlenW(path) + 1) * sizeof(WCHAR)));
    } else {
        RegDeleteValueW(hk, REG_APP_NAME);
    }
    RegCloseKey(hk);
}

/* ── GDI+ 辅助 ── */
static void MakeRoundRect(GpPath *path, float x, float y, float w, float h, float r) {
    float d = r * 2;
    GdipAddPathArc(path, x,         y,          d, d, 180, 90);
    GdipAddPathArc(path, x + w - d, y,          d, d, 270, 90);
    GdipAddPathArc(path, x + w - d, y + h - d,  d, d,   0, 90);
    GdipAddPathArc(path, x,         y + h - d,  d, d,  90, 90);
    GdipClosePathFigure(path);
}

/* ── DPI 感知 ── */
static void EnableDpiAwareness(void) {
    /* 优先 Per-Monitor V2 (Win10 1703+) */
    typedef BOOL (WINAPI *PFN_SetDpiCtx)(HANDLE);
    HMODULE hUser = GetModuleHandleW(L"user32.dll");
    if (hUser) {
        PFN_SetDpiCtx fn = (PFN_SetDpiCtx)GetProcAddress(hUser, "SetProcessDpiAwarenessContext");
        if (fn && fn((HANDLE)(LONG_PTR)-4)) return;
    }
    /* 回退 Per-Monitor V1 (Win8.1+) */
    HMODULE hShcore = LoadLibraryW(L"shcore.dll");
    if (hShcore) {
        typedef HRESULT (WINAPI *PFN_SetDpi)(int);
        PFN_SetDpi fn = (PFN_SetDpi)GetProcAddress(hShcore, "SetProcessDpiAwareness");
        if (fn) fn(2); /* PROCESS_PER_MONITOR_DPI_AWARE */
        FreeLibrary(hShcore);
    }
}

static UINT GetMonitorDpi(HMONITOR hMon) {
    typedef HRESULT (WINAPI *PFN_GetDpiMon)(HMONITOR, int, UINT*, UINT*);
    static PFN_GetDpiMon pfn = NULL;
    static BOOL init = FALSE;
    if (!init) {
        HMODULE h = LoadLibraryW(L"shcore.dll");
        if (h) pfn = (PFN_GetDpiMon)GetProcAddress(h, "GetDpiForMonitor");
        init = TRUE;
    }
    if (pfn && hMon) {
        UINT dpiX = 96, dpiY = 96;
        if (SUCCEEDED(pfn(hMon, 0, &dpiX, &dpiY))) return dpiX;
    }
    HDC hdc = GetDC(NULL);
    UINT dpi = (UINT)GetDeviceCaps(hdc, LOGPIXELSX);
    ReleaseDC(NULL, hdc);
    return dpi;
}

static inline int Dpi(int base, UINT dpi) { return MulDiv(base, (int)dpi, 96); }
static inline float DpiF(float base, UINT dpi) { return base * dpi / 96.0f; }

/* ── 实心填充托盘图标 ── */
static HICON CreateTrayIcon(void) {
    int sz = 32;
    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);

    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth    = sz;
    bmi.bmiHeader.biHeight   = -sz;
    bmi.bmiHeader.biPlanes   = 1;
    bmi.bmiHeader.biBitCount = 32;
    void *bits = NULL;
    HBITMAP hbm = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
    HBITMAP oldBmp = SelectObject(hdcMem, hbm);

    GpGraphics *g = NULL;
    GdipCreateFromHDC(hdcMem, &g);
    GdipGraphicsClear(g, 0x00000000);
    GdipSetSmoothingMode(g, SmoothingModeAntiAlias);
    GdipSetTextRenderingHint(g, TextRenderingHintAntiAlias);

    /* 实心圆角矩形背景 */
    GpPath *p = NULL;
    GdipCreatePath(FillModeAlternate, &p);
    MakeRoundRect(p, 1, 1, (float)(sz - 2), (float)(sz - 2), 7);
    GpSolidFill *bgBr = NULL;
    GdipCreateSolidFill(0xFF3B82F6u, &bgBr);  /* 蓝色 */
    GdipFillPath(g, bgBr, p);
    GdipDeleteBrush(bgBr);
    GdipDeletePath(p);

    /* 白色 A 字母 */
    GpFontFamily *ff = NULL;
    GdipCreateFontFamilyFromName(L"Segoe UI", NULL, &ff);
    GpFont *font = NULL;
    GdipCreateFont(ff, 18.0f, 1, UnitPixel, &font);
    GpStringFormat *sf = NULL;
    GdipCreateStringFormat(0, 0, &sf);
    GdipSetStringFormatAlign(sf, StringAlignmentCenter);
    GdipSetStringFormatLineAlign(sf, StringAlignmentCenter);
    GpSolidFill *txtBr = NULL;
    GdipCreateSolidFill(0xFFFFFFFFu, &txtBr);
    GpRectF rcTxt = {0, 0, (float)sz, (float)sz};
    GdipDrawString(g, L"A", 1, font, &rcTxt, sf, txtBr);
    GdipDeleteBrush(txtBr);
    GdipDeleteFont(font);
    GdipDeleteFontFamily(ff);
    GdipDeleteStringFormat(sf);
    GdipDeleteGraphics(g);

    SelectObject(hdcMem, oldBmp);
    /* 单色掩码，全零 = 不透明（32位ARGB图标由颜色位图alpha通道决定透明度） */
    BYTE maskBits[128]; /* 32 * 32 / 8 */
    ZeroMemory(maskBits, sizeof(maskBits));
    HBITMAP hbmMask = CreateBitmap(sz, sz, 1, 1, maskBits);
    ICONINFO ii = {0};
    ii.fIcon = TRUE;
    ii.hbmColor = hbm;
    ii.hbmMask  = hbmMask;
    HICON hIcon = CreateIconIndirect(&ii);

    DeleteObject(hbm);
    DeleteObject(hbmMask);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
    return hIcon;
}

/* ── 托盘 ── */
static void CreateTray(HWND hwnd) {
    g_nid.cbSize = sizeof(g_nid);
    g_nid.hWnd   = hwnd;
    g_nid.uID    = 1;
    g_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    g_nid.uCallbackMessage = WM_TRAYICON;
    g_nid.hIcon  = g_capsState ? g_iconOn : g_iconOff;
    lstrcpyW(g_nid.szTip, g_lang->tip);
    Shell_NotifyIconW(NIM_ADD, &g_nid);
}

static void UpdateTrayIcon(void) {
    g_nid.hIcon = g_capsState ? g_iconOn : g_iconOff;
    Shell_NotifyIconW(NIM_MODIFY, &g_nid);
}

static void RemoveTray(void) {
    Shell_NotifyIconW(NIM_DELETE, &g_nid);
}

/* ══════════ 核心绘制 ══════════ */

/* 释放 OSD 位图缓存 */
static void FreeOSDCache(void) {
    if (g_cacheDC) {
        SelectObject(g_cacheDC, g_cacheOldBmp);
        DeleteObject(g_cacheBmp);
        DeleteDC(g_cacheDC);
        g_cacheDC = NULL;
        g_cacheBmp = NULL;
        g_cacheOldBmp = NULL;
    }
}

/* 将缓存位图合成到屏幕（仅更新透明度，不重绘内容） */
static void CompositeOSD(void) {
    if (!g_cacheDC) return;

    POINT ptSrc = {0, 0};
    SIZE  szWnd = {g_osdW, g_osdH};
    BLENDFUNCTION blend = {0};
    blend.BlendOp             = AC_SRC_OVER;
    blend.SourceConstantAlpha = g_alpha;
    blend.AlphaFormat         = AC_SRC_ALPHA;

    /* 定位到当前活动窗口所在的显示器 */
    HWND hwFg = GetForegroundWindow();
    HMONITOR hMon = MonitorFromWindow(hwFg ? hwFg : g_hwndOSD, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfoW(hMon, &mi);
    RECT rc = mi.rcWork;
    POINT ptDst = {
        rc.left + ((rc.right - rc.left) - g_osdW) / 2,
        rc.bottom - g_osdH - g_osdGap
    };

    HDC hdcScreen = GetDC(NULL);
    UpdateLayeredWindow(g_hwndOSD, hdcScreen, &ptDst, &szWnd,
                        g_cacheDC, &ptSrc, 0, &blend, ULW_ALPHA);
    ReleaseDC(NULL, hdcScreen);
}

/* 完整绘制 OSD 内容到缓存，然后合成到屏幕 */
static void RenderOSD(void) {
    /* 获取目标显示器 DPI 并缩放尺寸 */
    HWND hwFg = GetForegroundWindow();
    HMONITOR hMon = MonitorFromWindow(hwFg ? hwFg : g_hwndOSD, MONITOR_DEFAULTTOPRIMARY);
    UINT dpi = GetMonitorDpi(hMon);
    int w = Dpi(OSD_WIDTH, dpi), h = Dpi(OSD_HEIGHT, dpi);
    g_osdW = w; g_osdH = h;
    g_osdGap = Dpi(OSD_BOTTOM_GAP, dpi);

    g_darkMode = GetEffectiveDarkMode();
    DWORD clrBG    = g_darkMode ? DARK_BG         : LITE_BG;
    DWORD clrBdr   = g_darkMode ? DARK_BORDER     : LITE_BORDER;
    DWORD clrBig   = g_darkMode ? DARK_TEXT_BIG   : LITE_TEXT_BIG;
    DWORD clrSmall = g_darkMode ? DARK_TEXT_SMALL  : LITE_TEXT_SMALL;

    /* 重建缓存 DC */
    FreeOSDCache();
    HDC hdcScreen = GetDC(NULL);
    g_cacheDC = CreateCompatibleDC(hdcScreen);
    ReleaseDC(NULL, hdcScreen);

    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth    = w;
    bmi.bmiHeader.biHeight   = -h;
    bmi.bmiHeader.biPlanes   = 1;
    bmi.bmiHeader.biBitCount = 32;
    void *bits = NULL;
    g_cacheBmp = CreateDIBSection(g_cacheDC, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
    g_cacheOldBmp = SelectObject(g_cacheDC, g_cacheBmp);

    GpGraphics *gfx = NULL;
    GdipCreateFromHDC(g_cacheDC, &gfx);
    GdipGraphicsClear(gfx, 0x00000000);
    GdipSetSmoothingMode(gfx, SmoothingModeAntiAlias);
    GdipSetTextRenderingHint(gfx, TextRenderingHintAntiAliasGridFit);

    /* 背景 */
    GpPath *bgPath = NULL;
    GdipCreatePath(FillModeAlternate, &bgPath);
    MakeRoundRect(bgPath, 0.5f, 0.5f, (float)w - 1, (float)h - 1, DpiF(OSD_CORNER, dpi));

    GpSolidFill *bgBr = NULL;
    GdipCreateSolidFill(clrBG, &bgBr);
    GdipFillPath(gfx, bgBr, bgPath);
    GdipDeleteBrush(bgBr);

    GpPen *borderPen = NULL;
    GdipCreatePen1(clrBdr, 1.0f, UnitPixel, &borderPen);
    GdipDrawPath(gfx, borderPen, bgPath);
    GdipDeletePen(borderPen);
    GdipDeletePath(bgPath);

    /* 大号 A */
    GpFontFamily *ffUI = NULL;
    GdipCreateFontFamilyFromName(L"Segoe UI", NULL, &ffUI);
    GpFont *fontBig = NULL;
    GdipCreateFont(ffUI, DpiF(42.0f, dpi), 0, UnitPixel, &fontBig);

    GpStringFormat *sf = NULL;
    GdipCreateStringFormat(0, 0, &sf);
    GdipSetStringFormatAlign(sf, StringAlignmentCenter);
    GdipSetStringFormatLineAlign(sf, StringAlignmentCenter);

    GpSolidFill *bigBr = NULL;
    GdipCreateSolidFill(clrBig, &bigBr);
    GpRectF rcA = {0.0f, DpiF(-4.0f, dpi), (float)w, (float)h * 0.72f};
    GdipDrawString(gfx, L"A", 1, fontBig, &rcA, sf, bigBr);
    GdipDeleteBrush(bigBr);
    GdipDeleteFont(fontBig);

    /* CAPS LOCK 标签 */
    GpFont *fontSmall = NULL;
    GdipCreateFont(ffUI, DpiF(13.0f, dpi), 1, UnitPixel, &fontSmall);  /* 1 = Bold */

    GpSolidFill *smBr = NULL;
    GdipCreateSolidFill(clrSmall, &smBr);
    GpRectF rcLabel = {0.0f, (float)h * 0.62f, (float)w, (float)h * 0.35f};
    GdipDrawString(gfx, L"CAPS LOCK", -1, fontSmall, &rcLabel, sf, smBr);
    GdipDeleteBrush(smBr);
    GdipDeleteFont(fontSmall);
    GdipDeleteFontFamily(ffUI);
    GdipDeleteStringFormat(sf);
    GdipDeleteGraphics(gfx);

    /* 合成到屏幕 */
    CompositeOSD();
}

/* ── 窗口过程 ── */
static LRESULT CALLBACK OSDWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_TIMER:
        if (wp == TIMER_FADE) {
            if (g_alpha <= FADE_STEP_VAL) {
                KillTimer(hwnd, TIMER_FADE);
                g_timerFade = 0;
                ShowWindow(g_hwndOSD, SW_HIDE);
                g_visible = FALSE;
                FreeOSDCache();
            } else {
                g_alpha -= FADE_STEP_VAL;
                CompositeOSD();
            }
        }
        return 0;

    case WM_TRAYICON:
        if (lp == WM_RBUTTONUP) {
            POINT pt;
            GetCursorPos(&pt);
            HMENU hm = CreatePopupMenu();

            /* 开机启动 */
            AppendMenuW(hm, MF_STRING | (IsAutoStartEnabled() ? MF_CHECKED : 0),
                        IDM_AUTOSTART, g_lang->menuAutostart);

            /* 主题子菜单 */
            HMENU hmTheme = CreatePopupMenu();
            AppendMenuW(hmTheme, MF_STRING | (g_themeMode == 0 ? MF_CHECKED : 0),
                        IDM_THEME_AUTO, g_lang->menuThemeAuto);
            AppendMenuW(hmTheme, MF_STRING | (g_themeMode == 1 ? MF_CHECKED : 0),
                        IDM_THEME_DARK, g_lang->menuThemeDark);
            AppendMenuW(hmTheme, MF_STRING | (g_themeMode == 2 ? MF_CHECKED : 0),
                        IDM_THEME_LIGHT, g_lang->menuThemeLight);
            AppendMenuW(hm, MF_POPUP, (UINT_PTR)hmTheme, g_lang->menuTheme);

            /* 语言子菜单 */
            HMENU hmLang = CreatePopupMenu();
            AppendMenuW(hmLang, MF_STRING | (g_langMode == 0 ? MF_CHECKED : 0),
                        IDM_LANG_AUTO, g_lang->menuLangAuto);
            AppendMenuW(hmLang, MF_STRING | (g_langMode == 1 ? MF_CHECKED : 0),
                        IDM_LANG_ZH, g_lang->menuLangZH);
            AppendMenuW(hmLang, MF_STRING | (g_langMode == 2 ? MF_CHECKED : 0),
                        IDM_LANG_EN, g_lang->menuLangEN);
            AppendMenuW(hm, MF_POPUP, (UINT_PTR)hmLang, g_lang->menuLang);

            AppendMenuW(hm, MF_SEPARATOR, 0, NULL);
            AppendMenuW(hm, MF_STRING, IDM_EXIT, g_lang->menuExit);

            SetForegroundWindow(hwnd);
            TrackPopupMenu(hm, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
            DestroyMenu(hm);
        }
        return 0;

    case WM_COMMAND:
        switch (LOWORD(wp)) {
        case IDM_EXIT:
            DestroyWindow(hwnd);
            break;
        case IDM_AUTOSTART:
            SetAutoStart(!IsAutoStartEnabled());
            break;
        case IDM_THEME_AUTO:
            g_themeMode = 0;
            g_darkMode = GetEffectiveDarkMode();
            if (g_visible) RenderOSD();
            SaveConfig();
            break;
        case IDM_THEME_DARK:
            g_themeMode = 1;
            g_darkMode = TRUE;
            if (g_visible) RenderOSD();
            SaveConfig();
            break;
        case IDM_THEME_LIGHT:
            g_themeMode = 2;
            g_darkMode = FALSE;
            if (g_visible) RenderOSD();
            SaveConfig();
            break;
        case IDM_LANG_AUTO:
            g_langMode = 0;
            ApplyLanguage();
            lstrcpyW(g_nid.szTip, g_lang->tip);
            Shell_NotifyIconW(NIM_MODIFY, &g_nid);
            SaveConfig();
            break;
        case IDM_LANG_ZH:
            g_langMode = 1;
            ApplyLanguage();
            lstrcpyW(g_nid.szTip, g_lang->tip);
            Shell_NotifyIconW(NIM_MODIFY, &g_nid);
            SaveConfig();
            break;
        case IDM_LANG_EN:
            g_langMode = 2;
            ApplyLanguage();
            lstrcpyW(g_nid.szTip, g_lang->tip);
            Shell_NotifyIconW(NIM_MODIFY, &g_nid);
            SaveConfig();
            break;
        }
        return 0;

    case WM_DESTROY:
        RemoveTray();
        PostQuitMessage(0);
        return 0;

    default:
        /* Explorer 重启时重新添加托盘图标 */
        if (msg == WM_TASKBARCREATED && WM_TASKBARCREATED != 0) {
            Shell_NotifyIconW(NIM_ADD, &g_nid);
            return 0;
        }
        break;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

/* ── 显示 ── */
static void ShowOSD(void) {
    if (g_timerFade) { KillTimer(g_hwndOSD, TIMER_FADE); g_timerFade = 0; }
    g_alpha = 255;
    ShowWindow(g_hwndOSD, SW_SHOWNOACTIVATE);
    RenderOSD();
    g_visible = TRUE;
}

/* ── 隐藏 ── */
static void HideOSD(BOOL animate) {
    if (!g_visible) return;
    if (animate) {
        if (!g_timerFade)
            g_timerFade = SetTimer(g_hwndOSD, TIMER_FADE, FADE_STEP_MS, NULL);
    } else {
        if (g_timerFade) { KillTimer(g_hwndOSD, TIMER_FADE); g_timerFade = 0; }
        ShowWindow(g_hwndOSD, SW_HIDE);
        g_visible = FALSE;
    }
}

/* ── 键盘钩子 ── */
static LRESULT CALLBACK LLKeyboardProc(int nCode, WPARAM wp, LPARAM lp) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT *p = (KBDLLHOOKSTRUCT *)lp;
        if (p->vkCode == VK_CAPITAL && (wp == WM_KEYDOWN || wp == WM_SYSKEYDOWN)) {
            BOOL newState = !(GetKeyState(VK_CAPITAL) & 1);
            g_capsState = newState;
            UpdateTrayIcon();
            if (newState)
                ShowOSD();
            else
                HideOSD(TRUE);
        }
    }
    return CallNextHookEx(g_hook, nCode, wp, lp);
}

/* ══════════ 入口 ══════════ */
int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrev, LPWSTR cmdLine, int nShow) {
    (void)hPrev; (void)cmdLine; (void)nShow;
    g_hInst = hInst;

    EnableDpiAwareness();

    HANDLE hMutex = CreateMutexW(NULL, TRUE, L"CapsBar_SingleInstance");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        CloseHandle(hMutex);
        return 0;
    }

    GdiplusStartupInput gdipInput = {1, NULL, FALSE, FALSE};
    GdiplusStartup(&g_gdipToken, &gdipInput, NULL);

    InitConfigPath();
    LoadConfig();
    InitLanguage();

    /* 托盘图标：实心蓝色圆角矩形 + 白色 A */
    g_iconOn  = CreateTrayIcon();
    g_iconOff = g_iconOn;

    WNDCLASSEXW wc = {0};
    wc.cbSize        = sizeof(wc);
    wc.lpfnWndProc   = OSDWndProc;
    wc.hInstance      = hInst;
    wc.hIcon         = (HICON)LoadImageW(hInst, MAKEINTRESOURCEW(IDI_APP),
                                         IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);
    wc.hbrBackground  = (HBRUSH)GetStockObject(NULL_BRUSH);
    wc.lpszClassName  = L"CapsBarOSD";
    RegisterClassExW(&wc);

    g_hwndOSD = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT,
        L"CapsBarOSD", NULL, WS_POPUP,
        0, 0, OSD_WIDTH, OSD_HEIGHT,
        NULL, NULL, hInst, NULL);

    g_capsState = (GetKeyState(VK_CAPITAL) & 1) != 0;
    g_darkMode  = GetEffectiveDarkMode();

    WM_TASKBARCREATED = RegisterWindowMessageW(L"TaskbarCreated");
    CreateTray(g_hwndOSD);
    if (g_capsState) ShowOSD();

    g_hook = SetWindowsHookExW(WH_KEYBOARD_LL, LLKeyboardProc, hInst, 0);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    UnhookWindowsHookEx(g_hook);
    FreeOSDCache();
    if (g_iconOn) DestroyIcon(g_iconOn);
    GdiplusShutdown(g_gdipToken);
    CloseHandle(hMutex);
    return 0;
}
