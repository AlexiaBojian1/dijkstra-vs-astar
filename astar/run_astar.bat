@echo off
echo ==============================
echo Compiling All A* Variants
echo ==============================

REM Create build and statistics folders if not existing
if not exist ..\build mkdir ..\build
if not exist ..\statistics mkdir ..\statistics

echo [1/2] Compiling astar_alt...
g++ -std=c++17 -I..\helpers astar_alt.cpp ..\helpers\timer.cpp -o ..\build\astar_alt.exe

echo [2/2] Compiling astar_weighted...
g++ -std=c++17 -I..\helpers astar_weighted.cpp ..\helpers\timer.cpp -o ..\build\astar_weighted.exe

echo ==============================
echo Running All A* Variants
echo ==============================

echo Running astar_alt...
..\build\astar_alt.exe

echo Running astar_weighted...
..\build\astar_weighted.exe

echo ==============================
echo ✅ All A* variants completed.
echo ==============================
pause
