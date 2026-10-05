cmake_minimum_required(VERSION 3.20.0)

set(PROJECT_NAME "toy2decomp")

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# MSVC runtime library flags are selected by an abstraction
cmake_policy(SET CMP0091 NEW)

project(${PROJECT_NAME} C CXX RC)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/patcher)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/include)

set(CMAKE_MSVC_RUNTIME_LIBRARY "")

# C++ flags
set(CMAKE_CXX_FLAGS "/W3 /GX /D \"WIN32\" /D \"_WINDOWS\"")
set(CMAKE_CXX_FLAGS_DEBUG "/Gm /MLd /Zi /Od /D \"_DEBUG\"")
set(CMAKE_CXX_FLAGS_RELEASE "/O2 /ML /D \"NDEBUG\"")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/Zi /ML /O2 /D \"NDEBUG\"")
set(CMAKE_CXX_FLAGS_MINSIZEREL "/Os /ML /D \"NDEBUG\"")

# EXE linker flags
set(CMAKE_EXE_LINKER_FLAGS "/machine:I386 /NODEFAULTLIB:libcp.lib")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/incremental:yes /debug")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/incremental:no")
set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "/incremental:no /debug")
set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "/incremental:no")

# DLL linker flags
set(CMAKE_SHARED_LINKER_FLAGS "/machine:I386")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "/incremental:yes /debug")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "/incremental:no")
set(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO "/incremental:no /debug")
set(CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL "/incremental:no")


# Toy2 Sources
set(TOY2_SOURCES
    src/SinCosLUT.inc
    src/LibraryMSVC.h

    src/Logger.cpp
    src/Logger.h

    src/FileUtils.h
    src/FileUtils.cpp

    src/Nullsub.cpp
    src/Nullsub.h

    src/Common.h
    src/Numerics.h
    src/Numerics.cpp

    src/DrawingDevice.h
    src/DrawingDevice.cpp

    src/ModeSelect.cpp
    src/ModeSelect.h

    src/Random.h
    src/Random.cpp

    src/SoftwareRenderer.cpp
    src/SoftwareRenderer.h

    src/SaveManager.cpp
    src/SaveManager.h

    src/InputTypes.h
    src/InputManager.h
    src/InputManager.cpp

    src/CharacterLoader.h
    src/CharacterLoader.cpp

    src/RawLoader.cpp
    src/RawLoader.h

    # Toy2
    src/Toy2/LevelSelect.cpp
    src/Toy2/LevelSelect.h
    src/Toy2/Toy2.cpp
    src/Toy2/Toy2.h
    src/Toy2/D3DApp.cpp
    src/Toy2/D3DApp.h
    src/Toy2/D3DAppTypes.h
    src/Toy2/Buzz.cpp
    src/Toy2/Buzz.h
    src/Toy2/Actor.cpp
    src/Toy2/Actor.h
    src/Toy2/Levels.cpp
    src/Toy2/Levels.h
    src/Toy2/Collision.h
    src/Toy2/Collision.cpp
    src/Toy2/Collectables.cpp
    src/Toy2/Collectables.h
    src/Toy2/MainMenu.cpp
    src/Toy2/MainMenu.h
    src/Toy2/Animation.h
    src/Toy2/Animation.cpp
    src/Toy2/Weather.cpp
    src/Toy2/Weather.h
    src/Toy2/Sector.cpp
    src/Toy2/Sector.h
    src/Toy2/KiteTail.cpp
    src/Toy2/KiteTail.h

    # Nu3D
    src/Nu3D/Creature.h
    src/Nu3D/Link.h
    src/Nu3D/Link.cpp
    src/Nu3D/Material.h
    src/Nu3D/Material.cpp
    src/Nu3D/Nu3D.h
    src/Nu3D/Nu3D.cpp
    src/Nu3D/BmpDataNode.h
    src/Nu3D/BmpDataNode.cpp
    src/Nu3D/Patch.h
    src/Nu3D/Patch.cpp
    src/Nu3D/Portal.h
    src/Nu3D/Portal.cpp
    src/Nu3D/Primitive.h
    src/Nu3D/Primitive.cpp
    src/Nu3D/Sprite.h
    src/Nu3D/Viewport.h
    src/Nu3D/Viewport.cpp
    src/Nu3D/Font.h
    src/Nu3D/Font.cpp
    src/Nu3D/Light.cpp
    src/Nu3D/Light.h
    src/Nu3D/Camera.cpp
    src/Nu3D/Camera.h
    src/Nu3D/Particles.h
    src/Nu3D/Particles.cpp
    src/Nu3D/Math.cpp
    src/Nu3D/Math.h
    src/Nu3D/Scene.cpp
    src/Nu3D/Scene.h

    # Renderer
    src/Renderer/Renderer.h
    src/Renderer/Renderer.cpp
    src/Renderer/RenderType.h
    src/Renderer/Sprite.h
    src/Renderer/Sprite.cpp
    src/Renderer/RenderQueue.cpp
    src/Renderer/RenderQueue.h
    src/Renderer/RenderDraw.cpp
    src/Renderer/Effects.cpp
    src/Renderer/Effects.h
    src/Renderer/Shadows.cpp
    src/Renderer/Shadows.h
    src/Renderer/SpriteSheets.cpp
    src/Renderer/SpriteSheets.h
    src/Renderer/Glue.cpp
    src/Renderer/Glue.h

    # NGNLoader
    src/NGNLoader/NGNLoader.cpp
    src/NGNLoader/NGNLoader.h
    src/NGNLoader/NGNTypes.h
    src/NGNLoader/ObjectLoad.cpp
    src/NGNLoader/ObjectLoad.h

    # AudioManager
    src/AudioManager/AudioManager.cpp
    src/AudioManager/AudioManager.h
)

# Build the game
add_executable(toy2decomp WIN32 
    ${TOY2_SOURCES}

    resources/toy2.rc
)

# Build the testing dll(s)
add_library(patcher SHARED
    patcher/main.cpp

    patcher/Loader.cpp
    patcher/Loader.h

    patcher/GamePatches.cpp
    patcher/GamePatches.h

    patcher/Utilities.h

    ${TOY2_SOURCES}
)

target_link_directories(patcher PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/external/libs
    ${CMAKE_CURRENT_SOURCE_DIR}/external/libs/directx6
)
target_link_libraries(patcher PRIVATE minhook_x86 ddraw dxguid winmm dinput)

target_link_directories(toy2decomp PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/external/libs/directx6
)
target_link_libraries(toy2decomp PRIVATE ddraw dxguid winmm dinput)
set_target_properties(toy2decomp PROPERTIES OUTPUT_NAME "toy2")
