cmake_minimum_required(VERSION 2.8)

project(simple)

find_package(PkgConfig)

set(FLAGS
" -std=c11 -I .. -pthread -Weverything -Wno-padded -Wno-format-nonliteral -fcolor-diagnostics"
)

set(DEBUG_FLAGS "${FLAGS} -g")
set(OPTMISING_FLAGS "${FLAGS} -O3 -fforce-addr -ftree-vectorize -funroll-loops")

set(CMAKE_C_FLAGS
  #${OPTMISING_FLAGS}
  ${DEBUG_FLAGS}
)

ADD_EXECUTABLE(simple
    main.c
    simple_types.c
    type.c
)
