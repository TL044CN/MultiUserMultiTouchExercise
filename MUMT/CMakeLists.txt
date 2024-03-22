
set(MUMT_INCLUDE_DIRS include)

add_library(MUMT
    source/Helpers.cpp
    source/Finger.cpp
    source/QuadTree.cpp
    source/Tracker.cpp
    source/TUIOServerAdapter.cpp
)


target_include_directories(MUMT
PUBLIC
    ${MUMT_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    ${TUIO_INCLUDE_DIRS}
)

target_link_libraries(MUMT
    PUBLIC
        ${OpenCV_LIBS}
        TUIO
)

#add_compile_definitions(DEBUG=1)
