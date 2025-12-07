@echo off
REM EduSavant Windows Build Script
REM This script compiles EduSavant into a Windows executable (.exe)
REM Requirements: MinGW-w64 with g++ compiler

echo ========================================
echo EduSavant Windows Build Script
echo Made By Mr. Aayush Bhandari
echo ========================================
echo.

REM Check if g++ is available
where g++ >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: g++ compiler not found!
    echo Please install MinGW-w64 from: https://www.mingw-w64.org/
    echo Or install MSYS2 and run: pacman -S mingw-w64-x86_64-gcc
    pause
    exit /b 1
)

echo [1/4] Cleaning previous build...
if exist EduSavant.exe del EduSavant.exe
if exist *.o del *.o
if exist src\*.o del src\*.o
if exist vendor\imgui\*.o del vendor\imgui\*.o
if exist vendor\imgui\backends\*.o del vendor\imgui\backends\*.o

echo [2/4] Compiling source files...
g++ -I. -I.\src -I.\vendor\imgui -I.\vendor\imgui\backends -g -Wall -Wformat -c -o main.o main.cpp
if %ERRORLEVEL% NEQ 0 goto :error

g++ -I. -I.\src -I.\vendor\imgui -I.\vendor\imgui\backends -g -Wall -Wformat -c -o src\App.o src\App.cpp
if %ERRORLEVEL% NEQ 0 goto :error

echo [3/4] Compiling ImGui library...
g++ -I. -I.\src -I.\vendor\imgui -I.\vendor\imgui\backends -g -Wall -Wformat -c -o vendor\imgui\imgui.o vendor\imgui\imgui.cpp
if %ERRORLEVEL% NEQ 0 goto :error

g++ -I. -I.\src -I.\vendor\imgui -I.\vendor\imgui\backends -g -Wall -Wformat -c -o vendor\imgui\imgui_demo.o vendor\imgui\imgui_demo.cpp
if %ERRORLEVEL% NEQ 0 goto :error

g++ -I. -I.\src -I.\vendor\imgui -I.\vendor\imgui\backends -g -Wall -Wformat -c -o vendor\imgui\imgui_draw.o vendor\imgui\imgui_draw.cpp
if %ERRORLEVEL% NEQ 0 goto :error

g++ -I. -I.\src -I.\vendor\imgui -I.\vendor\imgui\backends -g -Wall -Wformat -c -o vendor\imgui\imgui_tables.o vendor\imgui\imgui_tables.cpp
if %ERRORLEVEL% NEQ 0 goto :error

g++ -I. -I.\src -I.\vendor\imgui -I.\vendor\imgui\backends -g -Wall -Wformat -c -o vendor\imgui\imgui_widgets.o vendor\imgui\imgui_widgets.cpp
if %ERRORLEVEL% NEQ 0 goto :error

g++ -I. -I.\src -I.\vendor\imgui -I.\vendor\imgui\backends -g -Wall -Wformat -c -o vendor\imgui\backends\imgui_impl_glfw.o vendor\imgui\backends\imgui_impl_glfw.cpp
if %ERRORLEVEL% NEQ 0 goto :error

g++ -I. -I.\src -I.\vendor\imgui -I.\vendor\imgui\backends -g -Wall -Wformat -c -o vendor\imgui\backends\imgui_impl_opengl3.o vendor\imgui\backends\imgui_impl_opengl3.cpp
if %ERRORLEVEL% NEQ 0 goto :error

echo [4/4] Linking executable...
g++ -o EduSavant.exe main.o src\App.o vendor\imgui\imgui.o vendor\imgui\imgui_demo.o vendor\imgui\imgui_draw.o vendor\imgui\imgui_tables.o vendor\imgui\imgui_widgets.o vendor\imgui\backends\imgui_impl_glfw.o vendor\imgui\backends\imgui_impl_opengl3.o -I. -I.\src -I.\vendor\imgui -I.\vendor\imgui\backends -g -Wall -Wformat -lopengl32 -lgdi32 -lglfw3 -mwindows
if %ERRORLEVEL% NEQ 0 goto :error

echo.
echo ========================================
echo BUILD SUCCESSFUL!
echo ========================================
echo Executable created: EduSavant.exe
echo.
echo To run the application, double-click EduSavant.exe
echo or run: EduSavant.exe
echo ========================================
pause
exit /b 0

:error
echo.
echo ========================================
echo BUILD FAILED!
echo ========================================
echo Please check the error messages above.
echo.
echo Common issues:
echo - Missing GLFW library (install via MSYS2: pacman -S mingw-w64-x86_64-glfw)
echo - Missing OpenGL headers
echo ========================================
pause
exit /b 1
