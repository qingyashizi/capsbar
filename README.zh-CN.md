<div align="center">

<img src="app.ico" width="80" height="80" alt="CapsBar 图标"/>

# CapsBar

**优雅的 Windows 大写锁定状态提示器**

[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Windows-blue.svg)](https://www.microsoft.com/windows)
[![Language](https://img.shields.io/badge/Language-C-lightgrey.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Release](https://img.shields.io/github/v/release/qingyashizi/capsbar)](https://github.com/qingyashizi/capsbar/releases/latest)

**[English Documentation](README.md)**

</div>

---

## 这是什么？

CapsBar 是一个轻量级 Windows 托盘程序。每当大写开启时，屏幕底部会**始终显示**一个圆角提示窗，让你一眼就能看清当前状态。关闭大写后提示窗自动淡出消失。从此告别不知道大小写是否开启的困惑。

## 截图

| 暗色模式 | 亮色模式 |
|:--------:|:--------:|
| ![暗色模式](dark.png) | ![亮色模式](light.png) |

## 功能特性

- **OSD 悬浮提示** — 屏幕底部弹出圆角提示窗。**大写开启时常驻显示**，关闭大写时自动淡出
- **淡出动画** — 关闭大写时提示窗自然淡出消失，不打断你的工作流
- **暗色 / 亮色主题** — 自动跟随 Windows 系统主题，也可在托盘菜单中手动切换
- **中英双语** — 自动检测系统语言，或在菜单中手动选择
- **DPI 感知** — 完整支持 Per-Monitor V2 DPI，在任何分辨率和缩放比例下都清晰锐利
- **开机自启** — 托盘菜单中一键开关"开机启动"
- **系统托盘图标** — 静默驻守通知区域，右键弹出菜单
- **极小体积** — 单文件便携 `.exe`，体积仅 **37 KB**，无需安装，无运行时依赖，无注册表残留

## 系统要求

| | |
|---|---|
| **最低要求** | Windows 7 SP1 |
| **推荐配置** | Windows 10 及以上（获得完整 Per-Monitor DPI 支持） |
| **架构** | x86 / x64 |

## 安装方式

1. 从 [Releases](https://github.com/qingyashizi/capsbar/releases/latest) 页面下载最新的 `capsbar.exe`
2. 将其放置到你喜欢的位置（例如 `C:\Program Files\CapsBar\`）
3. 直接运行 `capsbar.exe`，托盘图标即刻出现
4. 右键托盘图标 → **开机启动** 即可设置自动启动

> 无需安装向导。卸载时，在托盘菜单中关闭"开机启动"，然后删除文件即可。

## SmartScreen 安全警告说明

首次下载或运行 CapsBar 时，浏览器或 Windows 可能会弹出**安全警告**，这属于正常现象，并不代表软件存在风险。

**为什么会有这个警告？**  
CapsBar 已使用有效的 [Certum](https://www.certum.eu/) 代码签名证书进行签名（可验证发布者为 *Fangmeng Liu*）。但 Microsoft SmartScreen 的信任机制基于程序的**下载量和用户接受记录**来建立——新发布的软件即使持有有效证书，也需要一定时间积累信誉值，这一阶段的警告会随着下载量增加而逐渐消失。

**本软件全部源代码（约 660 行 C 语言）均已在此仓库开放**，你可以自行审阅代码、从源码编译，或在运行前进行任意方式的安全验证。

**如何跳过警告运行程序：**

- **浏览器下载警告** — Chrome 点击 **保留**，Edge 点击 **保留** 或 **仍然保留**。
- **SmartScreen 弹窗**（蓝色"Windows 已保护你的电脑"对话框）— 点击 **更多信息**，再点击 **仍要运行**。

**如何独立验证签名真实性：**

1. 右键 `capsbar.exe` → **属性** → **数字签名** 选项卡
2. 签名者应显示 **Fangmeng Liu**，颁发机构为 **Certum Code Signing 2021 CA**
3. 或在 PowerShell 中运行：
   ```powershell
   Get-AuthenticodeSignature "capsbar.exe" | Select-Object Status, SignerCertificate
   ```
   预期输出：`Status: Valid`

## 配置文件

设置会自动保存到与可执行文件同目录的 `capsbar.ini` 文件中。

| 键名 | 可选值 | 说明 |
|---|---|---|
| `ThemeMode` | `0` 跟随系统 · `1` 暗色 · `2` 亮色 | OSD 颜色主题 |
| `LangMode` | `0` 跟随系统 · `1` 中文 · `2` 英文 | 界面语言 |

## 从源码构建

**前置条件：** CMake 3.10+，MSVC（Visual Studio）或 MinGW-w64

```bash
git clone https://github.com/qingyashizi/capsbar.git
cd capsbar
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

生成的 `capsbar.exe` 位于 `build/Release/`（MSVC）或 `build/`（MinGW）。

## 托盘菜单说明

| 菜单项 | 说明 |
|---|---|
| 开机启动 | 开关登录时自动运行 |
| 主题 → 跟随系统 | 使用 Windows 暗色/亮色设置 |
| 主题 → 暗色模式 | 强制使用翡翠绿暗色风格 |
| 主题 → 亮色模式 | 强制使用毛玻璃亮色风格 |
| 语言 | 在自动 / 中文 / English 之间切换 |
| 退出 CapsBar | 退出程序 |

## 开源协议

MIT 协议 — 详情见 [LICENSE](LICENSE) 文件。
