project(kourglass)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${AKONADI_INCLUDE_DIR}
)

set(kourglass_SRCS
    taskpropertiesdialog.cpp
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

kde4_add_ui_files(kourglass_SRCS
    taskpropertiesdialog.ui
    neweventdialog.ui
    newprojectdialog.ui
    newtaskdialog.ui
    mainview.ui
)

kde4_add_executable(kourglass
    ${kourglass_SRCS}
)

target_link_libraries(kourglass
    ${KDE4_KDEUI_LIBS}
    ${KDEPIMLIBS_AKONADI_LIBS}
    kcalcore
    akonadi-calendar
    akonadi-kde
)

set_target_properties(kourglass PROPERTIES COMPILE_FLAGS "-std=c++11 -g -Wall")

install(TARGETS kourglass DESTINATION ${BIN_INSTALL_DIR})

install(FILES kourglassui.rc DESTINATION ${DATA_INSTALL_DIR}/kourglass)