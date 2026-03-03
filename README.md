# ChronoType ⏳

**ChronoType** is a modern, totally local, and stealthy keystroke analytics engine built natively with C++ for Windows. It provides beautifully rendered statistics of your typing habits without compromising privacy.

## Why ChronoType? 🛡️
Security and privacy are the core of ChronoType. It operates with a **strict 100% offline policy.**
- **Local Data Only:** Every single keystroke is processed internally and logged to a local SQLite database (`database.db`) right on your hard drive.
- **Zero Telemetry:** The application does absolutely no outbound connections. No APIs, no phoning home, no anonymous metrics.
- **Dynamic Secure Ports:** The analytical dashboard runs exclusively on an internal `localhost` dynamic port bound uniquely for your machine. No external device can access it.

<br/>

## Dashboard Preview 📊
`![Dashboard Overview](path/)`

`![Top Sentences and Words](path/)`
 
`![Hidden System Tray Usage](path/)`

<br/>

## Features ✨
- **Stealth Background Mode:** The system automatically configures itself as a `WIN32` background application. There are no distracting console windows.
- **System Tray Command:** An unobtrusive, lightweight icon lives in your Windows Tray. Just right-click to instantly snap to your dashboard or gracefully exit.
- **Auto-Boot:** Launch and forget. ChronoType safely injects a local registry key to resume analytics immediately when your computer boots up.
- **Native Multi-Byte Localization:** Full native handling of UTF-8 special keys (e.g., parsing `ğ, ü, ş, ö, ç, i` dynamically into natural word formations without buffer corruption).
- **Embedded C++ HTTP Server:** Employs an ultra-fast local networking thread to display the dashboard via `cpp-httplib` and Vanilla JavaScript + Chart.js.

## Installation & Usage 🛠️

1. **Download:** Go to the [Releases](../../releases) section of this repository and download the latest `ChronoType.zip`.
2. **Extract:** Unzip the folder to your preferred location. *(Note: Make sure the `ui` folder remains in the same directory as the executable).*
3. **Run:** Double-click `ChronoType.exe`. The app will instantly dive into the background without displaying a console window.
4. **Access:** Check your Windows System tray (bottom right). Right-click the ChronoType icon and choose **"Open Dashboard"** to view your stats!
