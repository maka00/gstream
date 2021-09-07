project(guiapp)

include_directories(${Qt5Widgets_INCLUDE_DIRS} ${QtQml_INCLUDE_DIRS})
add_definitions(${Qt5Widgets_DEFINITIONS} ${QtQml_DEFINITIONS} ${${Qt5Quick_DEFINITIONS}})
find_package(Qt5 COMPONENTS Widgets Multimedia Qml Quick REQUIRED)
qt5_add_resources(QT_RESOURCES qml.qrc)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-D_DEBUG=1)
endif()
set(SOURCES main.cpp qcvedgefilter.cpp qcvedgefilter.h)
add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} ${QT_RESOURCES})

set_target_properties(${PROJECT_NAME}
        PROPERTIES
        DEBUG_POSTFIX "d"
        CXX_STANDARD 20
        )


target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${PROJECT_SOURCE_DIR}

        PRIVATE
        ${BOOST_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR}

        )
target_compile_features(${PROJECT_NAME}
        PUBLIC
        cxx_std_20
        )

target_compile_options(${PROJECT_NAME}
        PUBLIC
        PRIVATE
        -Wall -Wextra -pedantic
        )

target_link_libraries(${PROJECT_NAME}
        ${CONAN_LIBS}
        Qt5::Widgets
        Qt5::Multimedia
        Qt5::Qml
        Qt5::Quick
        )