cmake_minimum_required(VERSION 3.5)

project(uavcan_gui VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Charts)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Positioning)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS QuickWidgets)



set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(uavcan_gui
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        udpwindow.h udpwindow.cpp
        networkinterface.h networkinterface.cpp
        plotterWidget.h plotterWidget.cpp
        uavcandecoder.h uavcandecoder.cpp
        uavcanmonitor.h uavcanmonitor.cpp
        resource.qrc
        gnsswidget.h gnsswidget.cpp


    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET uavcan_gui APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(uavcan_gui SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(uavcan_gui
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(uavcan_gui PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(uavcan_gui PRIVATE Qt${QT_VERSION_MAJOR}::Network)
target_link_libraries(uavcan_gui PRIVATE Qt${QT_VERSION_MAJOR}::Charts)
target_link_libraries(uavcan_gui PRIVATE Qt${QT_VERSION_MAJOR}::Positioning)
target_link_libraries(uavcan_gui PRIVATE Qt${QT_VERSION_MAJOR}::QuickWidgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.uavcan_gui)
endif()
set_target_properties(uavcan_gui PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS uavcan_gui
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(uavcan_gui)
endif()
