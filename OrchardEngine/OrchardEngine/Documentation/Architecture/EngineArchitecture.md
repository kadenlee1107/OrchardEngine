# Orchard Engine Architecture

## Overview

Orchard Engine is a modern, macOS-native game engine designed specifically for Apple Silicon (M1, M2, M3, M4) processors. It leverages Metal 3 for rendering, CoreAudio for audio, and provides a dual-scripting system using both C++17 and Swift.

## Core Design Principles

1. **Native Performance**: Built exclusively for Apple Silicon with ARM NEON optimizations
2. **Modern Architecture**: Entity Component System (ECS) with data-oriented design
3. **Zero Dependencies**: Custom implementations of physics, rendering, and audio systems
4. **Developer Friendly**: SwiftUI-based editor with hot-reload support

## High-Level Architecture

```
┌─────────────────────────────────────────────────┐
│           Orchard Editor (SwiftUI)              │
│  ┌──────────┬──────────┬────────────────────┐  │
│  │ Scene    │Inspector │ Asset Browser      │  │
│  │ Hierarchy│  Panel   │ Material Editor    │  │
│  └──────────┴──────────┴────────────────────┘  │
└─────────────────────────────────────────────────┘
                      ↓
┌─────────────────────────────────────────────────┐
│         Swift-C++ Interop Layer                 │
│  ┌──────────────────────────────────────────┐  │
│  │  Swift Bindings ↔ C++ Bridge             │  │
│  └──────────────────────────────────────────┘  │
└─────────────────────────────────────────────────┘
                      ↓
┌─────────────────────────────────────────────────┐
│              Orchard Engine Core                │
│  ┌──────────┬──────────┬──────────┬─────────┐  │
│  │   ECS    │ Renderer │  Physics │  Audio  │  │
│  │  System  │ (Metal)  │  Engine  │ Engine  │  │
│  └──────────┴──────────┴──────────┴─────────┘  │
│  ┌──────────────────────────────────────────┐  │
│  │    Resource Manager & Asset Pipeline     │  │
│  └──────────────────────────────────────────┘  │
└─────────────────────────────────────────────────┘
                      ↓
┌─────────────────────────────────────────────────┐
│            Platform Layer (macOS)               │
│  ┌──────────┬──────────┬──────────┬─────────┐  │
│  │  Metal 3 │ MetalKit │CoreAudio │ AppKit  │  │
│  └──────────┴──────────┴──────────┴─────────┘  │
└─────────────────────────────────────────────────┘
```

## Core Systems

### 1. Entity Component System (ECS)

The ECS system uses an archetype-based architecture for optimal cache performance:

- **Entities**: Lightweight identifiers with versioning
- **Components**: Plain data structures with no logic
- **Systems**: Logic processors that operate on component data
- **Archetypes**: Memory chunks organizing entities by component signature

**Key Features**:
- SIMD-accelerated iteration using ARM NEON
- Hot-reload support for components
- Efficient memory layout for cache coherency
- Parallel system execution

### 2. Rendering System

Metal 3-based rendering pipeline with modern features:

**Render Graph**:
- Node-based rendering pipeline
- Automatic resource management
- Multi-pass rendering support
- GPU-driven culling and indirect draws

**Features**:
- PBR (Physically Based Rendering) with GGX/Smith
- Cascaded shadow maps
- HDR rendering with tone mapping
- Post-processing (FXAA, bloom, etc.)
- MetalFX upscaling integration
- Ray tracing support (optional)

### 3. Physics Engine

Custom physics implementation with:

**Collision Detection**:
- Broadphase: Sweep and Prune (SAP)
- Narrowphase: GJK + EPA algorithms
- Continuous collision detection

**Dynamics**:
- Semi-implicit Euler integration
- Sequential impulse solver
- Constraint system (joints, motors)
- Soft body support

**Features**:
- Character controller
- Trigger volumes
- Raycasting and shape casting
- Decoupled physics timestep

### 4. Audio System

CoreAudio-based audio engine:

- Ultra-low latency mixing
- Spatial audio with HRTF
- Real-time DSP effects (reverb, EQ, compression)
- Streaming audio for music
- Audio Units integration
- Sound banks with compression

### 5. Resource Management

UUID-based asset system:

- Automatic dependency tracking
- Incremental loading
- Memory pooling
- Asset streaming
- Custom .orchardpkg format
- Import pipeline for FBX, OBJ, USD, textures, audio

## Memory Architecture

### Memory Arenas
- Frame allocator for temporary allocations
- Pool allocators for fixed-size objects
- Aligned allocations for SIMD operations

### Resource Streaming
- Async loading with priority queue
- Mipmap streaming for textures
- LOD system for meshes

## Threading Model

```
Main Thread:
  - Game logic update
  - ECS system execution
  - Input handling
  
Render Thread:
  - Command buffer encoding
  - Metal submission
  - GPU synchronization
  
Worker Threads:
  - Asset loading
  - Physics broad phase
  - Audio mixing
  - Animation blending
```

## Platform Integration

### Metal 3
- Bindless resources
- Mesh shaders (object/mesh shader pipeline)
- Ray tracing acceleration structures
- MetalFX temporal upscaling

### ProMotion
- Adaptive refresh rate (24-120 Hz)
- Variable rate shading

### XDR Displays
- HDR/EDR output support
- P3 wide color gamut

## Swift-C++ Interoperability

The engine uses a careful bridging layer:

1. **C++ Engine Core**: Performance-critical systems
2. **C Bridge Functions**: Opaque pointers and C-compatible types
3. **Swift Wrappers**: Safe, idiomatic Swift API
4. **SwiftUI Editor**: Modern UI with reactive updates

## Build System

- **CMake**: Cross-configuration build generator
- **Xcode**: Native IDE integration
- **Metal Compiler**: Shader compilation pipeline
- **Swift Package Manager**: Dependency management (future)

## Supported Platforms

- **Primary**: macOS 13+ (Ventura) on Apple Silicon
- **Future**: iOS, iPadOS, tvOS, visionOS

## Performance Targets

- 60 FPS minimum on M1
- 120 FPS on M1 Pro and above
- ProMotion support (adaptive refresh)
- < 16ms frame time for complex scenes

## File Formats

### Scene Format (.orchardscene)
Binary format with:
- Entity hierarchy
- Component data
- Asset references (UUID-based)

### Package Format (.orchardpkg)
Compressed archive containing:
- Compiled shaders
- Processed assets
- Metadata and manifests

### Project Format (.orchardproject)
JSON-based project configuration

## Extension Points

- Custom components
- Custom systems
- Shader graphs
- Material graphs
- Build pipeline plugins
- Asset importers
