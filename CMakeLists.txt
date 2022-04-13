cmake_minimum_required(VERSION 3.9.6)

# ---------------------------------------------------------------------------- #
# PROJECT
# name version language
# ---------------------------------------------------------------------------- #
project(hlscnn VERSION 0.1 LANGUAGES CXX)

# ---------------------------------------------------------------------------- #
# VARIABLE
# alias and configurations
# ---------------------------------------------------------------------------- #
set(MyTarget ${PROJECT_NAME})

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

# ---------------------------------------------------------------------------- #
# External dependencies
# ---------------------------------------------------------------------------- #
##
## ilang
##
find_package(ilang REQUIRED)

# ---------------------------------------------------------------------------- #
# TARGET
# library
# ---------------------------------------------------------------------------- #
add_library(${MyTarget}ila
  src/hlscnn_top.cc
  src/config_state.cc
  src/config_instr.cc
  src/top_io.cc
  src/arch_state.cc
  src/internal_state.cc
  src/spad_instr.cc
  src/utils.cc
  src/conv_child_instr.cc
  src/conv_trigger_instr.cc
  src/vir_mem_instr.cc
  src/init_condition.cc
)

add_library(${PROJECT_NAME}::${MyTarget}ila ALIAS ${MyTarget}ila)

target_include_directories(${MyTarget}ila
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)
# target_include_directories(${MyTarget} PRIVATE include)

target_link_libraries(${MyTarget}ila ilang::ilang)
# ---------------------------------------------------------------------------- #
# TARGET
# executable
# ---------------------------------------------------------------------------- #
add_executable(${MyTarget} 
  app/main.cc
)

target_link_libraries(${MyTarget} PUBLIC ${MyTarget}ila)
