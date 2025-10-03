cmake_minimum_required(VERSION 3.19)
project(OpenGL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
add_executable(${PROJECT_NAME} ${SOURCES})

#GLAD 추가
set(GLAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glad)
set(GLAD_SRC 
    ${GLAD_DIR}/src/glad.c
)
add_library(GLAD_LIB STATIC ${GLAD_SRC})
target_include_directories(GLAD_LIB PUBLIC
    ${GLAD_DIR}/include
)
#GLFW 추가
set(GLFW_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glfw)
add_library(GLFW_LIB STATIC IMPORTED)
if(APPLE)
    set_target_properties(GLFW_LIB PROPERTIES
        IMPORTED_LOCATION ${GLFW_DIR}/lib-universal/libglfw3.a
        INTERFACE_INCLUDE_DIRECTORIES ${GLFW_DIR}/include
    )
elseif(WIN32)
    set_target_properties(GLFW_LIB PROPERTIES
        IMPORTED_LOCATION ${GLFW_DIR}/lib-vc2022/glfw3.lib
        INTERFACE_INCLUDE_DIRECTORIES ${GLFW_DIR}/include
    )
endif()
#GLM 추가
set(GLW_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glm)
add_library(GLM_LIB INTERFACE)
target_include_directories(GLM_LIB INTERFACE
    ${GLW_DIR}
)
#STB 추가
set(STB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/stb)
add_library(STB_LIB INTERFACE)
target_include_directories(STB_LIB INTERFACE
    ${STB_DIR}
)
#ASSIMP 추가
set(ASSIMP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp)
add_library(ASSIMP_LIB STATIC IMPORTED)
if(APPLE)
    set_target_properties(ASSIMP_LIB PROPERTIES
        IMPORTED_LOCATION ${ASSIMP_DIR}/lib/libassimp.6.dylib
        INTERFACE_INCLUDE_DIRECTORIES ${ASSIMP_DIR}/include
    )
elseif(WIN32)
    set_target_properties(ASSIMP_LIB PROPERTIES
        IMPORTED_LOCATION ${ASSIMP_DIR}/lib/assimp-vc143-mt.lib
        INTERFACE_INCLUDE_DIRECTORIES ${ASSIMP_DIR}/include
    )
endif()
if(APPLE)
    target_link_libraries(${PROJECT_NAME} PUBLIC
    GLAD_LIB
    GLM_LIB
    GLFW_LIB
    STB_LIB
    ASSIMP_LIB
    "-framework Cocoa"
    "-framework IOKit"
    "-framework CoreVideo"
    "-framework OpenGL"
    )
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY   "${CMAKE_SOURCE_DIR}/bin"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/bin"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin"
        OUTPUT_NAME   "${PROJECT_NAME}_debug"
        OUTPUT_NAME_DEBUG   "${PROJECT_NAME}_debug"
        OUTPUT_NAME_RELEASE "${PROJECT_NAME}_release"
        BUILD_RPATH "@executable_path"
        INSTALL_RPATH "@executable_path"
        RPATH_USE_LINK_PATH TRUE
    )
elseif(WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC
    GLAD_LIB
    GLM_LIB
    GLFW_LIB
    STB_LIB
    ASSIMP_LIB
    opengl32
    )
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY   "${CMAKE_SOURCE_DIR}/bin"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/bin"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin"
        OUTPUT_NAME   "${PROJECT_NAME}_debug"
        OUTPUT_NAME_DEBUG   "${PROJECT_NAME}_debug"
        OUTPUT_NAME_RELEASE "${PROJECT_NAME}_release"
    )
endif()
if (MSVC)
    add_compile_options(/utf-8)
endif()
