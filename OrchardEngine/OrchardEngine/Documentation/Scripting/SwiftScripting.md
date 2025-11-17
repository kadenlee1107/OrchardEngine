# Swift Scripting Guide

## Overview

Orchard Engine supports Swift as a first-class scripting language, providing high-level gameplay scripting while maintaining native performance through the C++ engine core.

## Component-Based Scripting

### Creating a Component

```swift
import OrchardEngine

class PlayerController: ScriptComponent {
    // Properties
    var moveSpeed: Float = 5.0
    var jumpForce: Float = 7.0
    
    // Component lifecycle
    override func onInit() {
        print("Player initialized")
    }
    
    override func onUpdate(deltaTime: Double) {
        handleInput(Float(deltaTime))
    }
    
    override func onDestroy() {
        print("Player destroyed")
    }
    
    private func handleInput(_ dt: Float) {
        // Input handling logic
    }
}
```

### Lifecycle Methods

- `onInit()`: Called once when component is added
- `onUpdate(deltaTime:)`: Called every frame
- `onFixedUpdate(deltaTime:)`: Called at fixed intervals (for physics)
- `onLateUpdate()`: Called after all updates
- `onDestroy()`: Called when entity is destroyed

## Working with Entities

### Creating Entities

```swift
let world = World()

// Create empty entity
let entity = world.createEntity()

// Add components
var transform = Transform(
    position: SIMD3<Float>(0, 0, 0),
    rotation: SIMD4<Float>(0, 0, 0, 1),
    scale: SIMD3<Float>(1, 1, 1)
)
world.addComponent(entity, component: transform)
```

### Querying Components

```swift
// Get component from entity
if let transform = world.getComponent(entity, Transform.self) {
    print("Position: \(transform.position)")
}

// Check if entity has component
if world.hasComponent(entity, Health.self) {
    // Entity has health
}
```

### Entity Hierarchies

```swift
// Set parent-child relationship
transform.parent = parentEntity
transform.children.append(childEntity)

// Get world position (including parent transforms)
let worldMatrix = transform.getWorldMatrix()
```

## Input Handling

### Keyboard Input

```swift
class InputController: ScriptComponent {
    override func onUpdate(deltaTime: Double) {
        if Input.isKeyPressed(.w) {
            // Move forward
        }
        
        if Input.isKeyDown(.space) {
            // Jump (single press)
        }
        
        if Input.isKeyReleased(.e) {
            // Interact (on release)
        }
    }
}
```

### Mouse Input

```swift
// Mouse position
let mousePos = Input.mousePosition

// Mouse delta (for camera)
let delta = Input.mouseDelta

// Mouse buttons
if Input.isMouseButtonPressed(.left) {
    // Fire weapon
}

// Mouse wheel
let scroll = Input.mouseScroll
```

### Gamepad Input

```swift
// Check if gamepad is connected
if Input.isGamepadConnected(0) {
    // Left stick
    let leftStick = Input.getGamepadAxis(.leftStick, player: 0)
    
    // Buttons
    if Input.isGamepadButtonPressed(.a, player: 0) {
        // Jump
    }
}
```

## Physics Integration

### Adding Physics

```swift
// Create rigidbody
let rb = Rigidbody()
rb.setMass(1.0)
rb.setGravityScale(1.0)
entity.addComponent(rb)

// Add collider
let collider = BoxCollider(halfExtents: SIMD3<Float>(0.5, 0.5, 0.5))
entity.addComponent(collider)
```

### Applying Forces

```swift
class PhysicsController: ScriptComponent {
    var rigidbody: Rigidbody?
    
    func jump() {
        rigidbody?.addForce(SIMD3<Float>(0, 500, 0))
    }
    
    func moveInDirection(_ direction: SIMD3<Float>) {
        rigidbody?.setLinearVelocity(direction * 5.0)
    }
}
```

### Collision Callbacks

```swift
class CollisionHandler: ScriptComponent {
    func onCollisionEnter(_ collision: Collision) {
        print("Hit: \(collision.other.name)")
    }
    
    func onCollisionStay(_ collision: Collision) {
        // Continuous contact
    }
    
    func onCollisionExit(_ collision: Collision) {
        print("Lost contact")
    }
    
    func onTriggerEnter(_ other: Entity) {
        print("Entered trigger")
    }
}
```

## Audio

### Playing Sounds

```swift
class AudioController: ScriptComponent {
    var audioSource: AudioSource?
    var footstepSound: AudioClip?
    
    override func onInit() {
        audioSource = entity.getComponent(AudioSource.self)
        footstepSound = Resources.load("footstep.wav")
    }
    
    func playFootstep() {
        audioSource?.playOneShot(footstepSound!)
    }
    
    func playMusic() {
        audioSource?.setClip(musicClip)
        audioSource?.setLoop(true)
        audioSource?.play()
    }
}
```

### 3D Audio

```swift
// Enable spatial audio
audioSource.setSpatial(true)
audioSource.setMinDistance(1.0)
audioSource.setMaxDistance(100.0)

// Audio will automatically attenuate based on distance from listener
```

## Animation

### Playing Animations

```swift
class AnimationController: ScriptComponent {
    var animator: Animator?
    
    func playAnimation(_ name: String) {
        animator?.play(name)
    }
    
    func setAnimationSpeed(_ speed: Float) {
        animator?.setSpeed(speed)
    }
    
    func blendAnimations(_ anim1: String, _ anim2: String, weight: Float) {
        animator?.blend(anim1, anim2, weight: weight)
    }
}
```

## Coroutines

### Async Behavior

```swift
class CoroutineExample: ScriptComponent {
    override func onInit() {
        startCoroutine(fadeIn())
    }
    
    func fadeIn() async {
        var alpha: Float = 0.0
        
        while alpha < 1.0 {
            alpha += 0.01
            material.setFloat("alpha", alpha)
            await waitForSeconds(0.016) // ~60fps
        }
    }
    
    func waitForSeconds(_ seconds: Double) async {
        // Implementation by engine
    }
}
```

## Event System

### Subscribing to Events

```swift
class EventHandler: ScriptComponent {
    override func onInit() {
        EventBus.subscribe(PlayerDiedEvent.self) { event in
            self.onPlayerDied(event)
        }
    }
    
    func onPlayerDied(_ event: PlayerDiedEvent) {
        print("Game Over!")
        // Show game over screen
    }
}
```

### Dispatching Events

```swift
struct PlayerDiedEvent: GameEvent {
    let player: Entity
    let cause: String
}

// Dispatch event
EventBus.dispatch(PlayerDiedEvent(
    player: playerEntity,
    cause: "fell into pit"
))
```

## Performance Tips

### Minimize Allocations

```swift
// Bad: Creates new array every frame
override func onUpdate(deltaTime: Double) {
    let enemies = [enemy1, enemy2, enemy3]
    checkEnemies(enemies)
}

// Good: Reuse array
var enemies: [Entity] = []

override func onInit() {
    enemies = [enemy1, enemy2, enemy3]
}

override func onUpdate(deltaTime: Double) {
    checkEnemies(enemies)
}
```

### Use Value Types

```swift
// Prefer structs (value types) over classes for components
struct Position: Component {
    var x, y, z: Float
}

// Not: class Position
```

### Cache Component References

```swift
class OptimizedController: ScriptComponent {
    // Cache components in onInit
    var transform: Transform?
    var rigidbody: Rigidbody?
    
    override func onInit() {
        transform = entity.getComponent(Transform.self)
        rigidbody = entity.getComponent(Rigidbody.self)
    }
    
    override func onUpdate(deltaTime: Double) {
        // Use cached references (fast)
        transform?.position.y += 1.0
        
        // Not: entity.getComponent(Transform.self)?.position.y += 1.0 (slow)
    }
}
```

## Hot Reload

Orchard supports hot-reloading of Swift scripts:

1. Make changes to your Swift script
2. Save the file
3. Press Cmd+R in the editor
4. Script recompiles and reloads without restarting the game

**Note**: State is preserved where possible using serialization.

## Debugging

### Logging

```swift
// Different log levels
Log.info("Player spawned at \(position)")
Log.warning("Low health: \(health)")
Log.error("Failed to load resource")
Log.debug("Frame time: \(deltaTime)")
```

### Drawing Debug Shapes

```swift
// Draw debug line
Debug.drawLine(start, end, color: .red, duration: 1.0)

// Draw debug sphere
Debug.drawSphere(center, radius: 1.0, color: .green)

// Draw debug box
Debug.drawBox(center, size, rotation, color: .blue)
```

## Best Practices

1. **Keep components small**: Single responsibility principle
2. **Prefer composition**: Use multiple components instead of inheritance
3. **Avoid Update() for static logic**: Use events or initialization
4. **Profile your code**: Use Instruments to find bottlenecks
5. **Use property wrappers**: For serialization and editor exposure

## Example: Complete Character Controller

See [FirstPersonController.swift](../../Samples/FirstPersonDemo/FirstPersonController.swift) for a complete example.
