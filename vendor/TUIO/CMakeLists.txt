file(GLOB TUIO_SOURCES TUIO/*.cpp)
file(GLOB TUIO_HEADERS TUIO/*.h)

set(TUIO_INCLUDE_DIRS TUIO oscpack)

add_library(TUIO ${TUIO_SOURCES} ${TUIO_HEADERS})
target_include_directories(TUIO PUBLIC ${TUIO_INCLUDE_DIRS})
