cmake_minimum_required(VERSION 3.16.3)
project(qt3dtest)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

# include necessary qt3d modules
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets 3DCore 3DExtras 3DRender 3DInput)

## Eigen3 Library
find_package(Eigen3 REQUIRED)
add_definitions(${EIGEN_DEFINITIONS})
include_directories(SYSTEM ${EIGEN3_INCLUDE_DIR})

set(SOURCES
    auxiliary/dms.cpp
    auxiliary/cachingdms.cpp
    skypoint.cpp
    ksnumbers.cpp
    computematerial.cpp
    computeframegraph.cpp
    main.cpp
    )

add_executable(${PROJECT_NAME} ${SOURCES})

# link the qt3d libraries
target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::3DCore
    Qt5::3DExtras
    Qt5::3DRender
    Qt5::3DInput
    )