cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_SDK_PATH "${CMAKE_CURRENT_LIST_DIR}/pico-sdk")
include(${PICO_SDK_PATH}/pico_sdk_init.cmake)

################################################################
# Edit the following three lines to fit your specific use case #
################################################################

# https://github.com/raspberrypi/pico-sdk/blob/ee68c78d0afae2b69c03ae1a72bf5cc267a2d94c/README.md?plain=1#L187-L192
set(PICO_BOARD pico CACHE STRING "Board type")

# Change `Example` to whatever your project/filename is called,
# or feel free to leave it as example. Just add what you need.
project(Example C CXX ASM)
add_executable(${CMAKE_PROJECT_NAME} ${CMAKE_PROJECT_NAME}.c)


pico_sdk_init()

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.4.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.4.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

pico_set_program_name(${CMAKE_PROJECT_NAME} "${CMAKE_PROJECT_NAME}")
pico_set_program_version(${CMAKE_PROJECT_NAME} "0.1")

pico_enable_stdio_uart(${CMAKE_PROJECT_NAME} 0)
pico_enable_stdio_usb(${CMAKE_PROJECT_NAME} 1)

pico_add_extra_outputs(${CMAKE_PROJECT_NAME})

add_subdirectory(vesc)

add_library(PicoVescUart PicoVescUart.c)

target_link_libraries(PicoVescUart
pico_stdlib
pico_multicore
hardware_uart
vesc
)

target_link_libraries(${CMAKE_PROJECT_NAME} PicoVescUart)

target_include_directories(PicoVescUart PRIVATE
${CMAKE_CURRENT_LIST_DIR}
${CMAKE_CURRENT_LIST_DIR}/vesc
)