cmake_minimum_required(VERSION 3.25)

project(OrchardEngine
    VERSION 1.0.0
    LANGUAGES CXX Swift Objective-C
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_Swift_LANGUAGE_VERSION 5.9)

set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "Build architectures for macOS")
set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "Minimum macOS deployment version")

if(NOT APPLE)
    message(FATAL_ERROR "Orchard Engine requires macOS with Apple Silicon")
endif()

find_library(METAL_LIBRARY Metal REQUIRED)
find_library(METALKIT_LIBRARY MetalKit REQUIRED)
find_library(QUARTZ_CORE_LIBRARY QuartzCore REQUIRED)
find_library(CORE_AUDIO_LIBRARY CoreAudio REQUIRED)
find_library(AVFOUNDATION_LIBRARY AVFoundation REQUIRED)
find_library(COCOA_LIBRARY Cocoa REQUIRED)

set(ENGINE_SOURCES
    Engine/Core/Engine.cpp
    Engine/Core/Memory.cpp
    Engine/Core/Application.hpp
    Engine/Core/Engine.hpp
    Engine/Core/Memory.hpp
    Engine/Core/ResourceManager.hpp
    Engine/Core/SceneManager.hpp
    Engine/Core/EventSystem.hpp
    
    Engine/Math/Vector.hpp
    Engine/Math/Matrix.hpp
    Engine/Math/Quaternion.hpp
    Engine/Math/Transform.hpp
    
    Engine/Utils/UUID.cpp
    Engine/Utils/UUID.hpp
    
    Engine/ECS/Entity.hpp
    Engine/ECS/Component.hpp
    Engine/ECS/Archetype.hpp
    Engine/ECS/World.hpp
    Engine/ECS/System.hpp
    Engine/ECS/Components/TransformComponent.hpp
    
    Engine/Physics/PhysicsWorld.hpp
    Engine/Physics/Rigidbody.hpp
    Engine/Physics/Collider.hpp
    Engine/Physics/CollisionDetection.hpp
    Engine/Physics/Constraint.hpp
    
    Engine/Audio/AudioEngine.hpp
    Engine/Audio/DSP.hpp
    
    Engine/Rendering/Renderer.hpp
    Engine/Rendering/Metal/MetalContext.hpp
    Engine/Rendering/Metal/RenderGraph/RenderGraph.hpp
    
    Engine/Resources/Mesh.hpp
    Engine/Resources/Material.hpp
    Engine/Resources/Texture.hpp
    
    Engine/Scripting/CPPRuntime/SwiftBridge.hpp
    Engine/Scripting/CPPRuntime/SwiftBridge.cpp
)

set(METAL_SHADERS
    Engine/Rendering/Metal/Shaders/Common.metal
    Engine/Rendering/Metal/Shaders/PBR.metal
    Engine/Rendering/Metal/Shaders/Shadow.metal
    Engine/Rendering/Metal/Shaders/PostProcess.metal
)

add_library(OrchardEngineCore STATIC ${ENGINE_SOURCES})

target_include_directories(OrchardEngineCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Engine
)

target_link_libraries(OrchardEngineCore PUBLIC
    ${METAL_LIBRARY}
    ${METALKIT_LIBRARY}
    ${QUARTZ_CORE_LIBRARY}
    ${CORE_AUDIO_LIBRARY}
    ${AVFOUNDATION_LIBRARY}
    ${COCOA_LIBRARY}
)

target_compile_options(OrchardEngineCore PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -march=armv8-a
    -mtune=apple-m1
)

set_target_properties(OrchardEngineCore PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)

foreach(SHADER ${METAL_SHADERS})
    get_filename_component(SHADER_NAME ${SHADER} NAME_WE)
    set(SHADER_OUTPUT "${CMAKE_BINARY_DIR}/Shaders/${SHADER_NAME}.metallib")
    
    add_custom_command(
        OUTPUT ${SHADER_OUTPUT}
        COMMAND xcrun -sdk macosx metal -c ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER} 
                -o ${CMAKE_BINARY_DIR}/Shaders/${SHADER_NAME}.air
        COMMAND xcrun -sdk macosx metallib ${CMAKE_BINARY_DIR}/Shaders/${SHADER_NAME}.air
                -o ${SHADER_OUTPUT}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER}
        COMMENT "Compiling Metal shader ${SHADER_NAME}"
    )
    
    list(APPEND COMPILED_SHADERS ${SHADER_OUTPUT})
endforeach()

add_custom_target(CompileShaders ALL DEPENDS ${COMPILED_SHADERS})

option(BUILD_EDITOR "Build Orchard Editor" ON)
if(BUILD_EDITOR)
    add_subdirectory(Editor)
endif()

option(BUILD_SAMPLES "Build sample projects" ON)
if(BUILD_SAMPLES)
    add_subdirectory(Samples)
endif()

enable_testing()
