cmake_minimum_required(VERSION 3.19)
project(StudentManagerSystem LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Sql)

qt_standard_project_setup()
set(CMAKE_AUTORCC ON)   # 添加这一行

qt_add_executable(StudentManagerSystem
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    res.qrc
    databasemanager.h databasemanager.cpp
    studentinfowidget.h studentinfowidget.cpp studentinfowidget.ui
    tabledelegates.h

)

target_link_libraries(StudentManagerSystem
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt6::Sql
)
install(TARGETS StudentManagerSystem
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET StudentManagerSystem
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}
)