CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(simple)

SET(CMAKE_C_COMPILER "/usr/bin/clang")

SET(FLAGS
    " -std=c11 -I .. -pthread -Wno-pedantic -Weverything -Wno-padded \
    -Wno-format-nonliteral -fcolor-diagnostics"
)

SET(DEBUG_FLAGS "${FLAGS} -g")
SET(OPTMISING_FLAGS "${FLAGS} -O3 -fforce-addr -ftree-vectorize -funroll-loops")

SET(CMAKE_C_FLAGS
  #${OPTMISING_FLAGS}
  ${DEBUG_FLAGS}
)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

ADD_LIBRARY(supersimple STATIC
    builtin_types.c
    simple_error.c
    simple_hashtable.c
    simple_object.c
    simple_string.c
    simple_test.c
    type.c
)

ADD_EXECUTABLE(
    simple
    main.c
)

TARGET_LINK_LIBRARIES(
    simple
    m
    supersimple
)

ADD_EXECUTABLE(
    test_simple
    test/main.c
)

TARGET_LINK_LIBRARIES(
    test_simple
    m
    supersimple
)
