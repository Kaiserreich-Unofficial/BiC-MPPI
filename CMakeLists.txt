cmake_minimum_required(VERSION 3.5)
project(mppi)

include(FetchContent)

FetchContent_Declare(
    eigenrand
    GIT_REPOSITORY https://github.com/bab2min/EigenRand.git
    GIT_TAG        main  # 或者指定某个版本如 v0.3.5
)

FetchContent_MakeAvailable(eigenrand)

FetchContent_Declare(
    matplotlibcpp
    GIT_REPOSITORY https://github.com/lava/matplotlib-cpp.git
    GIT_TAG        master
)

FetchContent_MakeAvailable(matplotlibcpp)

find_package(PythonLibs REQUIRED)
find_package (Eigen3 REQUIRED NO_MODULE)
find_package(OpenMP REQUIRED)

execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c "import numpy; print(numpy.get_include())"
    OUTPUT_VARIABLE NUMPY_INCLUDE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

include_directories(
    mppi/
    model/

    # EigenRand/
    ${eigenrand_SOURCE_DIR}
    ${EIGEN3_INCLUDE_DIRS}
    ${NUMPY_INCLUDE_DIR}
    ${matplotlibcpp_SOURCE_DIR}
    ${PYTHON_INCLUDE_DIRS}
)

if (quadrotor)
    set(COMMON_SOURCES
        src/quadrotor/bi_mppi.cpp
        src/quadrotor/mppi.cpp
        src/quadrotor/log_mppi.cpp
        src/quadrotor/cluster_mppi.cpp
    )
else()
    set(COMMON_SOURCES
        src/wmrobot/bi_mppi.cpp
        src/wmrobot/mppi.cpp
        src/wmrobot/log_mppi.cpp
        src/wmrobot/cluster_mppi.cpp
    )
endif()


foreach(SRC ${COMMON_SOURCES})
    get_filename_component(EXE_NAME ${SRC} NAME_WE)
    add_executable(${EXE_NAME} ${SRC})
    target_compile_options(${EXE_NAME} PRIVATE -O3 -flto)
    target_link_libraries(${EXE_NAME}
        ${PYTHON_LIBRARIES}
        OpenMP::OpenMP_CXX
    )
endforeach()
