cmake_minimum_required(VERSION 3.19)
project(ocv01)

if(MSVC)
    add_compile_options("/W4" "$<$<CONFIG:RELEASE>:/O2>")
else()
	# for the hardcore type of guy...
	#add_compile_options("-Wall" "-Wextra" "-Werror" "$<$<CONFIG:RELEASE>:-O3>")
	#add_compile_options("$<$<CONFIG:RELEASE>:-O3>")
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	    # add_compile_options("-stdlib=libc++")
	    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -v")
	    # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
    else()
        # nothing special for gcc at the moment
    endif()
endif()
set(SOURCES main.cpp)
if (WIN32)
	SET (Boost_USE_STATIC_LIBS ON)
endif(WIN32)


find_package(Threads)
find_package(PkgConfig REQUIRED)
find_package(Boost COMPONENTS filesystem iostreams system program_options timer REQUIRED)
find_package( OpenCV REQUIRED )
pkg_check_modules(GStreamer REQUIRED IMPORTED_TARGET gstreamer-1.0)
pkg_check_modules(GLib REQUIRED IMPORTED_TARGET glib-2.0)

enable_testing()

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-D_DEBUG=1)
endif()
add_executable(${PROJECT_NAME} ${SOURCES})

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
# CMake 3.14 onwards
target_link_directories(${PROJECT_NAME}
        PRIVATE
        ../lib
)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        ${CONAN_LIBS}
        Threads::Threads
        Boost::program_options
        Boost::filesystem
        Boost::timer
        opencv_core
        opencv_videoio
        opencv_highgui
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
