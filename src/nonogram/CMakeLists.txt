set (INSTALL_DESTINATION "bin")
set (SOURCES data/Answer.cpp
             data/Nonogram.cpp
             data/Solution.cpp
             gui/MainWindow.cpp
             gui/PlayField.cpp
             gui/command/Base.cpp
             gui/command/Cross.cpp
             gui/command/Fill.cpp
             gui/command/Lock.cpp
             util/print_exception.cpp
             main.cpp
)
set (LIBRARIES Qt5::OpenGL Qt5::Widgets)
set (TARGET_NAME Nonogram)
add_executable (${TARGET_NAME} ${SOURCES})
set (${TARGET_NAME} PROPERTY AUTOMOC ON)
target_compile_features (${TARGET_NAME} PRIVATE cxx_std_17)
target_link_libraries (${TARGET_NAME} PRIVATE ${LIBRARIES})
install (TARGETS ${TARGET_NAME} RUNTIME DESTINATION "${INSTALL_DESTINATION}")