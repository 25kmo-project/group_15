cmake_minimum_required(VERSION 3.16)

project(Pankkiautomaatti VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Network)


set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    environment.cpp
    environment.h

    debitvscredit.cpp
    debitvscredit.h
    debitvscredit.ui

    withdraw.cpp
    withdraw.h
    withdraw.ui

    appicon.rc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Pankkiautomaatti
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        menu.h menu.cpp menu.ui
        deposit.h deposit.cpp deposit.ui
        transactionhistory.h transactionhistory.cpp transactionhistory.ui
        clientinfo.h clientinfo.cpp clientinfo.ui
        transfer.h transfer.cpp transfer.ui
        withdraw.h withdraw.cpp withdraw.ui
        currency.h currency.cpp currency.ui
        balance.h balance.cpp balance.ui
        receipt.h receipt.cpp receipt.ui


    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Pankkiautomaatti APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Pankkiautomaatti SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Pankkiautomaatti
            ${PROJECT_SOURCES}
            environment.h environment.cpp
            style.qss
            resource.qrc
            debitvscredit.h debitvscredit.cpp debitvscredit.ui
        )
    endif()
endif()

qt_add_big_resources(RESOURCES_ADDED resource.qrc)
target_sources(Pankkiautomaatti PRIVATE ${RESOURCES_ADDED})

target_link_libraries(Pankkiautomaatti PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network)

target_compile_definitions(Pankkiautomaatti PRIVATE
  $<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT>
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Pankkiautomaatti)
endif()
set_target_properties(Pankkiautomaatti PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# icon for macOS app
if(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE bankicon.icns)
    set(APP_ICON_MACOSX ${CMAKE_CURRENT_SOURCE_DIR}/bankicon.icns)
    set_source_files_properties(${APP_ICON_MACOSX} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    target_sources(Pankkiautomaatti PRIVATE ${APP_ICON_MACOSX})
endif()

include(GNUInstallDirs)
install(TARGETS Pankkiautomaatti
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Pankkiautomaatti)
endif()
