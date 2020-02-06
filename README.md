Herkulex_English
===============

Herkulex_English is an alternative API for working with Herkulex motors in C++.

Tested on:
1. Windows 10 v1903 using Visual Studio Community 2019 (v16.3.7), CMake (v3.14.4)
2. Runs on both 32 bit and 64 bit system architecture

Dependencies:
1. Enumser (v1.39) (required for auto detecting of comports)

Building (CMake)
----------------------------------
1. Double click build.bat to run CMake
2. Go build and open HerkulexTest.sln
3. Set app_test as the startup_project and run

Code Documentation
----------------------------------
Doxygen generated documentation found in "Herkulex_English\Dox_output\html\index.html"

Bugs
---------
1. Program crashes randomly when unknown error is read from Herkulex motors after running the motors for a long while. [unknown cause]

Enjoy,

Er Jie Kai (erjiekai@gmail.com)