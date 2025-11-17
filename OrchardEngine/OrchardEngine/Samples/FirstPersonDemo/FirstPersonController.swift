import Foundation
import simd

class FirstPersonController: ScriptComponent {
    // Movement settings
    var moveSpeed: Float = 5.0
    var sprintMultiplier: Float = 2.0
    var jumpForce: Float = 5.0
    var mouseSensitivity: Float = 0.002
    
    // State
    private var velocity = SIMD3<Float>.zero
    private var isGrounded = false
    private var pitch: Float = 0.0
    private var yaw: Float = 0.0
    
    // References
    private var transform: Transform?
    private var rigidbody: Rigidbody?
    
    override func onInit() {
        // Lock cursor
        lockCursor()
    }
    
    override func onUpdate(deltaTime: Double) {
        let dt = Float(deltaTime)
        
        handleMouseLook()
        handleMovement(dt)
        handleJump()
        
        updateRotation()
    }
    
    private func handleMouseLook() {
        // Get mouse delta from input system
        let mouseDelta = getMouseDelta()
        
        yaw += mouseDelta.x * mouseSensitivity
        pitch -= mouseDelta.y * mouseSensitivity
        
        pitch = max(-Float.pi / 2, min(Float.pi / 2, pitch))
    }
    
    private func handleMovement(_ dt: Float) {
        var moveDirection = SIMD3<Float>.zero
        
        // Get input
        if isKeyPressed(.w) { moveDirection.z += 1 }
        if isKeyPressed(.s) { moveDirection.z -= 1 }
        if isKeyPressed(.a) { moveDirection.x -= 1 }
        if isKeyPressed(.d) { moveDirection.x += 1 }
        
        if simd_length(moveDirection) > 0 {
            moveDirection = simd_normalize(moveDirection)
        }
        
        // Apply sprint
        var speed = moveSpeed
        if isKeyPressed(.leftShift) {
            speed *= sprintMultiplier
        }
        
        // Transform direction to world space based on yaw
        let forward = SIMD3<Float>(sin(yaw), 0, cos(yaw))
        let right = SIMD3<Float>(cos(yaw), 0, -sin(yaw))
        
        let worldDirection = forward * moveDirection.z + right * moveDirection.x
        
        // Apply velocity
        if let rb = rigidbody {
            var vel = rb.getLinearVelocity()
            vel.x = worldDirection.x * speed
            vel.z = worldDirection.z * speed
            rb.setLinearVelocity(vel)
        }
    }
    
    private func handleJump() {
        if isKeyPressed(.space) && isGrounded {
            if let rb = rigidbody {
                var vel = rb.getLinearVelocity()
                vel.y = jumpForce
                rb.setLinearVelocity(vel)
                isGrounded = false
            }
        }
    }
    
    private func updateRotation() {
        // Update camera rotation based on pitch and yaw
        if let trans = transform {
            let rotation = simd_quatf(angle: yaw, axis: SIMD3<Float>(0, 1, 0))
            trans.rotation = SIMD4<Float>(rotation.imag.x, rotation.imag.y, 
                                         rotation.imag.z, rotation.real)
        }
    }
    
    // Placeholder methods that would be implemented by the engine
    private func getMouseDelta() -> SIMD2<Float> {
        return .zero
    }
    
    private func isKeyPressed(_ key: KeyCode) -> Bool {
        return false
    }
    
    private func lockCursor() {
        // Lock and hide cursor for FPS controls
    }
    
    enum KeyCode {
        case w, a, s, d, space, leftShift
    }
}

struct Rigidbody {
    func getLinearVelocity() -> SIMD3<Float> {
        return .zero
    }
    
    func setLinearVelocity(_ velocity: SIMD3<Float>) {
    }
}
