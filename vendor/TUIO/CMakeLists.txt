file(GLOB TUIO_SOURCES src/*.cpp)
file(GLOB TUIO_HEADERS src/*.h)

set(TUIO_INCLUDE_DIRS src oscpack)

add_library(TUIO ${TUIO_SOURCES})
target_include_directories(TUIO PUBLIC src oscpack)
