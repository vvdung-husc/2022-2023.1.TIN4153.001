@echo off
set CC=g++
set SRC_COMMON=../_COMMON

%CC% network.cpp ^
     %SRC_COMMON%/Log.cpp ^
     %SRC_COMMON%/Utils.cpp ^
-l ws2_32 ^
-o network.exe
REM g++ main.cpp ../../_src/Log.cpp -o t.exe