add_library(UserLibs STATIC
    lcd-timer/nhd0216k3z_lcd.c
)

target_include_directories(UserLibs PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/lcd-timer
)

target_link_libraries(UserLibs PUBLIC stm32cubemx)