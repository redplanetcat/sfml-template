#!/usr/bin/env bash

SRC_DIR="src"
OUT_NAME="main"
SFML_INCLUDE="../SFML/include"
SFML_LIB="../SFML/lib"

echo "Building Apples..."

mkdir -p debug

LINK_FLAGS="-lsfml-window-d -lsfml-graphics-d -lsfml-system-d -ludev -lpthread"
FLAGS="-g -Wall -Wextra -fno-stack-protector"

if [[ ! -f debug/${OUT_NAME} || $(find ${SRC_DIR} -type f -newer debug/${OUT_NAME} -print -quit | wc -l) -gt 0 ]];  then
  g++ "${SRC_DIR}/main.cpp" -I"${SFML_INCLUDE}" -L"${SFML_LIB}" ${FLAGS} ${LINK_FLAGS} -o debug/${OUT_NAME}
  if [ $? -eq 0 ]; then
    echo "Build successful! Executable is at: debug/${OUT_NAME}"
  else
    echil "Build failed. Check errors above."
  fi

else
	echo "Executable is up to date."
fi

rsync -auv "Resources" "debug"

