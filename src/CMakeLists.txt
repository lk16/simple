cmake_minimum_required(VERSION 2.8)

project(simple)

find_package(PkgConfig)

set(FLAGS
" -std=c99 -I .. -pthread -Weverything"
)

set(DEBUG_FLAGS "${FLAGS} -g")
set(OPTMISING_FLAGS "${FLAGS} -O3 -fforce-addr -ftree-vectorize -funroll-loops")

set(CMAKE_C_FLAGS
  #${OPTMISING_FLAGS}
  ${DEBUG_FLAGS}
)

FILE(GLOB SRCFILES *.c)
ADD_EXECUTABLE(../simple ${SRCFILES})
