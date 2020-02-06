mkdir build
cd build
REM cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
REM cmake -G "Visual Studio 16 2019" -A Win32 -DCMAKE_BUILD_TYPE=Debug ../
cmake -G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=Release ../
REM cmake -G "Visual Studio 15 2017" -DCMAKE_BUILD_TYPE=Release ../
REM mingw32-make
REM cd ..