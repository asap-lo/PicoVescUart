CMake_minimum_required(VERSION 3.13...3.27)

set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

# initialize pico-sdk from submodule
# note: this must happen before project()
include(pico-sdk/pico_sdk_init.cmake)

project(my_project)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project

set( DPICO_SDK_PATH /home/jack/PicoVescUart/pico-sdk )

add_executable(pico_vesc_uart
	pico_vesc_uart.c
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(pico_vesc_uart pico_stdlib)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(pico_vesc_uart)

