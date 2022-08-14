## Configure
## -- step for preparing build for Visual Studio
# cmake -Bbuild . -DCMAKE_BUILD_TYPE=Debug

## Build
## -- step for building code
# cmake --build build --config Debug

cmake_minimum_required(VERSION 3.14)

set(PROJECT_NAME HiEngine)
set(CMAKE_CXX_STANDARD 17)
if(NOT DEFINED CMAKE_CUDA_ARCHITECTURES)
  set(CMAKE_CUDA_ARCHITECTURES 61) # GTX 1060
endif()


# macro will be used in cpp codes
set(WINDOW_NAME "HiEngine")
set(WINDOW_WIDTH 960)
set(WINDOW_HEIGHT 540)

project(${PROJECT_NAME} LANGUAGES CXX CUDA)
# include_directories(src/scene)
add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/common.cpp src/common.h
    src/shader.cpp src/shader.h
    src/program.cpp src/program.h
    src/context.cpp src/context.h
    src/buffer.cpp src/buffer.h
    src/vertex_layout.cpp src/vertex_layout.h
    src/image.cpp src/image.h
    src/texture.cpp src/texture.h
    src/mesh.cpp src/mesh.h
    src/model.cpp src/model.h
    src/simbuffer.cpp src/simbuffer.h
    src/framebuffer.cpp src/framebuffer.h
    src/scenes/scene.h
    src/HiPhysics/hiphysics.cu src/HiPhysics/hiphysics.h
    src/HiPhysics/hiphysicsPBD.cu src/HiPhysics/hiphysicsPBD.h
    )

include(Dependency.cmake)

# set(SCENE "Scene")
# add_library(${SCENE} PUBLIC src/scenes/scene.h)
# target_include_directories(${SCENE} PUBLIC src/scenes/scene.h)


# target_link_libraries(${PROJECT_NAME} ${SCENE})
# target_include_directories(${SCENE} INTERFACE src/scenes/)
# target_link_libraries(${SCENE} INTERFACE src/scenes/)

# target_include_directories(${PROJECT_NAME} INTERFACE src/scenes/)
# target_link_libraries(${PROJECT_NAME} INTERFACE src/scenes/)
# 프로젝트에 include / lib 관련 옵션 추가
# target_include_directories
# 타겟의 이름: add_executable(${PROJECT_NAME} src/main.cpp)
#             spdlog
#             등이 될 수 있다.

set_target_properties(${PROJECT_NAME} PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
target_include_directories(${PROJECT_NAME} PUBLIC ${DEP_INCLUDE_DIR})
target_link_directories(${PROJECT_NAME} PUBLIC ${DEP_LIB_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${DEP_LIBS} ${SCENE}) # 실제로 사용할 라이브러리 .lib은 지워라
target_compile_definitions(${PROJECT_NAME} PUBLIC
    WINDOW_NAME="${WINDOW_NAME}"
    WINDOW_WIDTH=${WINDOW_WIDTH}
    WINDOW_HEIGHT=${WINDOW_HEIGHT}
    )

add_dependencies(${PROJECT_NAME} ${DEP_LIST}) # DEP LIST 컴파일 후에 다하고 나서 PROJECT_NAME을 컴파일해라