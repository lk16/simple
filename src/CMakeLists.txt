cmake_minimum_required(VERSION 2.8)

project(simple)

find_package(PkgConfig)

set(FLAGS
    " -std=c11 -I .. -pthread -Wno-pedantic -Weverything -Wno-padded \
    -Wno-format-nonliteral -fcolor-diagnostics"
)

set(DEBUG_FLAGS "${FLAGS} -g")
set(OPTMISING_FLAGS "${FLAGS} -O3 -fforce-addr -ftree-vectorize -funroll-loops")

set(CMAKE_C_FLAGS
  #${OPTMISING_FLAGS}
  ${DEBUG_FLAGS}
)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

ADD_EXECUTABLE(simple
    builtin_types.c
    main.c
    simple_hashtable.c
    simple_error.c
    simple_string.c
    type.c
)
