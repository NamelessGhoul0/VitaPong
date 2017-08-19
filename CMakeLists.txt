cmake_minimum_required(VERSION 2.8)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

set(SHORT_NAME Pong)
project(${SHORT_NAME})
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(VITA_APP_NAME "Pong")
set(VITA_TITLEID  "VITAPONG0")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(
)

add_executable(${SHORT_NAME}
  src/main.c
  src/audio.cpp
  src/menu.c
  src/credits.c
  src/font.c
  src/gekihen.c
  src/splash.c
)

target_link_libraries(${SHORT_NAME}
  vita2d
  SceDisplay_stub
  SceGxm_stub
  SceSysmodule_stub
  SceCommonDialog_stub
  SceCtrl_stub
  SceTouch_stub
  SceAudio_stub
  freetype
  png
  soloud
  pthread
  m
  z
)

vita_create_self(${SHORT_NAME}.self ${SHORT_NAME})
vita_create_vpk(${SHORT_NAME}.vpk ${VITA_TITLEID} ${SHORT_NAME}.self
  VERSION ${VITA_VERSION}
  NAME ${VITA_APP_NAME}
  FILE res/icon0.png sce_sys/icon0.png
  FILE res/template.xml sce_sys/livearea/contents/template.xml
  FILE res/startup.png sce_sys/livearea/contents/startup.png
  FILE res/bg0.png sce_sys/livearea/contents/bg0.png
  FILE res/pong_paddle.wav pong_paddle.wav
  FILE res/pong_score.wav pong_score.wav
  FILE res/pong_wall.wav pong_wall.wav
)
