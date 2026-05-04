#!/bin/bash
g++ -I"${workspaceFolder}/include" -std=c++17 $(find ${workspaceFolder} -name '*.cpp') -o ${workspaceFolder}/$1.exe liballegro_monolith-debug.dll.a -static
