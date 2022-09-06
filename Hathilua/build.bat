@echo off
set CC=g++
set SRC_COMMON=../_COMMON

%CC% Biatap1.cpp ^
     %SRC_COMMON%/Log.cpp ^
-o bt1.exe
REM g++ main.cpp ../../_src/Log.cpp -o t.exe