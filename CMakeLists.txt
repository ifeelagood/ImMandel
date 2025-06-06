cmake_minimum_required(VERSION 3.27) # visual studio 2022 cmake @ 17/24


set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

project(ImMandelCore LANGUAGES CXX)

# static core library. reusable. public can be unit tested
add_library(ImMandelCore STATIC)
target_sources(ImMandelCore
PRIVATE 
    "src/D3D.cpp"
    "src/Window.cpp"
    "src/GUI.cpp"
    "src/ImGui_mpfr.cpp"
PUBLIC
    "src/D3D.h"
    "src/Window.h"
    "src/GUI.h"
    "src/Engine.h"
    "src/EscapeTimeEngine.h"
    "src/Transform.h"
    "src/Shader.h"
    "src/ShaderParameters.h"
    "src/Surface.h"
    "src/ImGui_mpfr.h"
)

target_include_directories(ImMandelCore PUBLIC src)

add_executable(ImMandelApp
    "src/TransformGUI.h"
    "src/Application.h"
    "src/Application.cpp"
    "src/main.cpp"
)

target_link_libraries(ImMandelApp PRIVATE ImMandelCore)

# vcpkg libraries

# https://cmake.org/cmake/help/latest/command/find_package.html


find_package(imgui CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(Boost REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(gmp REQUIRED IMPORTED_TARGET gmp)
pkg_check_modules(mpfr REQUIRED IMPORTED_TARGET mpfr)


target_link_libraries(ImMandelCore 
PUBLIC 
    imgui::imgui Eigen3::Eigen Boost::boost PkgConfig::gmp PkgConfig::mpfr
    d3d11.lib dxgi.lib d3dcompiler.lib
)

# shaders
file(COPY ${CMAKE_SOURCE_DIR}/res/shaders DESTINATION ${CMAKE_BINARY_DIR}/res)


if (MSVC)   
    # core guidelines check
    #if (DEFINED ENV{VSINSTALLDIR})
    #    set(CPP_CORECHECK_RULESET "$ENV{VSINSTALLDIR}\\Team Tools\\Static Analysis Tools\\Rule Sets\\CppCoreCheckRules.ruleset")
    #    if(EXISTS "${CPP_CORECHECK_RULESET}")
    #        add_compile_options("/analyze:ruleset=${CPP_CORECHECK_RULESET}")
    #    else()
    #        message(WARNING "CppCoreCheckRules.ruleset not found at expected location: ${CPP_CORECHECK_RULESET}")
    #    endif()
    #else()
    #    message(WARNING "VSINSTALLDIR not set. Skipping ruleset for Core Guidelines.")
    #endif()


    set(CMAKE_CXX_FLAGS_RELEASE "/O2")

    add_compile_options(/EHsc) # exceptions
    add_compile_options(/arch:AVX2)
    add_compile_options(/permissive- /W4 /analyze)
    #add_compile_options(/analyze:plugin EspXEngine.dll)
    #add_compile_options(/analyze:plugin CppCoreCheck.dll)
    #add_compile_options(/analyze:ruleset "${CPP_CORECHECK_RULESET}")
endif()

# enable testing 
enable_testing()
add_subdirectory(tests)