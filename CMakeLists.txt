cmake_minimum_required(VERSION 3.27) # visual studio 2022 cmake @ 17/24

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "/arch:AVX2")
set(CMAKE_CXX_FLAGS_RELEASE "/O2")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

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
    "src/main.cpp"
    "src/D3D.h"
    "src/D3D.cpp"
    "src/Window.h"
    "src/Window.cpp"
    "src/Application.h"
    "src/Application.cpp"
#    "src/Engine.h"
    "src/MappedRegion.h"
#    "src/Transform.h"
    "src/GUI.h"
    "src/GUI.cpp"
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

# shaders

file(COPY ${CMAKE_SOURCE_DIR}/res/shaders DESTINATION ${CMAKE_BINARY_DIR}/res)

# c++ guideline check
if (MSVC)
    # Get the VC tools install path
    execute_process(
        COMMAND "${CMAKE_COMMAND}" -E env
        OUTPUT_VARIABLE ENV_OUT
    )

    if (DEFINED ENV{VCToolsInstallDir})
        set(VCTOOLS_INSTALL_DIR "$ENV{VCToolsInstallDir}")
    else()
        message(FATAL_ERROR "VCToolsInstallDir is not defined. Ensure you're running from a VS developer environment.")
    endif()

    # Path to ruleset (optional: adjust which ruleset to use)
    set(CPP_CORECHECK_RULESET "${VCTOOLS_INSTALL_DIR}/bin/Hostx64/x64/codeanalysis/CppCoreCheckRules.ruleset")

    # Apply flags
    add_compile_options(/permissive- /W4 /analyze)
    add_compile_options(/analyze:plugin EspXEngine.dll)
    add_compile_options(/analyze:plugin CppCoreCheck.dll)
    add_compile_options(/analyze:ruleset "${CPP_CORECHECK_RULESET}")
endif()