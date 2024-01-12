cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)
project(test_project C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()
add_executable(milestone2
  milestone2.c
)

target_include_directories(milestone2 PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Uncomment if you want to flash your board via picotool.
# pico_enable_stdio_usb(milestone2 1)
pico_add_extra_outputs(milestone2)

target_link_libraries(milestone2
  pico_stdlib
  # Uncomment when using Pico W
  pico_cyw43_arch_none
  )

target_link_libraries(milestone2 pico_stdlib hardware_adc hardware_pwm)


