import Foundation
import simd

class ThirdPersonController: ScriptComponent {
    // Camera settings
    var cameraDistance: Float = 5.0
    var cameraHeight: Float = 2.0
    var cameraRotationSpeed: Float = 100.0
    var cameraSmoothSpeed: Float = 10.0
    
    // Movement settings
    var moveSpeed: Float = 4.0
    var rotationSpeed: Float = 10.0
    var jumpForce: Float = 5.0
    
    // State
    private var cameraYaw: Float = 0.0
    private var cameraPitch: Float = 20.0
    private var currentCameraOffset = SIMD3<Float>.zero
    private var isGrounded = false
    
    // References
    private var transform: Transform?
    private var cameraTransform: Transform?
    private var rigidbody: Rigidbody?
    
    override func onInit() {
        setupCamera()
    }
    
    override func onUpdate(deltaTime: Double) {
        let dt = Float(deltaTime)
        
        handleCameraRotation(dt)
        handleMovement(dt)
        handleJump()
        updateCamera(dt)
    }
    
    private func setupCamera() {
        // Create camera entity and attach to player
    }
    
    private func handleCameraRotation(_ dt: Float) {
        let mouseDelta = getMouseDelta()
        
        cameraYaw += mouseDelta.x * cameraRotationSpeed * dt
        cameraPitch -= mouseDelta.y * cameraRotationSpeed * dt
        
        cameraPitch = max(-80.0, min(80.0, cameraPitch))
    }
    
    private func handleMovement(_ dt: Float) {
        var moveDirection = SIMD2<Float>.zero
        
        if isKeyPressed(.w) { moveDirection.y += 1 }
        if isKeyPressed(.s) { moveDirection.y -= 1 }
        if isKeyPressed(.a) { moveDirection.x -= 1 }
        if isKeyPressed(.d) { moveDirection.x += 1 }
        
        if simd_length(moveDirection) > 0 {
            moveDirection = simd_normalize(moveDirection)
            
            // Calculate movement direction relative to camera
            let radYaw = cameraYaw * Float.pi / 180.0
            let forward = SIMD3<Float>(sin(radYaw), 0, cos(radYaw))
            let right = SIMD3<Float>(cos(radYaw), 0, -sin(radYaw))
            
            let worldDirection = forward * moveDirection.y + right * moveDirection.x
            
            // Apply movement
            if let rb = rigidbody {
                var vel = rb.getLinearVelocity()
                vel.x = worldDirection.x * moveSpeed
                vel.z = worldDirection.z * moveSpeed
                rb.setLinearVelocity(vel)
            }
            
            // Rotate character to face movement direction
            if let trans = transform {
                let targetYaw = atan2(worldDirection.x, worldDirection.z)
                let currentYaw = getCurrentYaw(trans)
                let newYaw = lerpAngle(currentYaw, targetYaw, rotationSpeed * dt)
                
                let rotation = simd_quatf(angle: newYaw, axis: SIMD3<Float>(0, 1, 0))
                trans.rotation = SIMD4<Float>(rotation.imag.x, rotation.imag.y,
                                             rotation.imag.z, rotation.real)
            }
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
    
    private func updateCamera(_ dt: Float) {
        guard let trans = transform, let camTrans = cameraTransform else { return }
        
        // Calculate desired camera position
        let radYaw = cameraYaw * Float.pi / 180.0
        let radPitch = cameraPitch * Float.pi / 180.0
        
        let offset = SIMD3<Float>(
            sin(radYaw) * cos(radPitch) * cameraDistance,
            sin(radPitch) * cameraDistance + cameraHeight,
            cos(radYaw) * cos(radPitch) * cameraDistance
        )
        
        let targetPosition = trans.position - offset
        
        // Smoothly interpolate camera position
        currentCameraOffset = lerp(currentCameraOffset, offset, cameraSmoothSpeed * dt)
        camTrans.position = trans.position - currentCameraOffset
        
        // Look at player
        let direction = simd_normalize(trans.position - camTrans.position)
        let lookRotation = lookRotationFromDirection(direction)
        camTrans.rotation = lookRotation
    }
    
    private func getCurrentYaw(_ trans: Transform) -> Float {
        return 0.0 // Extract yaw from quaternion
    }
    
    private func lerpAngle(_ a: Float, _ b: Float, _ t: Float) -> Float {
        var delta = b - a
        while delta > Float.pi { delta -= 2 * Float.pi }
        while delta < -Float.pi { delta += 2 * Float.pi }
        return a + delta * t
    }
    
    private func lerp(_ a: SIMD3<Float>, _ b: SIMD3<Float>, _ t: Float) -> SIMD3<Float> {
        return a + (b - a) * t
    }
    
    private func lookRotationFromDirection(_ direction: SIMD3<Float>) -> SIMD4<Float> {
        return .zero // Calculate quaternion from direction
    }
    
    private func getMouseDelta() -> SIMD2<Float> {
        return .zero
    }
    
    private func isKeyPressed(_ key: KeyCode) -> Bool {
        return false
    }
    
    enum KeyCode {
        case w, a, s, d, space
    }
}
