#!/usr/bin/env bash

PLATFORM_SRC_DIR="platform"
GAME_SRC_DIR="game"
COMMON_SRC_DIR="common"
PLATFORM_OUT_NAME="main"
GAME_OUT_NAME="libgame.so"
SFML_INCLUDE="../SFML/include"
SFML_LIB="../SFML/lib"
COMPILER="/usr/bin/g++"

echo "Building project..."

mkdir -p debug

SFML_LINK_FLAGS="-lsfml-window-s-d -lsfml-graphics-s-d -lsfml-audio-s-d -lsfml-system-s-d -lX11 -lXrandr -lXcursor -lXi -lGL -ludev -lpthread -lopenal -lvorbisenc -lvorbisfile -lvorbis -logg -lFLAC -lfreetype"
WARNING_FLAGS="-Wall -Wextra -Wconversion -Wsign-conversion -Wno-unused-parameter -Wno-unused-function -Werror -Wno-unused-variable -Wno-unused-but-set-variable"
FLAGS="-g -march=native -fno-stack-protector -ftree-vectorize -ffast-math -fno-rtti -fno-exceptions -static-libgcc -static-libstdc++"
DEFINES="-DPLATFORM_LINUX=1 -DDEBUG=1 -DSFML_STATIC=1"

if [[ ! -f debug/${PLATFORM_OUT_NAME} || $(find ${PLATFORM_SRC_DIR} ${COMMON_SRC_DIR} -type f -newer debug/${PLATFORM_OUT_NAME} -print -quit | wc -l)  -gt 0 ]];  then
  ${COMPILER} "${PLATFORM_SRC_DIR}/main_linux.cpp" ${FLAGS} ${SFML_LINK_FLAGS} ${WARNING_FLAGS} ${DEFINES} -I"${SFML_INCLUDE}" -L"${SFML_LIB}" -o debug/${PLATFORM_OUT_NAME}
  if [ $? -eq 0 ]; then
    echo "Platform Build successful! Executable is at: debug/${PLATFORM_OUT_NAME}"
  else
    echo "Platform Build failed. Check errors above."
  fi
else
	echo "Platform executable is up to date."
fi

if [[ ! -f debug/${GAME_OUT_NAME} || $(find ${GAME_SRC_DIR} ${COMMON_SRC_DIR} -type f -newer debug/${GAME_OUT_NAME} -print -quit | wc -l) -gt 0 ]]; then
  ${COMPILER} -c -fPIC ${FLAGS} ${WARNING_FLAGS} "${GAME_SRC_DIR}/game.cpp" -o "debug/game.o"
  if [ $? -eq 0 ]; then
    ${COMPILER} -shared -o "debug/${GAME_OUT_NAME}" "debug/game.o"
    if [ $? -eq 0 ]; then
      echo "Game Build successful! Library is at: debug/${GAME_OUT_NAME}"
    fi
  else
    echo "Game Build failed. Check errors above."
  fi
else
  echo "Game library is up to date."
fi

rsync -auv "Resources" "debug"

