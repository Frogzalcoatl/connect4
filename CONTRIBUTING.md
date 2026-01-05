# Contributing to Connect4

I highly doubt im gonna have anyone contributing to this project. This is more so for me bc my dumb ah is gonna forget the dependencies i need to build the project if im ever not on my main pc.

## Global Dependencies
1. **Git**
2. **CMake** (Version 3.22+) [Download](https://cmake.org/download/)

## Platform Specific Dependencies

**Windows**
* Download [Build Tools for Visual Studio](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2026)
    * Select and install the **"Desktop development with C++"** workload.

**MacOS**
* Install Xcode Command Line Tools:
```bash
xcode-select --install
```
* (Optional) If you wanna buil the masos ".app" bundle or for ios, you need the full xcode ide

**Linux (Debian/Ubuntu)**
* Needs a C compiler and the system libraries for SDL3 that are listed on their website:
```bash
sudo apt-get update
sudo apt-get install -y \
build-essential git make pkg-config cmake ninja-build \
libasound2-dev libpulse-dev libaudio-dev libjack-dev libsndio-dev \
libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxfixes-dev \
libxi-dev libxss-dev libxtst-dev libxkbcommon-dev libdrm-dev \
libgbm-dev libgl1-mesa-dev libgles2-mesa-dev libegl1-mesa-dev \
libdbus-1-dev libibus-1.0-dev libudev-dev
```

## Build Instructions
1. **Configure:** (Generate build files in the `/build` folder)
```bash
cmake -B build
```
2. **Compile:**
```bash
cmake --build build --config Release
```
3. **Run:**
    * Windows `.\build\bin\Release\Connect4.exe`
    * Mac/Linux `./build/bin/Connect4`

## Android Development (Optional)
This is only needed if you wanna build apks locally or test edits to java files in the `android/` folder
### 1. Prerequisites
* **Java JDK 17:** [Download Eclipse Adoptium (Temurin) 17](https://adoptium.net/)
* **Android SDK & NDK**
### Option A: Android Studio
This handles all paths and dependencies automatically.

1.  Download **[Android Studio](https://developer.android.com/studio)**.
2.  Install and open Android Studio.
3.  Go to **More Actions > SDK Manager** (or Settings > Languages & Frameworks > Android SDK).
4.  **SDK Platforms Tab:** Check **Android 15 (API 35)**.
5.  **SDK Tools Tab:** Check the following:
    *   **Android SDK Build-Tools**
    *   **NDK (Side by side)**
    *   **CMake**
    *   **Android SDK Command-line Tools**
6.  Click **Apply** to download and install.

### Option B: Command Line Tools (Lightweight / No IDE)
If you don't wanna install the full Android Studio IDE.

1.  Download the [Android Studio Command Line Tools](https://developer.android.com/studio#command-line-tools-only)
2.  Extract the zip file to a folder (ex: `C:\android-sdk` or `~/android-sdk`).
3.  Inside that folder, create a subfolder structure so the path looks exactly like this:
    `.../android-sdk/cmdline-tools/latest/`
    (Move the extracted `bin` and `lib` folders inside the new `latest` folder).
4.  Open your terminal/command prompt in that `bin` folder and run this command to install the specific versions you need:

    **Windows:**
    ```cmd
    sdkmanager.bat "platform-tools" "platforms;android-35" "build-tools;35.0.0" "ndk;28.2.13676358" "cmake;3.22.1"
    ```

    **Mac/Linux:**
    ```bash
    ./sdkmanager "platform-tools" "platforms;android-35" "build-tools;35.0.0" "ndk;28.2.13676358" "cmake;3.22.1"
    ```

### 2. Configuration (`local.properties`)
Create a file named `local.properties` inside the project's `android/` folder. It must point to your SDK location.

**Windows Example:**
```local.properties
sdk.dir=C\:\\Users\\YourName\\AppData\\Local\\Android\\Sdk
```

**Mac/Linux Example:**
```local.properties
sdk.dir=/Users/YourName/Library/Android/sdk
```

### 3. **Build**
Using Command Line to build the .apk:
```bash
cd android
# Windows
.\gradlew.bat assembleDebug
# Mac/Linux
./gradlew.bat assembleDebug
```
* These commands are also available in `.vscode/tasks.json` if youre using vscode

## Vscode Setup (Recommended IDE)
Speaking of vscode thats what ive been using for this.

1. **Install Extensions:**
    * When you open the project folder, accept the recommendation to install the **C/C++ Extension Pack** and optionally **Gradle for Java** if you plan on messing around in the android folder.
2. **Java Setup:**
    * Make sure your Vscode settings point to JDK 17 to avoid gradle errors
    * Add this to your user settings if you see "Unsupported class file version" errors: 
    ```json
    "java.import.gradle.java.home": "/path/to/jdk-17"
    ```
3. **Building**
    * **For Desktop:** Press `F7` or press build in the CMake bottom bar thingy.
    * **For Android .apk file:** Press `Ctrl+Shift+B` and select **"Android: Build APK (Debug)"**