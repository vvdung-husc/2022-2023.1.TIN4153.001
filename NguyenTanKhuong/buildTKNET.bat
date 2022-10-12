@echo off
set CC=g++
set SRC_COMMON=../_COMMON

%CC% TanKhuongNET.cpp ^
     %SRC_COMMON%/Log.cpp ^
-l ws2_32 ^
-o TanKhuongNET.exe
REM g++ main.cpp ../../_src/Log.cpp -o t.exe