file(GLOB TUIO_OSC_SOURCE oscpack/osc/*.cpp)
file(GLOB TUIO_IP_SOURCE oscpack/ip/*.cpp oscpack/ip/posix/*.cpp)
file(GLOB TUIO_SOURCES TUIO/*.cpp)
file(GLOB TUIO_HEADERS TUIO/*.h)

add_library(TUIO_OSC ${TUIO_OSC_SOURCE})
add_library(TUIO_IP ${TUIO_IP_SOURCE})

target_include_directories(TUIO_IP PUBLIC oscpack)

set(TUIO_INCLUDE_DIRS TUIO oscpack)

add_library(TUIO ${TUIO_SOURCES} ${TUIO_HEADERS})
target_include_directories(TUIO PUBLIC ${TUIO_INCLUDE_DIRS})

target_link_libraries(TUIO PUBLIC TUIO_OSC TUIO_IP)