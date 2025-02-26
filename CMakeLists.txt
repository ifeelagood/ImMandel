cmake_minimum_required(VERSION 3.27) # visual studio 2022 cmake @ 17/24

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "/arch:AVX2")
set(CMAKE_CXX_FLAGS_RELEASE "/O2")

#################
### Variables ###
#################

set(IMMANDEL_PROJECT_NAME
    ImMandel
)
set(IMMANDEL_SOURCE_DIR
    src
)
set(IMMANDEL_SOURCE
    src/main.cpp

  "src/Device.h" "src/Device.cpp")
set(IMMANDEL_LIBRARIES_DIR
    libs
)
set(IMMANDEL_LIBRARIES 
    MandelbrotEngineAVX2
)

project(${IMMANDEL_PROJECT_NAME} LANGUAGES CXX)

###############
### Project ###
###############

add_executable(${PROJECT_NAME} ${IMMANDEL_SOURCE} "src/Window.cpp")

foreach(LIBRARY ${IMMANDEL_LIBRARIES})
    add_subdirectory("${IMMANDEL_LIBRARIES_DIR}/${LIBRARY}")
endforeach(LIBRARY)

target_link_libraries(${PROJECT_NAME} PRIVATE ${IMMANDEL_LIBRARIES})


####################
### Dependencies ###
####################

# https://cmake.org/cmake/help/latest/command/find_package.html


find_package(imgui CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui Eigen3::Eigen)

# directx

target_link_libraries(${PROJECT_NAME} PRIVATE d3d11.lib dxgi.lib d3dcompiler.lib)