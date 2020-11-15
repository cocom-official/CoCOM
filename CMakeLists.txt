cmake_minimum_required(VERSION 3.5)

file(STRINGS "${CMAKE_SOURCE_DIR}/makespec/VERSION" CoCOM_VERSION)
file(STRINGS "${CMAKE_SOURCE_DIR}/makespec/BUILD_VERSION" CoCOM_BUILD_VERSION)
file(STRINGS "${CMAKE_SOURCE_DIR}/makespec/VERSION_SUFFIX" CoCOM_VERSION_SUFFIX)

set(CoCOM_VERSION_STRING "${CoCOM_VERSION}.${CoCOM_BUILD_VERSION}")
set(CoCOM_VERSION_STRING_WITH_SUFFIX "${CoCOM_VERSION}.${CoCOM_BUILD_VERSION}${CoCOM_VERSION_SUFFIX}")
set(CoCOM_VENDER "CoCOM-Official")

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
add_definitions(-DCoCOM_VERSION_STRING_WITH_SUFFIX="${CoCOM_VERSION_STRING_WITH_SUFFIX}")

# For Windows RC file.
set(rc_flag_major   "-DCOCOM_VERSION_MAJOR=${CMAKE_PROJECT_VERSION_MAJOR}")
set(rc_flag_minor   "-DCOCOM_VERSION_MINOR=${CMAKE_PROJECT_VERSION_MINOR}")
set(rc_flag_bugfix  "-DCOCOM_VERSION_BUGFIX=${CPACK_PACKAGE_VERSION_PATCH}")
set(rc_flag_build   "-DCOCOM_VERSION_BUILD=${CoCOM_BUILD_VERSION}")
set(rc_flag_ver_str "-DCOCOM_VERSION_STRING=${CoCOM_VERSION_STRING_WITH_SUFFIX}")
set(rc_flag_vender_str "-DCOCOM_VENDER=${CoCOM_VENDER}")


message(" ")
message("CoCOM Version: ${CoCOM_VERSION_STRING_WITH_SUFFIX}")
message("CoCOM Build Version: ${CoCOM_BUILD_VERSION}")
message("┌─────────────────────────────────────────────────────────┐")
message("│ CoCOM, A Cross Platform Serial Port Utility.            │")
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
                           COMMAND rc.exe /fo ${WINDOWS_RES_FILE} ${rc_list} 
                           ${rc_flag_major} ${rc_flag_minor} ${rc_flag_bugfix} ${rc_flag_build} 
                           ${rc_flag_ver_str} ${rc_flag_vender_str}
                           WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    else()
        add_custom_command(OUTPUT ${WINDOWS_RES_FILE}
                           COMMAND windres.exe ${rc_list} ${WINDOWS_RES_FILE} 
                           ${rc_flag_major} ${rc_flag_minor} ${rc_flag_bugfix} ${rc_flag_build} 
                           ${rc_flag_ver_str} ${rc_flag_vender_str}
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

if (CMAKE_BUILD_TYPE STREQUAL Release) # Release

    set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
    set(CPACK_PACKAGE_VENDOR ${CoCOM_VENDER})
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CoCOM -- Serial Port Utility")
    set(CPACK_PACKAGE_VERSION ${CoCOM_VERSION_STRING})
    set(CPACK_PACKAGE_VERSION_MAJOR ${CMAKE_PROJECT_VERSION_MAJOR})
    set(CPACK_PACKAGE_VERSION_MINOR ${CMAKE_PROJECT_VERSION_MINOR})
    set(CPACK_PACKAGE_VERSION_PATCH ${CPACK_PACKAGE_VERSION_PATCH})
    set(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME})
    set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE.txt)
    install(FILES ${CMAKE_SOURCE_DIR}/LICENSE.txt DESTINATION /)

    set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/assets/logos\\\\CoCOM.bmp")
    set(CPACK_PACKAGE_EXECUTABLES ${PROJECT_NAME} ${PROJECT_NAME})
    set(CPACK_CREATE_DESKTOP_LINKS ${PROJECT_NAME})


    if (WIN32) # WIN32
        set(pack_dir CoCOM)
        add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND bash -c "echo post build"
            COMMAND bash -c "mkdir -p ${pack_dir}"
            COMMAND bash -c "cp CoCOM.exe ${pack_dir}"
            COMMAND python3 "${CMAKE_SOURCE_DIR}/tools/mingw-bundledlls/mingw-bundledlls" --copy "${CMAKE_BINARY_DIR}/${pack_dir}/CoCOM.exe"
            COMMAND bash -c "windeployqt ${CMAKE_BINARY_DIR}/${pack_dir}/CoCOM.exe --no-translations"
        )

        install(DIRECTORY "${CMAKE_BINARY_DIR}/${pack_dir}/" DESTINATION /)

        set(CPACK_WIX_UPGRADE_GUID "58DD5230-1D15-41A7-BAA5-1D1E4DCBBF04")
        set(CPACK_WIX_MODIFY_PATH ON)
        set(CPACK_WIX_CULTURES "zh-CN;en-US")
        set(CPACK_WIX_PRODUCT_ICON "${CMAKE_SOURCE_DIR}/assets/logos/CoCOM.ico")
        set(CPACK_WIX_UI_BANNER "${CMAKE_SOURCE_DIR}/assets/wix_banner.png")
        set(CPACK_WIX_UI_DIALOG "${CMAKE_SOURCE_DIR}/assets/wix_dialog.png")

    else() # WIN32

    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION / PERMISSIONS WORLD_EXECUTE)

    endif() # WIN32

INCLUDE(CPack)
endif() # Release
