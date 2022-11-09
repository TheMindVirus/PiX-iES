cmake_minimum_required(VERSION 3.0)

project(cube)

add_executable(${PROJECT_NAME} main.c logo.o)

target_link_libraries(${PROJECT_NAME} PRIVATE
    pspgu
    pspgum
    pspge
    pspdisplay
)

create_pbp_file(
    TARGET ${PROJECT_NAME}
    ICON_PATH NULL
    BACKGROUND_PATH NULL
    PREVIEW_PATH NULL
    TITLE ${PROJECT_NAME}
)
