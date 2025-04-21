cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PICO_BOARD pico_w CACHE STRING "Board type")
include(pico_sdk_import.cmake)

project(ControleAeronave C CXX ASM)
pico_sdk_init()

add_executable(ControleAeronave ControleAeronave.c lib/ssd1306.c)

pico_set_program_name(ControleAeronave "ControleAeronave")
pico_set_program_version(ControleAeronave "0.1")

pico_enable_stdio_uart(ControleAeronave 1)
pico_enable_stdio_usb(ControleAeronave 1)

target_link_libraries(ControleAeronave
        pico_stdlib
        hardware_adc
        hardware_pwm
        hardware_pio
        hardware_i2c
        )

target_include_directories(ControleAeronave PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

#Gerar arquivo cabeçalho pio
pico_generate_pio_header(${CMAKE_PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/pio_matrix.pio)

pico_add_extra_outputs(ControleAeronave)

