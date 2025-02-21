cmake_minimum_required(VERSION 3.27) # visual studio 2022 cmake @ 17/24

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
    ${IMMANDEL_SOURCE_DIR}/main.cpp
)
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

add_executable(${PROJECT_NAME} ${IMMANDEL_SOURCE})

foreach(LIBRARY ${IMMANDEL_LIBRARIES})
    add_subdirectory("${IMMANDEL_LIBRARIES_DIR}/${LIBRARY}")
endforeach(LIBRARY)


####################
### Dependencies ###
####################

# https://cmake.org/cmake/help/latest/command/find_package.html


find_package(imgui CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)

set(IMMANDEL_PACKAGE_LIBRARIES
    imgui::imgui
    Eigen3::Eigen
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${IMMANDEL_LIBRARIES} ${PACKAGES_PROVIDES})