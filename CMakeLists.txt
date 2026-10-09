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

# The git hash is shown on screen. Version.h is only rewritten when the hash changes, so it won't rebuild everything
set(TOY2_GIT_HASH "unknown")
find_package(Git QUIET)

if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE TOY2_GIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    execute_process(
        COMMAND ${GIT_EXECUTABLE} status --porcelain --untracked-files=no
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE TOY2_GIT_STATUS
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    if(TOY2_GIT_STATUS)
        set(TOY2_GIT_HASH "${TOY2_GIT_HASH}-dirty")
    endif()
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/Version.h.in ${CMAKE_BINARY_DIR}/generated/Version.h @ONLY)
include_directories(${CMAKE_BINARY_DIR}/generated)

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
    src/Toy2/Camera.cpp
    src/Toy2/Camera.h
    src/Toy2/CreatureBehaviour.cpp
    src/Toy2/CreatureBehaviour.h
    src/Toy2/Cutscene.cpp
    src/Toy2/Cutscene.h
    src/Toy2/Dialogue.cpp
    src/Toy2/Dialogue.h
    src/Toy2/Gadget.cpp
    src/Toy2/Gadget.h
    src/Toy2/Level.cpp
    src/Toy2/Level.h
    src/Toy2/Lighting.cpp
    src/Toy2/Lighting.h
    src/Toy2/MoveableObject.cpp
    src/Toy2/MoveableObject.h
    src/Toy2/Particles.cpp
    src/Toy2/Particles.h
    src/Toy2/Platform.cpp
    src/Toy2/Platform.h
    src/Toy2/Portal.cpp
    src/Toy2/Portal.h
    src/Toy2/Shadow.cpp
    src/Toy2/Shadow.h

    # Toy2/Levels
    src/Toy2/Levels/AirportInfiltration.cpp
    src/Toy2/Levels/AirportInfiltration.h
    src/Toy2/Levels/AlleysAndGullies.cpp
    src/Toy2/Levels/AlleysAndGullies.h
    src/Toy2/Levels/AlsPenthouse.cpp
    src/Toy2/Levels/AlsPenthouse.h
    src/Toy2/Levels/AlsSpaceLand.cpp
    src/Toy2/Levels/AlsSpaceLand.h
    src/Toy2/Levels/AlsToyBarn.cpp
    src/Toy2/Levels/AlsToyBarn.h
    src/Toy2/Levels/AndysHouse.cpp
    src/Toy2/Levels/AndysHouse.h
    src/Toy2/Levels/AndysNeighborhood.cpp
    src/Toy2/Levels/AndysNeighborhood.h
    src/Toy2/Levels/BarnEncounter.cpp
    src/Toy2/Levels/BarnEncounter.h
    src/Toy2/Levels/BombsAway.cpp
    src/Toy2/Levels/BombsAway.h
    src/Toy2/Levels/ConstructionYard.cpp
    src/Toy2/Levels/ConstructionYard.h
    src/Toy2/Levels/ElevatorHop.cpp
    src/Toy2/Levels/ElevatorHop.h
    src/Toy2/Levels/EvilEmperorZurg.cpp
    src/Toy2/Levels/EvilEmperorZurg.h
    src/Toy2/Levels/FinalShowdown.cpp
    src/Toy2/Levels/FinalShowdown.h
    src/Toy2/Levels/SlimeTime.cpp
    src/Toy2/Levels/SlimeTime.h
    src/Toy2/Levels/TarmacTrouble.cpp
    src/Toy2/Levels/TarmacTrouble.h

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
    src/Nu3D/Collision.cpp
    src/Nu3D/Collision.h
    src/Nu3D/Creature.cpp
    src/Nu3D/FMV.cpp
    src/Nu3D/FMV.h

    # Renderer
    src/Renderer/Renderer.h
    src/Renderer/Renderer.cpp
    src/Renderer/ApplyFixes.h
    src/Renderer/ApplyFixes.cpp
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
    src/Renderer/LensFlare.cpp
    src/Renderer/LensFlare.h

    # NGNLoader
    src/NGNLoader/NGNLoader.cpp
    src/NGNLoader/NGNLoader.h
    src/NGNLoader/NGNTypes.h
    src/NGNLoader/ObjectLoad.cpp
    src/NGNLoader/ObjectLoad.h
    src/NGNLoader/NGNImage.cpp

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
target_link_libraries(patcher PRIVATE minhook_x86 ddraw dxguid winmm dinput dsound amstrmid)

target_link_directories(toy2decomp PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/external/libs/directx6
)
target_link_libraries(toy2decomp PRIVATE ddraw dxguid winmm dinput dsound amstrmid)
set_target_properties(toy2decomp PROPERTIES OUTPUT_NAME "toy2")
