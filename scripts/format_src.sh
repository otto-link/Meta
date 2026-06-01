#!/bin/bash

# directories to be formatted (recursive search)
DIRS="Meta/include Meta/src tests"
FORMAT_CMD="clang-format -style=file:scripts/clang_style -i {}"

for D in ${DIRS}; do
    find ${D}/. -type f \( -iname \*.hpp -o -iname \*.inl -o -iname \*.cpp \) -exec ${FORMAT_CMD} \;
done

# format cmake files
cmake-format -i CMakeLists.txt Meta/CMakeLists.txt
