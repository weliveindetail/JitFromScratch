@echo off
clang -c -o mangle_ref_win.obj mangle_ref.cpp
dumpbin /SYMBOLS mangle_ref_win.obj
