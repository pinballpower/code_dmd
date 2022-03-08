# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "/home/matuschd/pico/pico-sdk")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)
include(${PICO_SDK_PATH}/tools/CMakeLists.txt)

project(dmdreader C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init() 

# Add executable. Default name is the project name, version 0.1
add_executable(${PROJECT_NAME} dmdreader.c)
pico_set_program_name(${PROJECT_NAME} "dmdreader")
pico_set_program_version(${PROJECT_NAME} "0.1")

pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/spi_slave_sender.pio)

# Add the standard library to the build
target_link_libraries(${PROJECT_NAME} pico_stdlib)

# Add any user requested libraries
target_link_libraries(${PROJECT_NAME}
        hardware_spi
        hardware_dma
        hardware_pio
        hardware_timer
        hardware_watchdog
        hardware_clocks
        )

pico_add_extra_outputs(${PROJECT_NAME})

pico_enable_stdio_uart(${PROJECT_NAME} 1)
pico_enable_stdio_usb(${PROJECT_NAME} 1)