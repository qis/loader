cmake_minimum_required(VERSION 3.6 FATAL_ERROR)
project(loader VERSION 0.1.0 LANGUAGES CXX)

# Project
set(PROJECT "Loader")
set(PRODUCT "Matrix")
set(COMPANY "Xiphos")
set(COPYING "2017 Alexej Harm")

# Configuration
configure_file(res/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

# Compiler Settings
set(CMAKE_CXX_FLAGS "/permissive- /std:c++latest ${CMAKE_CXX_FLAGS} /utf-8")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /manifestuac:NO /ignore:4099 /ignore:4098")
add_definitions(-D_UNICODE -DUNICODDE -DWIN32_LEAN_AND_MEAN -DNOMINMAX)
add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -D_ATL_SECURE_NO_DEPRECATE)
add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
add_definitions(-DWINVER=0x0A00 -D_WIN32_WINNT=0x0A00)

# Output
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Packages
file(GLOB CMAKE_PREFIX_PATH LIST_DIRECTORIES true third_party/*)
find_package(compat REQUIRED)

# Soureces
file(GLOB_RECURSE src src/*.h src/*.cpp)
foreach(file IN ITEMS ${src})
  file(RELATIVE_PATH file "${CMAKE_CURRENT_SOURCE_DIR}" "${file}")
  get_filename_component(path "${file}" PATH)
  string(REPLACE "/" "\\" path "${path}")
  source_group("${path}" FILES "${file}")
endforeach()

# Resources
file(GLOB res res/resource.h res/resource.rc res/resource.manifest)
source_group(res FILES ${res})

# Precompiled Header
set(common ${CMAKE_CURRENT_BINARY_DIR}/common.pch)
set_source_files_properties(${src} PROPERTIES COMPILE_FLAGS "/Yu\"common.h\" /FI\"common.h\" /Fp\"${common}\"" OBJECT_DEPENDS "${common}")
set_source_files_properties(src/common.cpp PROPERTIES COMPILE_FLAGS "/Yc\"common.h\" /Fp\"${common}\"" OBJECT_OUTPUTS "${common}")

# Application
add_executable(${PROJECT_NAME} WIN32 ${src} ${res})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR} src res)
target_link_libraries(${PROJECT_NAME} PRIVATE compat)

# Install
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# Visual Studio
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER build)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})