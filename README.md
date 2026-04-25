<div align="center">

<img src="app.ico" width="80" height="80" alt="CapsBar Icon"/>

# CapsBar

**Elegant Caps Lock Indicator for Windows**
**优雅的 Windows 大写锁定状态提示器**

[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Windows-blue.svg)](https://www.microsoft.com/windows)
[![Language](https://img.shields.io/badge/Language-C-lightgrey.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Release](https://img.shields.io/github/v/release/qingyashizi/capsbar)](https://github.com/qingyashizi/capsbar/releases/latest)

[English](#english) · [中文](#中文)

</div>

---

## English

### What is CapsBar?

CapsBar is a lightweight Windows tray application that displays a beautiful on-screen indicator every time you toggle Caps Lock. No more accidentally typing in the wrong case.

### Screenshots

| Dark Mode | Light Mode |
|:---------:|:----------:|
| ![Dark Mode](dark.png) | ![Light Mode](light.png) |

### Features

- **OSD Notification** — A smooth rounded popup appears at the bottom of your screen whenever Caps Lock state changes
- **Fade Animation** — The indicator fades in and out naturally, without interrupting your workflow
- **Dark / Light Theme** — Follows your Windows system theme automatically, or set it manually via the tray menu
- **Bilingual** — Supports Chinese and English; auto-detects your system language or let you choose manually
- **DPI Aware** — Full Per-Monitor V2 DPI support; looks crisp on any resolution or scaling factor
- **Auto-start** — Optional "Start with Windows" toggle in the tray menu
- **System Tray Icon** — Lives quietly in the notification area; right-click for options
- **Tiny Footprint** — Single portable `.exe` of only **37 KB**, no installer, no runtime dependencies, no registry clutter

### Requirements

| | |
|---|---|
| **Minimum** | Windows 7 SP1 |
| **Recommended** | Windows 10 or later (for full Per-Monitor DPI support) |
| **Architecture** | x86 / x64 |

### Installation

1. Download the latest `capsbar.exe` from [Releases](https://github.com/qingyashizi/capsbar/releases/latest)
2. Place it anywhere you like (e.g. `C:\Program Files\CapsBar\`)
3. Run `capsbar.exe` — the tray icon appears immediately
4. Right-click the tray icon → **Start with Windows** to enable auto-start

> No installation wizard needed. To uninstall, disable auto-start from the tray menu and delete the file.

### Configuration

Settings are saved automatically to `capsbar.ini` in the same folder as the executable.

| Key | Values | Description |
|---|---|---|
| `ThemeMode` | `0` Follow System · `1` Dark · `2` Light | OSD color theme |
| `LangMode` | `0` Follow System · `1` Chinese · `2` English | UI language |

### Build from Source

**Prerequisites:** CMake 3.10+, MSVC (Visual Studio) or MinGW-w64

```bash
git clone https://github.com/qingyashizi/capsbar.git
cd capsbar
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

The resulting `capsbar.exe` will be in `build/Release/` (MSVC) or `build/` (MinGW).

### Tray Menu

| Menu Item | Description |
|---|---|
| Start with Windows | Toggle auto-start at login |
| Theme → Follow System | Use Windows dark/light setting |
| Theme → Dark Mode | Force dark emerald style |
| Theme → Light Mode | Force frosted-glass light style |
| Language | Switch between Auto / 中文 / English |
| Exit CapsBar | Quit the application |

### License

MIT License — see [LICENSE](LICENSE) for details.

---

## 中文

### 这是什么？

CapsBar 是一个轻量级 Windows 托盘程序，每当你切换大写锁定键时，屏幕底部会弹出一个精美的状态提示浮窗。从此告别不知道大小写是否开启的困惑。

### 功能特性

- **OSD 悬浮提示** — 每次 Caps Lock 状态变化时，屏幕底部平滑弹出圆角提示窗
- **淡入淡出动画** — 自然地淡入淡出，不打断你的工作流
- **暗色 / 亮色主题** — 自动跟随 Windows 系统主题，也可在托盘菜单中手动切换
- **中英双语** — 自动检测系统语言，或在菜单中手动选择
- **DPI 感知** — 完整支持 Per-Monitor V2 DPI，在任何分辨率和缩放比例下都清晰锐利
- **开机自启** — 托盘菜单中一键开关"开机启动"
- **系统托盘图标** — 静默驻守通知区域，右键弹出菜单
- **极小体积** — 单文件便携 `.exe`，体积仅 **37 KB**，无需安装，无运行时依赖，无注册表残留

### 系统要求

| | |
|---|---|
| **最低要求** | Windows 7 SP1 |
| **推荐配置** | Windows 10 及以上（获得完整 Per-Monitor DPI 支持） |
| **架构** | x86 / x64 |

### 安装方式

1. 从 [Releases](https://github.com/qingyashizi/capsbar/releases/latest) 页面下载最新的 `capsbar.exe`
2. 将其放置到你喜欢的位置（例如 `C:\Program Files\CapsBar\`）
3. 直接运行 `capsbar.exe`，托盘图标即刻出现
4. 右键托盘图标 → **开机启动** 即可设置自动启动

> 无需安装向导。卸载时，在托盘菜单中关闭"开机启动"，然后删除文件即可。

### 配置文件

设置会自动保存到与可执行文件同目录的 `capsbar.ini` 文件中。

| 键名 | 可选值 | 说明 |
|---|---|---|
| `ThemeMode` | `0` 跟随系统 · `1` 暗色 · `2` 亮色 | OSD 颜色主题 |
| `LangMode` | `0` 跟随系统 · `1` 中文 · `2` 英文 | 界面语言 |

### 从源码构建

**前置条件：** CMake 3.10+，MSVC（Visual Studio）或 MinGW-w64

```bash
git clone https://github.com/qingyashizi/capsbar.git
cd capsbar
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

生成的 `capsbar.exe` 位于 `build/Release/`（MSVC）或 `build/`（MinGW）。

### 托盘菜单说明

| 菜单项 | 说明 |
|---|---|
| 开机启动 | 开关登录时自动运行 |
| 主题 → 跟随系统 | 使用 Windows 暗色/亮色设置 |
| 主题 → 暗色模式 | 强制使用翡翠绿暗色风格 |
| 主题 → 亮色模式 | 强制使用毛玻璃亮色风格 |
| 语言 | 在自动 / 中文 / English 之间切换 |
| 退出 CapsBar | 退出程序 |

### 开源协议

MIT 协议 — 详情见 [LICENSE](LICENSE) 文件。
