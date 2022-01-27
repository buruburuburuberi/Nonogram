set (INSTALL_DESTINATION "bin")
set (SOURCES gui/MainWindow.cpp
             main.cpp
)
set (LIBRARIES Qt5::Widgets)
set (TARGET_NAME Nonogram)
add_executable (${TARGET_NAME} ${SOURCES})
set (${TARGET_NAME} PROPERTY AUTOMOC ON)
target_link_libraries (${TARGET_NAME} PRIVATE ${LIBRARIES})
install (TARGETS ${TARGET_NAME} RUNTIME DESTINATION "${INSTALL_DESTINATION}")