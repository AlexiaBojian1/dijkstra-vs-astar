@echo off
echo ==============================
echo Compiling All Dijkstra Variants
echo ==============================

REM Create build and statistics folders if not existing
if not exist ..\build mkdir ..\build
if not exist ..\statistics mkdir ..\statistics

echo [1/4] Compiling dijk_generated...
g++ -std=c++17 -I..\helpers dijk_generated.cpp ..\helpers\timer.cpp -o ..\build\dijkstra_generated.exe

echo [2/4] Compiling dijk_lazy...
g++ -std=c++17 -I..\helpers dijk_lazy.cpp ..\helpers\timer.cpp -o ..\build\dijkstra_lazy.exe

echo [3/4] Compiling dijk_decKey...
g++ -std=c++17 -I..\helpers dijk_decKey.cpp ..\helpers\timer.cpp -o ..\build\dijkstra_decKey.exe

echo [4/4] Compiling dijk_Fib...
g++ -std=c++17 -I..\helpers dijk_Fib.cpp ..\helpers\timer.cpp -lboost_heap -o ..\build\dijkstra_fib.exe

echo ==============================
echo Running All Dijkstra Variants
echo ==============================

echo Running dijkstra_generated...
..\build\dijkstra_generated.exe

echo Running dijkstra_lazy...
..\build\dijkstra_lazy.exe

echo Running dijkstra_decKey...
..\build\dijkstra_decKey.exe

echo Running dijkstra_fib...
..\build\dijkstra_fib.exe

echo ==============================
echo ✅ All Dijkstra variants completed.
echo ==============================
pause
