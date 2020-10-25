cmake_minimum_required(VERSION 3.5)

project(CoCOM LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Gui Widgets SerialPort
             LinguistTools REQUIRED)

if (${Qt5_FOUND})
    message(STATUS "Found Qt " ${Qt5_VERSION})
else()
    message(FATAL_ERROR  "Couldn't find Qt")
endif()

link_libraries(
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::SerialPort
    )

qt5_wrap_ui(ui_list
    "ui/mainWindow.ui"
    )

qt5_wrap_cpp(moc_list
    "src/include/MainWindow.h"
    )

qt5_add_resources(qrc_list
    "assets/assets.qrc"
    ${CMAKE_BINARY_DIR}/translations.qrc
    )

set(ts_list
    "assets/translations/CoCOM_zh_CN.ts"
    "assets/translations/CoCOM_en.ts"
    )

set(ts_raw_list
    "assets/translations_raw/CoCOM_zh_CN.ts"
    "assets/translations_raw/CoCOM_en.ts"
    )

set(src_list
    "src/main.cpp"
    "src/MainWindow.cpp"
    )


# qt5_add_translation(QM_FILES ${ts_list})
qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${ts_raw_list} ${src_list})

configure_file(assets/translations.qrc ${CMAKE_BINARY_DIR} COPYONLY)

add_custom_target(translations ALL DEPENDS ${QM_FILES})
add_custom_target(qt_resources ALL DEPENDS ${qrc_list})
add_dependencies(qt_resources translations)

# Windows application icon
if (WIN32)
    set(rc_list "assets/resource.rc")
    set(WINDOWS_RES_FILE ${CMAKE_BINARY_DIR}/resources.obj)
    if (MSVC)
        add_custom_command(OUTPUT ${WINDOWS_RES_FILE}
        COMMAND rc.exe /fo ${WINDOWS_RES_FILE} ${rc_list}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    else()
        add_custom_command(OUTPUT ${WINDOWS_RES_FILE}
        COMMAND windres.exe ${rc_list} ${WINDOWS_RES_FILE}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    endif()
endif()


if (CMAKE_BUILD_TYPE STREQUAL Debug)
    add_definitions(-DDEBUG)
    add_executable(${PROJECT_NAME} ${src_list} ${ui_list} ${moc_list} ${qrc_list} 
                   ${QM_FILES} ${WINDOWS_RES_FILE})
else ()
    if (WIN32)
        add_executable(${PROJECT_NAME} WIN32 ${src_list} ${ui_list} ${moc_list} ${qrc_list} 
                       ${QM_FILES} ${WINDOWS_RES_FILE})
    else()
        add_executable(${PROJECT_NAME} ${src_list} ${ui_list} ${moc_list} ${qrc_list} 
                       ${QM_FILES} ${WINDOWS_RES_FILE})
    endif ()
endif ()

add_dependencies(${CMAKE_PROJECT_NAME} qt_resources)

target_compile_options(
    ${PROJECT_NAME} PRIVATE
    -W -Wall
    )

# find_package(PkgConfig REQUIRED)
# pkg_search_module(GLIB REQUIRED glib-2.0)

# target_include_directories(
#     ${PROJECT_NAME} PRIVATE
#     ${GLIB_INCLUDE_DIRS}
#     )

# target_link_libraries(
#     ${PROJECT_NAME} PRIVATE
#     ${GLIB_LDFLAGS}
#     )

target_include_directories(
    ${PROJECT_NAME} PRIVATE
    "src/include/"
    )
