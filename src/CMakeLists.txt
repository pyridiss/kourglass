project(kourglass)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${AKONADI_INCLUDE_DIR}
)

set(kourglass_SRCS
    neweventdialog.cpp
    newprojectdialog.cpp
    newtaskdialog.cpp
    mainview.cpp
    event.cpp
    task.cpp
    storage.cpp
    mainwindow.cpp
    main.cpp
)

kconfig_add_kcfg_files(kourglass_SRCS
    kourglassSettings.kcfgc
)

ki18n_wrap_ui(kourglass_SRCS
    neweventdialog.ui
    newprojectdialog.ui
    newtaskdialog.ui
    mainview.ui
)

add_executable(kourglass
    ${kourglass_SRCS}
)

target_link_libraries(kourglass
    Qt5::Widgets
    KF5::CoreAddons
    KF5::I18n
    KF5::XmlGui
    KF5::ConfigWidgets
    KF5::AkonadiCore
    KF5::AkonadiWidgets
    KF5::CalendarCore
)

set_target_properties(kourglass PROPERTIES COMPILE_FLAGS "-std=c++11 -g -Wall")

install(TARGETS kourglass DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS kourglass.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
install(FILES kourglassui.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/kourglass)
