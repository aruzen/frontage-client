cmake_minimum_required(VERSION 3.20)
project(frontage_client VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(frontage_client
        src/main.cpp
        src/utils/gltf_loader.cpp
        src/utils/bitmap_text.cpp
        src/obj/card.cpp
        src/obj/piece.cpp
        src/resource/image.cpp
        src/scene/lobby/lobby.cpp
        src/drawer/texture.cpp
        src/resource/mvp.cpp
        src/obj/square.cpp
        src/global.cpp
        src/scene/scene.hpp
        src/resource/standard_buffers.cpp
        src/utils/operators.cpp
        src/scene/game/game.hpp
        src/scene/game/game.cpp
)

link_directories(lib)

target_include_directories(frontage_client PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(frontage_client PRIVATE ${Boost_LIBRARIES})

target_include_directories(frontage_client PRIVATE ./thirdparty/)

target_include_directories(frontage_client PRIVATE ${GLFW_INCLUDE_DIRS})
target_link_directories(frontage_client PRIVATE ${GLFW_LIBRARY_DIRS})
target_link_libraries(frontage_client PRIVATE ${GLFW_LIBRARIES})

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)
target_link_libraries(frontage_client PRIVATE ${GLFW_LIBRARIES})

find_package(glm CONFIG REQUIRED)
target_link_libraries(frontage_client PRIVATE glm::glm)

find_package(VSL REQUIRED)
target_link_libraries(frontage_client PRIVATE VSL::VSL)

find_package(AHO REQUIRED)
target_link_libraries(frontage_client PRIVATE AHO::AHOLi)
