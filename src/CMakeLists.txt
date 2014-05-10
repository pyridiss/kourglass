project(timetrak)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
)

set(timetrak_SRCS
    eventslistdialog.cpp
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

kde4_add_ui_files(timetrak_SRCS
                  eventslistdialog.ui
                  neweventdialog.ui
                  newprojectdialog.ui
                  newtaskdialog.ui
                  mainview.ui)

kde4_add_executable(timetrak ${timetrak_SRCS})

target_link_libraries(timetrak ${KDE4_KDEUI_LIBS})

set_target_properties(timetrak PROPERTIES COMPILE_FLAGS "-std=c++11 -g -Wall")

install(TARGETS timetrak DESTINATION ${BIN_INSTALL_DIR})

install(FILES timetrakui.rc DESTINATION ${DATA_INSTALL_DIR}/timetrak)