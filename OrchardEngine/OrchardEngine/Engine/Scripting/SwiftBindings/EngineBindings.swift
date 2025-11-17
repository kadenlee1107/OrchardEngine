import Foundation

@objc public class OrchardEngine: NSObject {
    private var cppEngine: OpaquePointer?
    
    public override init() {
        super.init()
        cppEngine = engine_create()
    }
    
    deinit {
        if let engine = cppEngine {
            engine_destroy(engine)
        }
    }
    
    public func initialize(appName: String, width: UInt32, height: UInt32) -> Bool {
        guard let engine = cppEngine else { return false }
        return appName.withCString { namePtr in
            return engine_initialize(engine, namePtr, width, height)
        }
    }
    
    public func shutdown() {
        if let engine = cppEngine {
            engine_shutdown(engine)
        }
    }
    
    public func run() {
        if let engine = cppEngine {
            engine_run(engine)
        }
    }
    
    public func requestExit() {
        if let engine = cppEngine {
            engine_request_exit(engine)
        }
    }
    
    public var deltaTime: Double {
        guard let engine = cppEngine else { return 0.0 }
        return engine_get_delta_time(engine)
    }
    
    public var frameCount: UInt64 {
        guard let engine = cppEngine else { return 0 }
        return engine_get_frame_count(engine)
    }
}

@objc public class World: NSObject {
    private var cppWorld: OpaquePointer?
    
    public override init() {
        super.init()
        cppWorld = world_create()
    }
    
    deinit {
        if let world = cppWorld {
            world_destroy(world)
        }
    }
    
    public func createEntity() -> Entity {
        guard let world = cppWorld else { return Entity(id: 0, version: 0) }
        let entityID = world_create_entity(world)
        return Entity(id: entityID.id, version: entityID.version)
    }
    
    public func destroyEntity(_ entity: Entity) {
        guard let world = cppWorld else { return }
        var cEntity = CEntity(id: entity.id, version: entity.version)
        world_destroy_entity(world, cEntity)
    }
    
    public func addComponent<T: Component>(_ entity: Entity, component: T) {
        guard let world = cppWorld else { return }
        var cEntity = CEntity(id: entity.id, version: entity.version)
        component.addToWorld(world, entity: cEntity)
    }
    
    public func update(deltaTime: Double) {
        guard let world = cppWorld else { return }
        world_update(world, deltaTime)
    }
}

public struct Entity: Equatable {
    public let id: UInt64
    public let version: UInt32
    
    public static func == (lhs: Entity, rhs: Entity) -> Bool {
        return lhs.id == rhs.id && lhs.version == rhs.version
    }
}

public protocol Component {
    func addToWorld(_ world: OpaquePointer, entity: CEntity)
}

public struct Transform: Component {
    public var position: SIMD3<Float>
    public var rotation: SIMD4<Float>
    public var scale: SIMD3<Float>
    
    public init(position: SIMD3<Float> = .zero,
                rotation: SIMD4<Float> = SIMD4<Float>(0, 0, 0, 1),
                scale: SIMD3<Float> = SIMD3<Float>(1, 1, 1)) {
        self.position = position
        self.rotation = rotation
        self.scale = scale
    }
    
    public func addToWorld(_ world: OpaquePointer, entity: CEntity) {
        var cTransform = CTransform(
            positionX: position.x, positionY: position.y, positionZ: position.z,
            rotationX: rotation.x, rotationY: rotation.y, rotationZ: rotation.z, rotationW: rotation.w,
            scaleX: scale.x, scaleY: scale.y, scaleZ: scale.z
        )
        world_add_transform_component(world, entity, &cTransform)
    }
}

public class ScriptComponent: Component {
    public func onInit() {}
    public func onUpdate(deltaTime: Double) {}
    public func onDestroy() {}
    
    public func addToWorld(_ world: OpaquePointer, entity: CEntity) {
        // Custom implementation for script components
    }
}

struct CEntity {
    var id: UInt64
    var version: UInt32
}

struct CTransform {
    var positionX, positionY, positionZ: Float
    var rotationX, rotationY, rotationZ, rotationW: Float
    var scaleX, scaleY, scaleZ: Float
}

@_silgen_name("engine_create")
func engine_create() -> OpaquePointer

@_silgen_name("engine_destroy")
func engine_destroy(_ engine: OpaquePointer)

@_silgen_name("engine_initialize")
func engine_initialize(_ engine: OpaquePointer, _ appName: UnsafePointer<CChar>, _ width: UInt32, _ height: UInt32) -> Bool

@_silgen_name("engine_shutdown")
func engine_shutdown(_ engine: OpaquePointer)

@_silgen_name("engine_run")
func engine_run(_ engine: OpaquePointer)

@_silgen_name("engine_request_exit")
func engine_request_exit(_ engine: OpaquePointer)

@_silgen_name("engine_get_delta_time")
func engine_get_delta_time(_ engine: OpaquePointer) -> Double

@_silgen_name("engine_get_frame_count")
func engine_get_frame_count(_ engine: OpaquePointer) -> UInt64

@_silgen_name("world_create")
func world_create() -> OpaquePointer

@_silgen_name("world_destroy")
func world_destroy(_ world: OpaquePointer)

@_silgen_name("world_create_entity")
func world_create_entity(_ world: OpaquePointer) -> CEntity

@_silgen_name("world_destroy_entity")
func world_destroy_entity(_ world: OpaquePointer, _ entity: CEntity)

@_silgen_name("world_add_transform_component")
func world_add_transform_component(_ world: OpaquePointer, _ entity: CEntity, _ transform: UnsafeMutablePointer<CTransform>)

@_silgen_name("world_update")
func world_update(_ world: OpaquePointer, _ deltaTime: Double)
