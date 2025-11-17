# Getting Started with Orchard Engine

## Prerequisites

- **Mac with Apple Silicon** (M1, M2, M3, or M4)
- **macOS 13 Ventura** or later
- **Xcode 15+** with Command Line Tools
- **CMake 3.25+**

## Installation

### 1. Clone the Repository

```bash
git clone https://github.com/yourname/orchard-engine.git
cd orchard-engine
```

### 2. Build the Engine

```bash
mkdir build
cd build
cmake .. -G Xcode
cmake --build . --config Release
```

Or open the generated Xcode project:

```bash
open OrchardEngine.xcodeproj
```

### 3. Run the Editor

```bash
./build/Release/OrchardEditor.app/Contents/MacOS/OrchardEditor
```

## Your First Project

### Creating a Scene in Swift

```swift
import OrchardEngine

class GameController: ScriptComponent {
    private var world: World?
    
    override func onInit() {
        setupScene()
    }
    
    func setupScene() {
        guard let world = world else { return }
        
        // Create a camera
        let camera = world.createEntity()
        var cameraTransform = Transform(
            position: SIMD3<Float>(0, 2, -5)
        )
        world.addComponent(camera, component: cameraTransform)
        
        // Create a cube
        let cube = world.createEntity()
        var cubeTransform = Transform(
            position: SIMD3<Float>(0, 1, 0)
        )
        world.addComponent(cube, component: cubeTransform)
    }
    
    override func onUpdate(deltaTime: Double) {
        // Game logic here
    }
}
```

### Creating a Simple App

```swift
import SwiftUI
import OrchardEngine

@main
struct MyGameApp: App {
    var body: some Scene {
        WindowGroup {
            GameView()
        }
    }
}

struct GameView: View {
    @StateObject private var engine = OrchardEngine()
    
    var body: some View {
        SceneViewWrapper()
            .onAppear {
                _ = engine.initialize(
                    appName: "My Game",
                    width: 1280,
                    height: 720
                )
            }
    }
}
```

## Core Concepts

### Entities

Entities are lightweight identifiers for game objects:

```swift
let entity = world.createEntity()
```

### Components

Components hold data:

```swift
struct Health: Component {
    var current: Float
    var maximum: Float
    
    func addToWorld(_ world: OpaquePointer, entity: CEntity) {
        // Implementation
    }
}

world.addComponent(entity, component: Health(current: 100, maximum: 100))
```

### Systems

Systems contain logic:

```swift
class HealthSystem: ScriptComponent {
    override func onUpdate(deltaTime: Double) {
        // Update all entities with Health components
    }
}
```

## Using the Editor

### Scene Hierarchy

The Scene Hierarchy panel shows all entities in your scene:
- Click to select entities
- Right-click for context menu
- Drag to reparent entities

### Inspector

The Inspector shows components for the selected entity:
- Modify component properties
- Add new components
- Remove components

### Scene View

The Scene View renders your scene:
- Left-drag to rotate camera
- Middle-drag to pan
- Scroll to zoom
- Right-click for context menu

### Transform Gizmos

- **W**: Translate (move)
- **E**: Rotate
- **R**: Scale

## Working with Assets

### Importing Models

Supported formats:
- FBX
- OBJ
- USD (Universal Scene Description)

Drag and drop into the Assets panel or:

```swift
let mesh = MeshImporter.importFBX("path/to/model.fbx")
```

### Importing Textures

Supported formats:
- PNG, JPG (LDR)
- EXR, HDR (HDR)

```swift
let texture = TextureImporter.importPNG("path/to/texture.png")
```

### Creating Materials

```swift
let material = Material()
material.setAlbedo(SIMD4<Float>(1, 0, 0, 1)) // Red
material.setMetallic(0.5)
material.setRoughness(0.3)
```

## Physics

### Adding a Rigidbody

```swift
// In C++ (through bridge):
// rigidbody.setMass(1.0)
// rigidbody.setGravityScale(1.0)
```

### Collision Detection

```swift
// Add collider to entity
let sphereCollider = SphereCollider(radius: 0.5)
```

## Audio

### Playing Sounds

```swift
let audioEngine = AudioEngine()
let clip = audioEngine.loadClip("path/to/sound.wav")

let source = audioEngine.createSource()
source.setClip(clip)
source.setVolume(0.8)
source.play()
```

### Spatial Audio

```swift
source.setSpatial(true)
source.setPosition(SIMD3<Float>(5, 0, 0))
source.setMinDistance(1.0)
source.setMaxDistance(100.0)
```

## Building for Release

### macOS

```bash
cmake --build build --config Release
```

The built app will be in:
```
build/Release/YourGame.app
```

### Code Signing

Configure in Xcode:
1. Select your project
2. Go to "Signing & Capabilities"
3. Select your team
4. Enable "Automatically manage signing"

## Next Steps

- Read the [Architecture Documentation](../Architecture/EngineArchitecture.md)
- Explore the [Rendering Pipeline](../Rendering/MetalRenderer.md)
- Check out the [Sample Projects](../../Samples/)
- Join the community forum

## Troubleshooting

### Build Errors

**Problem**: Metal shaders not compiling
```bash
xcrun -sdk macosx metal --version
```
Ensure Metal tools are installed.

**Problem**: Swift-C++ bridging issues
- Check that you're using Xcode 15+
- Verify Swift 5.9+ is installed

### Runtime Issues

**Problem**: Black screen
- Check that Metal is supported (requires Apple Silicon or AMD GPU)
- Verify shaders are compiled in build directory

**Problem**: Low FPS
- Check Activity Monitor for GPU usage
- Enable Metal Frame Capture in Xcode
- Reduce shadow quality or disable ray tracing

## Support

- GitHub Issues: https://github.com/yourname/orchard-engine/issues
- Documentation: https://docs.orchard-engine.dev
- Discord: https://discord.gg/orchard-engine
