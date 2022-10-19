@echo off
set CC=g++
set SRC_COMMON=../_COMMON

%CC% ThanhNamNET.cpp ^
     %SRC_COMMON%/Log.cpp ^
-l ws2_32 ^
-o ThanhNamNET.exe
REM g++ main.cpp ../../_src/Log.cpp -o t.exe