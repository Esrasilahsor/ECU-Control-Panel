# ECU Control Panel

An advanced, modern Qt/QML-based desktop application designed for monitoring, controlling, and diagnosing multi-node Electronic Control Units (ECUs). The panel provides real-time telemetry visualization, dynamic Excel-driven parameter mapping, firmware management, and diagnostic logging.

---

## 🚀 Features

- **Multi-Node ECU Management:**
  - Simultaneous monitoring and control of up to 4 ECU modules (**ECU A.1**, **ECU A.2**, **ECU B.1**, **ECU B.2**).
  - Module state management: Connect / Disconnect, Power On / Power Off, Emergency Kill Switch.
  - Distinct operational modes: **DATA VIEW** and **FIRMWARE UPLOAD**.

- **Excel-Driven Parameter Decoding (QXlsx):**
  - Load custom `.xlsx` definitions specifying RAM memory addresses, data widths, bit offsets, data types, and conversion formulas.
  - Automatically parse raw ECU memory frames into engineering values based on active Excel configurations.
  - Remembers previously loaded Excel configurations per module via persistent settings.

- **Real-Time Telemetry & Data Visualization:**
  - Interactive parameter tables displaying raw and scaled values in real time.
  - Dynamic graph plotting with configurable history depth to track parameter trends.
  - Start, pause, and inspect live data streaming or request targeted test frames.

- **Firmware Upload & Flashing:**
  - Supports flashing `.bin` and `.hex` firmware binaries to individual ECU modules.
  - Real-time upload progress tracking and transfer rate monitoring.
  - Safe transfer mechanisms with validation and cancelation capabilities.

- **Integrated System Logger:**
  - Real-time timestamped event logging tracking connection events, power toggles, telemetry status, and upload outcomes.

- **Modern Dark UI:**
  - Fluid, responsive interface built with QtQuick Controls 2.
  - High-DPI scaling enabled for crisp display on high-resolution screens.

---

## 📁 Project Structure

```text
ECU_Control_Panel/
├── ECU_Control_Panel.pro    # Main qmake project configuration file
├── Headers/                 # C++ header files
│   ├── connector.h          # ECU module communication & state engine
│   ├── dispatcher.h         # Central coordinator exposing APIs to QML
│   └── excelparser.h        # Excel sheet reader & parameter unpacker
├── Sources/                 # C++ implementation files
│   ├── connector.cpp        # Serial/network protocol & simulation logic
│   ├── dispatcher.cpp       # Multi-module routing, history & properties
│   ├── excelparser.cpp      # QXlsx memory layout decoding algorithms
│   └── main.cpp             # Application entry point & QML engine setup
├── Resources/               # QML user interface & assets
│   ├── qml.qrc              # Qt resource collection definition
│   ├── Main.qml             # Primary application window & layout stack
│   ├── ECUPanel.qml         # Individual ECU dashboard tile widget
│   ├── DataView.qml         # Live telemetry data table & graph page
│   └── FirmwareUpload.qml   # Firmware flashing workflow & progress UI
├── data/                    # Sample Excel layouts and test firmware
│   ├── ECU_Data.xlsx
│   ├── ECU_Engine_Data_Fixed.xlsx
│   ├── ECU_Cooling_Data_Fixed.xlsx
│   ├── ECU_Electrical_Data_Fixed.xlsx
│   └── ECU_Test_Firmware*.bin / .hex
├── thirdparty/              # External libraries
│   └── QXlsx/               # Qt-based Excel reading library (submodule/pri)
└── README.md                # Project documentation
```

---

## 🛠️ Prerequisites & Requirements

- **C++ Compiler:** GCC / MinGW 8.1.0 (64-bit) or higher (C++14 standard).
- **Qt Framework:** Qt 5.15 LTS (MinGW 64-bit recommended).
  - Required Qt modules:
    - `Qt Quick` & `Qt Quick Controls 2`
    - `Qt Core`, `Qt GUI`, `Qt Network`
    - `Qt QML` & `Qt Labs Settings`
    - `Qt Quick Dialogs`

---

## 🔨 Building & Running

### Option 1: Using Qt Creator (Recommended)

1. Launch **Qt Creator**.
2. Select **File -> Open File or Project...** and select `ECU_Control_Panel.pro`.
3. Choose the **Desktop Qt 5.15.x MinGW 64-bit** kit.
4. Click **Run** (`Ctrl + R`) or **Build** (`Ctrl + B`).

### Option 2: Command Line Build (MinGW)

Ensure `qmake.exe` and `mingw32-make.exe` are available in your system `PATH`:

```powershell
# 1. Create a build directory
mkdir build\Release
cd build\Release

# 2. Run qmake
qmake.exe ..\..\ECU_Control_Panel.pro -spec win32-g++ "CONFIG+=release"

# 3. Compile
mingw32-make.exe -j4
```

### Option 3: Creating a Standalone Executable (windeployqt)

To distribute the application without requiring a full Qt installation on the target PC:

```powershell
# From the directory containing the compiled ECU_Control_Panel.exe:
windeployqt.exe --qmldir ..\..\Resources ECU_Control_Panel.exe
```

All required Qt runtime DLLs, QML plugins, and platform drivers will be automatically copied next to the executable.

---

## 📖 Usage Guide

1. **Dashboard Overview:**
   - The main window displays all 4 ECU units with their live connection status.
   - Use the **Connect / Disconnect** and **Power** buttons to control module power states.
   - Switch between **DATA VIEW** and **FIRMWARE UPLOAD** modes using the dropdown on each ECU card.

2. **Loading Parameter Definitions:**
   - Navigate to the **DATA VIEW** of a module.
   - Click **Select Excel** and choose a definition file from the `data/` directory (e.g., `ECU_Engine_Data_Fixed.xlsx`).
   - The table will populate with the configured parameters, raw memory addresses, and data widths.

3. **Monitoring Telemetry:**
   - Click **Start Data** to begin streaming telemetry.
   - Select any parameter from the list to display its live value on the historical graph.

4. **Flashing Firmware:**
   - Switch the module mode to **FIRMWARE UPLOAD** and enter the firmware screen.
   - Choose a firmware binary (`.bin` or `.hex`) from `data/`.
   - Click **Start Upload** to begin flashing. Progress, transfer speed, and status messages will update dynamically.

---

## 📄 License

This project is licensed under the MIT License - see the project source files for details. External libraries (such as QXlsx) are subject to their respective open-source licenses.
