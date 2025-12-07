# EduSavant - Windows Build Instructions

## Prerequisites

To build EduSavant on Windows, you need:

1. **MinGW-w64** (GCC compiler for Windows)
2. **GLFW library** (for window management)
3. **OpenGL** (usually comes with graphics drivers)

## Installation Steps

### Option 1: Using MSYS2 (Recommended)

1. **Download and install MSYS2** from https://www.msys2.org/

2. **Open MSYS2 MinGW 64-bit terminal** and run:
   ```bash
   pacman -Syu
   pacman -S mingw-w64-x86_64-gcc
   pacman -S mingw-w64-x86_64-glfw
   ```

3. **Add MinGW to PATH**:
   - Add `C:\msys64\mingw64\bin` to your Windows PATH environment variable

### Option 2: Standalone MinGW

1. Download MinGW-w64 from https://www.mingw-w64.org/
2. Download GLFW from https://www.glfw.org/download.html
3. Extract and configure manually (advanced users)

## Building the Application

1. **Open Command Prompt** in the project directory

2. **Run the build script**:
   ```cmd
   build_windows.bat
   ```

3. **Wait for compilation** - this may take a few minutes

4. **Run the application**:
   ```cmd
   EduSavant.exe
   ```

## Troubleshooting

### "g++ is not recognized"
- Make sure MinGW bin folder is in your PATH
- Restart Command Prompt after adding to PATH

### "cannot find -lglfw3"
- Install GLFW library via MSYS2: `pacman -S mingw-w64-x86_64-glfw`
- Or download pre-compiled GLFW binaries

### "OpenGL errors"
- Update your graphics drivers
- OpenGL 3.0+ is required

## File Structure

```
EduSavant/
├── build_windows.bat    # Windows build script
├── EduSavant.exe        # Compiled executable (after build)
├── main.cpp             # Entry point
├── src/                 # Source files
├── vendor/              # Third-party libraries
│   └── imgui/          # Dear ImGui
└── *.db                # Database files (created at runtime)
```

## Credits

**Made By Mr. Aayush Bhandari**

---

For Linux/Mac users, use the provided Makefile instead:
```bash
make
./EduSavant
```
