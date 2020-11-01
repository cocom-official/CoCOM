cmake_minimum_required(VERSION 3.5)

file(STRINGS "${CMAKE_SOURCE_DIR}/makespec/VERSION" CoCOM_VERSION)
file(STRINGS "${CMAKE_SOURCE_DIR}/makespec/BUILD_VERSION" CoCOM_BUILD_VERSION)
file(STRINGS "${CMAKE_SOURCE_DIR}/makespec/VERSION_SUFFIX" CoCOM_VERSION_SUFFIX)

set(CoCOM_VERSION_STRING "${CoCOM_VERSION}.${CoCOM_BUILD_VERSION}${CoCOM_VERSION_SUFFIX}")

set(VERSION_LIST ${CoCOM_VERSION})
string(REPLACE "." ";" VERSION_LIST ${VERSION_LIST})
separate_arguments(VERSION_LIST)

list(GET VERSION_LIST 0 CMAKE_PROJECT_VERSION_MAJOR)
list(GET VERSION_LIST 1 CMAKE_PROJECT_VERSION_MINOR)
list(GET VERSION_LIST 2 CPACK_PACKAGE_VERSION_PATCH)


add_definitions(-DCOCOM_VERSION_MAJOR=${CMAKE_PROJECT_VERSION_MAJOR})
add_definitions(-DCOCOM_VERSION_MINOR=${CMAKE_PROJECT_VERSION_MINOR})
add_definitions(-DCOCOM_VERSION_BUGFIX=${CPACK_PACKAGE_VERSION_PATCH})
add_definitions(-DCOCOM_VERSION_BUILD=${CoCOM_BUILD_VERSION})
add_definitions(-DCOCOM_VERSION_STRING="${CoCOM_VERSION_STRING}")

# For Windows RC file.
set(rc_flag_major   "-DCOCOM_VERSION_MAJOR=${CMAKE_PROJECT_VERSION_MAJOR}")
set(rc_flag_minor   "-DCOCOM_VERSION_MINOR=${CMAKE_PROJECT_VERSION_MINOR}")
set(rc_flag_bugfix  "-DCOCOM_VERSION_BUGFIX=${CPACK_PACKAGE_VERSION_PATCH}")
set(rc_flag_build   "-DCOCOM_VERSION_BUILD=${CoCOM_BUILD_VERSION}")
set(rc_flag_ver_str "-DCOCOM_VERSION_STRING=${CoCOM_VERSION_STRING}")


message("CoCOM Version: ${rc_flag}")
message(" ")
message("CoCOM Version: ${CoCOM_VERSION_STRING}")
message("CoCOM Build Version: ${CoCOM_BUILD_VERSION}")
message("┌─────────────────────────────────────────────────────────┐")
message("│ CoCOM, A Cross Platform Serial port tool.               │")
message("│ Licenced under GPLv3                                    │")
message("│                                                         │")
message("│ You may only use this program to the extent             │")
message("│ permitted by local law.                                 │")
message("│                                                         │")
message("│ See: https://www.gnu.org/licenses/gpl-3.0.html          │")
message("├─────────────────────────────────────────────────────────┤")
message("│ Project Homepage: https://github.com/cocom-official     │")
message("│ Welcome to contribute!                                  │")
message("└─────────────────────────────────────────────────────────┘")
message(" ")


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

if (WIN32)
find_package(Qt5 COMPONENTS  WinExtras REQUIRED)
link_libraries(Qt5::WinExtras)
endif()

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
        COMMAND rc.exe /fo ${WINDOWS_RES_FILE} ${rc_list} ${rc_flag_major} ${rc_flag_minor} ${rc_flag_bugfix} ${rc_flag_build} ${rc_flag_ver_str}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    else()
        add_custom_command(OUTPUT ${WINDOWS_RES_FILE}
        COMMAND windres.exe ${rc_list} ${WINDOWS_RES_FILE} ${rc_flag_major} ${rc_flag_minor} ${rc_flag_bugfix} ${rc_flag_build} ${rc_flag_ver_str}
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
