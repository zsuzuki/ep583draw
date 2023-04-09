cmake_minimum_required(VERSION 3.12)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_SDK_PATH "../../pico-sdk")
include(pico_sdk_import.cmake)
project(picoe583b)

pico_sdk_init()

include_directories(./include)

if(CMAKE_HOST_APPLE)
include_directories(/opt/homebrew/include)
endif()

# generate an executable file
add_executable(${PROJECT_NAME}
    src/DEV_Config.c
    src/EPD_5in83b_V2.c
    src/GUI_Paint.c
    src/font.cpp
    src/main.cpp
)

pico_set_program_name(${PROJECT_NAME} "Pico ePaper Test")
pico_set_program_version(${PROJECT_NAME} "0.1")

# enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

target_link_libraries(${PROJECT_NAME} pico_stdlib hardware_spi)

pico_add_extra_outputs(${PROJECT_NAME})
