# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "${CMAKE_CURRENT_LIST_DIR}/pico-sdk")

set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.4.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.4.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

project(PicoVescUart C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()



# set(SPI_TFT_PORT "spi0" CACHE STRING "TFT spi port-name")
# set(SPI_TFT_CS   "17"    CACHE STRING "TFT CS  pin number")
# set(SPI_TFT_DC   "16"   CACHE STRING "TFT DC  pin number")
# set(SPI_TFT_RST  "15"   CACHE STRING "TFT RST pin number")
# set(TFT_OPTIONS TFT_ENABLE_BLACK TFT_ENABLE_RESET TFT_ENABLE_TEXT TFT_ENABLE_SHAPES
#                 TFT_ENABLE_ROTATE TFT_ENABLE_SCROLL
#             CACHE STRING "TFT options/functions")

# add_compile_definitions(SPI_TFT_PORT=${SPI_TFT_PORT})
# add_compile_definitions(SPI_TFT_CS=${SPI_TFT_CS})
# add_compile_definitions(SPI_TFT_DC=${SPI_TFT_DC})
# add_compile_definitions(SPI_TFT_RST=${SPI_TFT_RST})
# foreach(opt IN LISTS TFT_OPTIONS)
#     message(">>> using: ${opt}")
#     add_compile_definitions(${opt})
# endforeach(opt)

# # ENABLE_FONTS: set on command-line or select from
# # VSCode-menu (see .vscode/cmake-variants.json)
# if(ENABLE_FONTS)
#   message(">>> Font test")
#   add_compile_definitions(TFT_ENABLE_FONTS ENABLE_TEST9 ENABLE_TEST9A)
# else()
#   message(">>> Normal test")
#   add_compile_definitions(ENABLE_TESTR ENABLE_TEST1 ENABLE_TEST1A
#                            ENABLE_TEST2 ENABLE_TEST3 ENABLE_TEST4
#                            ENABLE_TEST5 ENABLE_TEST6 ENABLE_TEST7)
# endif()

# # add preprocessor-constant DEBUG for Debug-builds
# if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#   set(CMAKE_VERBOSE_MAKEFILE 1)
#   add_compile_definitions(DEBUG)
# else()
# endif()
# Add executable. Default name is the project name, version 0.1
add_executable(PicoVescUart PicoVescUart.c )

pico_set_program_name(PicoVescUart "PicoVescUart")
pico_set_program_version(PicoVescUart "0.1")

pico_enable_stdio_uart(PicoVescUart 0)
pico_enable_stdio_usb(PicoVescUart 1)

# Add the standard include files to the build
target_include_directories(PicoVescUart PRIVATE
${CMAKE_CURRENT_LIST_DIR}
${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
${CMAKE_CURRENT_LIST_DIR}/pico-st7735
${CMAKE_CURRENT_LIST_DIR}/vesc
)

# Add subdirectory, makes it work for some reason
add_subdirectory(vesc)
add_subdirectory(pico-st7735)

# library configuration (override if necessary)
set(SPI_TFT_PORT "spi0" CACHE STRING "TFT spi port-name")
set(SPI_TFT_CS   "17"    CACHE STRING "TFT CS  pin number")
set(SPI_TFT_DC   "16"   CACHE STRING "TFT DC  pin number")
set(SPI_TFT_RST  "15"   CACHE STRING "TFT RST pin number")
set(TFT_OPTIONS TFT_ENABLE_SHAPES TFT_ENABLE_BLACK TFT_ENABLE_RESET TFT_ENABLE_TEXT
                TFT_ENABLE_ROTATE
            CACHE STRING "TFT options/functions")
foreach(opt IN LISTS TFT_OPTIONS)
    message(">>> using: ${opt}")
    add_compile_definitions(${opt})
endforeach(opt)

# Add the standard library and vesc library to the build
target_link_libraries(PicoVescUart
vesc
hardware_spi # Library required for example
st7735
pico_multicore
hardware_uart
pico_stdlib)


pico_add_extra_outputs(PicoVescUart)