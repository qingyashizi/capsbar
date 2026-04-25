<div align="center">

<img src="app.ico" width="80" height="80" alt="CapsBar Icon"/>

# CapsBar

**Elegant Caps Lock Indicator for Windows**

[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Windows-blue.svg)](https://www.microsoft.com/windows)
[![Language](https://img.shields.io/badge/Language-C-lightgrey.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Release](https://img.shields.io/github/v/release/qingyashizi/capsbar)](https://github.com/qingyashizi/capsbar/releases/latest)

**[中文文档](README.zh-CN.md)**

</div>

---

## What is CapsBar?

CapsBar is a lightweight Windows tray application that displays a beautiful on-screen indicator every time you toggle Caps Lock. No more accidentally typing in the wrong case.

## Screenshots

| Dark Mode | Light Mode |
|:---------:|:----------:|
| ![Dark Mode](dark.png) | ![Light Mode](light.png) |

## Features

- **OSD Notification** — A rounded popup appears at the bottom of your screen. It **stays visible while Caps Lock is ON** and fades out when you turn it off
- **Fade Animation** — The indicator fades out naturally when Caps Lock is turned off, without interrupting your workflow
- **Dark / Light Theme** — Follows your Windows system theme automatically, or set it manually via the tray menu
- **Bilingual** — Supports Chinese and English; auto-detects your system language or let you choose manually
- **DPI Aware** — Full Per-Monitor V2 DPI support; looks crisp on any resolution or scaling factor
- **Auto-start** — Optional "Start with Windows" toggle in the tray menu
- **System Tray Icon** — Lives quietly in the notification area; right-click for options
- **Tiny Footprint** — Single portable `.exe` of only **37 KB**, no installer, no runtime dependencies, no registry clutter

## Requirements

| | |
|---|---|
| **Minimum** | Windows 7 SP1 |
| **Recommended** | Windows 10 or later (for full Per-Monitor DPI support) |
| **Architecture** | x86 / x64 |

## Installation

1. Download the latest `capsbar.exe` from [Releases](https://github.com/qingyashizi/capsbar/releases/latest)
2. Place it anywhere you like (e.g. `C:\Program Files\CapsBar\`)
3. Run `capsbar.exe` — the tray icon appears immediately
4. Right-click the tray icon → **Start with Windows** to enable auto-start

> No installation wizard needed. To uninstall, disable auto-start from the tray menu and delete the file.

## SmartScreen Warning

When downloading or running CapsBar for the first time, you may see a **Windows SmartScreen warning** or a browser security alert. This is expected behavior and not a sign of malware.

**Why does this happen?**  
CapsBar is signed with a valid [Certum](https://www.certum.eu/) code signing certificate (you can verify the publisher name *Fangmeng Liu*). However, Microsoft SmartScreen assigns trust based on a program's download reputation — new software with few downloads has not yet accumulated enough reputation, regardless of its certificate status. The warning will diminish as more users download and run the app.

**The entire source code (~660 lines of C) is available in this repository.** You are welcome to read it, build it yourself, or audit it before running the binary.

**How to proceed past the warning:**

- **Browser download warning** — Click **Keep** (Chrome) or **Keep anyway** (Edge) when prompted.
- **SmartScreen popup** (blue "Windows protected your PC" dialog) — Click **More info**, then **Run anyway**.

You can independently verify the binary's integrity:
1. Right-click `capsbar.exe` → **Properties** → **Digital Signatures** tab
2. The signature should show **Fangmeng Liu** issued by **Certum Code Signing 2021 CA**
3. Or run in PowerShell:
   ```powershell
   Get-AuthenticodeSignature "capsbar.exe" | Select-Object Status, SignerCertificate
   ```
   Expected output: `Status: Valid`

## Configuration

Settings are saved automatically to `capsbar.ini` in the same folder as the executable.

| Key | Values | Description |
|---|---|---|
| `ThemeMode` | `0` Follow System · `1` Dark · `2` Light | OSD color theme |
| `LangMode` | `0` Follow System · `1` Chinese · `2` English | UI language |

## Build from Source

**Prerequisites:** CMake 3.10+, MSVC (Visual Studio) or MinGW-w64

```bash
git clone https://github.com/qingyashizi/capsbar.git
cd capsbar
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

The resulting `capsbar.exe` will be in `build/Release/` (MSVC) or `build/` (MinGW).

## Tray Menu

| Menu Item | Description |
|---|---|
| Start with Windows | Toggle auto-start at login |
| Theme → Follow System | Use Windows dark/light setting |
| Theme → Dark Mode | Force dark emerald style |
| Theme → Light Mode | Force frosted-glass light style |
| Language | Switch between Auto / 中文 / English |
| Exit CapsBar | Quit the application |

## License

MIT License — see [LICENSE](LICENSE) for details.
