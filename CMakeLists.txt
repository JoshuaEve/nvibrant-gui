cmake_minimum_required(VERSION 3.21)
project(nvibrant-gui VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
include(GNUInstallDirs)

find_package(Qt6 6.4 REQUIRED COMPONENTS Widgets)

add_executable(nvibrant-gui
    src/main.cpp
    src/backend/ProcessRunner.cpp src/backend/ProcessRunner.h
    src/backend/NvibrantController.cpp src/backend/NvibrantController.h
    src/backend/ConfigManager.cpp src/backend/ConfigManager.h
    src/backend/PresetManager.cpp src/backend/PresetManager.h
    src/ui/MainWindow.cpp src/ui/MainWindow.h
    src/ui/SettingsDialog.cpp src/ui/SettingsDialog.h
    src/ui/AboutDialog.cpp src/ui/AboutDialog.h
    src/ui/TrayIcon.cpp src/ui/TrayIcon.h
    src/widgets/VibranceSlider.cpp src/widgets/VibranceSlider.h
    src/widgets/PresetList.cpp src/widgets/PresetList.h
    src/widgets/DeviceSelector.cpp src/widgets/DeviceSelector.h
    src/widgets/MonitorLayout.cpp src/widgets/MonitorLayout.h
    resources/resources.qrc)
target_link_libraries(nvibrant-gui PRIVATE Qt6::Widgets)
target_include_directories(nvibrant-gui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_definitions(nvibrant-gui PRIVATE
    NVIBRANT_GUI_VERSION="${PROJECT_VERSION}"
    NVIBRANT_GUI_GITHUB="https://github.com/Tremeschin/nvibrant")
target_compile_options(nvibrant-gui PRIVATE -Wall -Wextra -Wpedantic)

install(TARGETS nvibrant-gui RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES packaging/nvibrant-gui.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(FILES resources/icons/nvibrant-gui.svg DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)
install(FILES LICENSE DESTINATION ${CMAKE_INSTALL_DATADIR}/licenses/nvibrant-gui)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake
    COMMENT "Removing files installed by this build")
