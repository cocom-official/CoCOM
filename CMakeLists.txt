cmake_minimum_required(VERSION 3.5)

project(CoCOM LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.git)
  FIND_PACKAGE(Git)
  IF(GIT_FOUND)
    EXECUTE_PROCESS(
      COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      OUTPUT_VARIABLE "CoCOM_SHORT_COMMIT_ID"
      ERROR_QUIET
      OUTPUT_STRIP_TRAILING_WHITESPACE)
      EXECUTE_PROCESS(
        COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        OUTPUT_VARIABLE "CoCOM_LONG_COMMIT_ID"
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE)
  ELSE(GIT_FOUND)
    SET(CoCOM_SHORT_COMMIT_ID 0)
    SET(CoCOM_LONG_COMMIT_ID 0)
  ENDIF(GIT_FOUND)
ENDIF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.git)
add_definitions(-DCOCOM_SHORT_COMMIT_ID="${CoCOM_SHORT_COMMIT_ID}")
add_definitions(-DCOCOM_LONG_COMMIT_ID="${CoCOM_LONG_COMMIT_ID}")

file(STRINGS "${CMAKE_SOURCE_DIR}/makespec/VERSION" CoCOM_VERSION)
file(STRINGS "${CMAKE_SOURCE_DIR}/makespec/BUILD_VERSION" CoCOM_BUILD_VERSION)
file(STRINGS "${CMAKE_SOURCE_DIR}/makespec/VERSION_SUFFIX" CoCOM_VERSION_SUFFIX)

set(CoCOM_VERSION_STRING "${CoCOM_VERSION}.${CoCOM_BUILD_VERSION}")
set(CoCOM_VERSION_STRING_WITH_SUFFIX "${CoCOM_VERSION}.${CoCOM_BUILD_VERSION}${CoCOM_VERSION_SUFFIX}")
set(CoCOM_VENDER "CoCOM-Official")
set(CoCOM_HOMEPAGE "https://github.com/cocom-official")
set(CoCOM_PORTABLE_FILE_NAME "CoCOM-Portable.txt")

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
add_definitions(-DCOCOM_VERSION_STRING="${CoCOM_VERSION}")
add_definitions(-DCOCOM_VERSION_STRING_WITH_SUFFIX="${CoCOM_VERSION_STRING_WITH_SUFFIX}")
add_definitions(-DCOCOM_APPLICATIONNAME="${PROJECT_NAME}")
add_definitions(-DCOCOM_VENDER="${CoCOM_VENDER}")
add_definitions(-DCOCOM_HOMEPAGE="${CoCOM_HOMEPAGE}")
add_definitions(-DCOCOM_PORTABLE_FILE_NAME="${CoCOM_PORTABLE_FILE_NAME}")
add_definitions(-DCOCOM_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

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
message("Git Commit ID: ${CoCOM_LONG_COMMIT_ID}")
message("┌─────────────────────────────────────────────────────────┐")
message("│ CoCOM, A Cross Platform Serial Port Utility.            │")
message("│ Licenced under GPLv3                                    │")
message("│                                                         │")
message("│ You may only use this program to the extent             │")
message("│ permitted by local law.                                 │")
message("│                                                         │")
message("│ See: https://www.gnu.org/licenses/gpl-3.0.html          │")
message("├─────────────────────────────────────────────────────────┤")
message("│ Project Homepage: ${CoCOM_HOMEPAGE}     │")
message("│ Welcome to contribute!                                  │")
message("└─────────────────────────────────────────────────────────┘")
message(" ")


set(CMAKE_INCLUDE_CURRENT_DIR ON)

### 1. find package ###
find_package(Qt5 COMPONENTS Core Gui Widgets SerialPort Charts
             LinguistTools REQUIRED)

# add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x050F00)

if (${Qt5_FOUND})
    message(STATUS "Found Qt " ${Qt5_VERSION})
else()
    message(FATAL_ERROR  "Couldn't find Qt")
endif()

### 2. add package to link libraries ###
link_libraries(
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::SerialPort
    Qt5::Charts
    )

if (WIN32)
find_package(Qt5 COMPONENTS  WinExtras REQUIRED)
link_libraries(Qt5::WinExtras)
endif()

qt5_wrap_ui(ui_list
    "ui/mainWindow.ui"
    "ui/configDialog.ui"
    "ui/appsDialog.ui"
    "ui/plotWindow.ui"
    "ui/plotConfigDialog.ui"
    "ui/plotConfigTabWIdget.ui"
    "ui/luaConsoleWindow.ui"
    )

qt5_wrap_cpp(moc_list
    "source/inc/Common.h"

    "source/inc/Settings/GlobalSettings.h"
    "source/inc/Settings/CommandSettings.h"

    "source/inc/Media/Serial.h"
    "source/inc/Media/SerialData.h"

    "source/inc/Widgets/MainWindow.h"
    "source/inc/Widgets/TextBrowser.h"
    "source/inc/Widgets/CommandsTab.h"
    "source/inc/Widgets/ConfigDialog.h"
    "source/inc/Widgets/AppsDialog.h"
    "source/inc/Widgets/PlotWindow.h"
    "source/inc/Widgets/PlotConfigDialog.h"
    "source/inc/Widgets/PlotConfigTabWIdget.h"
    "source/inc/Widgets/Console.h"
    "source/inc/Widgets/LuaConsoleWindow.h"

    "source/inc/Lua/CoLua.h"
    "source/inc/Lua/CoLuaWorker.h"

    "source/inc/Chart/Callout.h"
    "source/inc/Chart/Chart.h"
    "source/inc/Chart/LineSeries.h"
    "source/inc/Chart/ChartView.h"
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
    "source/src/main.cpp"
    "source/src/Common.cpp"

    "source/src/Settings/GlobalSettings.cpp"
    "source/src/Settings/CommandSettings.cpp"

    "source/src/Media/Serial.cpp"
    "source/src/Media/SerialData.cpp"

    "source/src/Widgets/MainWindow.cpp"
    "source/src/Widgets/TextBrowser.cpp"
    "source/src/Widgets/CommandsTab.cpp"
    "source/src/Widgets/ConfigDialog.cpp"
    "source/src/Widgets/AppsDialog.cpp"
    "source/src/Widgets/PlotWindow.cpp"
    "source/src/Widgets/PlotConfigDialog.cpp"
    "source/src/Widgets/PlotConfigTabWIdget.cpp"
    "source/src/Widgets/Console.cpp"
    "source/src/Widgets/LuaConsoleWindow.cpp"

    "source/src/Lua/CoLua.cpp"

    "source/src/Chart/Callout.cpp"
    "source/src/Chart/Chart.cpp"
    "source/src/Chart/LineSeries.cpp"
    "source/src/Chart/ChartView.cpp"
    )


# qt5_add_translation(QM_FILES ${ts_list})
qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${ts_list})

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

set(QHOTKEY_INSTALL OFF)
add_subdirectory("libs/QHotkey/")
set(NO_FFLUA_TEST "y")
add_subdirectory("libs/fflua/")

target_link_libraries(
    ${PROJECT_NAME} PRIVATE
    QHotkey::QHotkey
    fflua
)

target_include_directories(
    ${PROJECT_NAME} PRIVATE
    "source/inc/"
    "source/inc/Chart"
    "source/inc/Lua"
    "source/inc/Widgets"
    "source/inc/Media"
    "source/inc/Settings"
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
    install(FILES ${CMAKE_SOURCE_DIR}/README.md DESTINATION /)

    if (CPACK_GENERATOR STREQUAL 7Z)
        install(FILES ${CMAKE_SOURCE_DIR}/assets/${CoCOM_PORTABLE_FILE_NAME} DESTINATION /)
    endif()

    set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/assets/logos\\\\CoCOM.bmp")
    set(CPACK_PACKAGE_EXECUTABLES ${PROJECT_NAME} ${PROJECT_NAME})
    set(CPACK_CREATE_DESKTOP_LINKS ${PROJECT_NAME})


    if (WIN32) # WIN32
        set(pack_dir CoCOM)
        set(scripts_path ${CMAKE_SOURCE_DIR}/scripts)
        add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND bash -c "echo post build"
            COMMAND bash -c "mkdir -p ${pack_dir}"
            COMMAND bash -c "cp CoCOM.exe ${pack_dir}"
            COMMAND bash -c "cp -r ${scripts_path} ${pack_dir}/scripts"
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
