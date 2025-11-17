import Foundation
import simd

class PhysicsDemo: ScriptComponent {
    private var world: World?
    private var spawnTimer: Float = 0.0
    private let spawnInterval: Float = 1.0
    
    override func onInit() {
        setupPhysicsWorld()
        createGround()
        createWalls()
    }
    
    override func onUpdate(deltaTime: Double) {
        spawnTimer += Float(deltaTime)
        
        if spawnTimer >= spawnInterval {
            spawnRandomObject()
            spawnTimer = 0.0
        }
        
        handleInput()
    }
    
    private func setupPhysicsWorld() {
        // Physics world is managed by the engine
        // Set gravity and other parameters
    }
    
    private func createGround() {
        guard let world = world else { return }
        
        let ground = world.createEntity()
        
        var transform = Transform(
            position: SIMD3<Float>(0, -1, 0),
            scale: SIMD3<Float>(50, 1, 50)
        )
        world.addComponent(ground, component: transform)
        
        // Add box collider
        // Add static rigidbody
    }
    
    private func createWalls() {
        let wallPositions: [SIMD3<Float>] = [
            SIMD3<Float>(-25, 5, 0),
            SIMD3<Float>(25, 5, 0),
            SIMD3<Float>(0, 5, -25),
            SIMD3<Float>(0, 5, 25)
        ]
        
        let wallScales: [SIMD3<Float>] = [
            SIMD3<Float>(1, 10, 50),
            SIMD3<Float>(1, 10, 50),
            SIMD3<Float>(50, 10, 1),
            SIMD3<Float>(50, 10, 1)
        ]
        
        guard let world = world else { return }
        
        for (position, scale) in zip(wallPositions, wallScales) {
            let wall = world.createEntity()
            
            var transform = Transform(
                position: position,
                scale: scale
            )
            world.addComponent(wall, component: transform)
            
            // Add box collider
            // Add static rigidbody
        }
    }
    
    private func spawnRandomObject() {
        guard let world = world else { return }
        
        let objectType = Int.random(in: 0..<3)
        let entity = world.createEntity()
        
        let spawnHeight: Float = 15.0
        let randomX = Float.random(in: -10...10)
        let randomZ = Float.random(in: -10...10)
        
        var transform = Transform(
            position: SIMD3<Float>(randomX, spawnHeight, randomZ)
        )
        world.addComponent(entity, component: transform)
        
        switch objectType {
        case 0:
            // Spawn cube
            createCube(entity: entity)
        case 1:
            // Spawn sphere
            createSphere(entity: entity)
        case 2:
            // Spawn capsule
            createCapsule(entity: entity)
        default:
            break
        }
        
        // Add dynamic rigidbody with random impulse
        applyRandomImpulse(entity: entity)
    }
    
    private func createCube(entity: Entity) {
        // Add cube mesh renderer
        // Add box collider
    }
    
    private func createSphere(entity: Entity) {
        // Add sphere mesh renderer
        // Add sphere collider
    }
    
    private func createCapsule(entity: Entity) {
        // Add capsule mesh renderer
        // Add capsule collider
    }
    
    private func applyRandomImpulse(entity: Entity) {
        let impulse = SIMD3<Float>(
            Float.random(in: -5...5),
            Float.random(in: 0...5),
            Float.random(in: -5...5)
        )
        
        // Apply impulse to rigidbody
    }
    
    private func handleInput() {
        if isKeyPressed(.r) {
            // Reset scene
            clearAllObjects()
        }
        
        if isMouseButtonPressed(.left) {
            // Shoot a projectile
            shootProjectile()
        }
    }
    
    private func clearAllObjects() {
        // Remove all dynamic objects from the scene
    }
    
    private func shootProjectile() {
        guard let world = world else { return }
        
        let entity = world.createEntity()
        
        // Get camera position and direction
        let cameraPos = getCameraPosition()
        let cameraDir = getCameraDirection()
        
        var transform = Transform(
            position: cameraPos + cameraDir * 2,
            scale: SIMD3<Float>(0.2, 0.2, 0.2)
        )
        world.addComponent(entity, component: transform)
        
        // Add sphere mesh and collider
        // Apply high velocity impulse in camera direction
    }
    
    private func getCameraPosition() -> SIMD3<Float> {
        return .zero
    }
    
    private func getCameraDirection() -> SIMD3<Float> {
        return SIMD3<Float>(0, 0, 1)
    }
    
    private func isKeyPressed(_ key: KeyCode) -> Bool {
        return false
    }
    
    private func isMouseButtonPressed(_ button: MouseButton) -> Bool {
        return false
    }
    
    enum KeyCode {
        case r
    }
    
    enum MouseButton {
        case left, right
    }
}
