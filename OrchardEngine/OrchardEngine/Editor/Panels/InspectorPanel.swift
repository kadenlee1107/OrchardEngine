import SwiftUI

struct InspectorPanel: View {
    @EnvironmentObject var editorState: EditorState
    
    var body: some View {
        VStack(spacing: 0) {
            if let entity = editorState.selectedEntity {
                ScrollView {
                    VStack(spacing: 12) {
                        // Entity Header
                        EntityHeader(entity: entity)
                        
                        Divider()
                        
                        // Components
                        TransformComponent(entity: entity)
                        
                        Divider()
                        
                        if entity.hasComponent(MeshRenderer.self) {
                            MeshRendererComponent()
                            Divider()
                        }
                        
                        if entity.hasComponent(Camera.self) {
                            CameraComponent()
                            Divider()
                        }
                        
                        if entity.hasComponent(Light.self) {
                            LightComponent()
                            Divider()
                        }
                        
                        // Add Component Button
                        Menu {
                            Button("Mesh Renderer") { }
                            Button("Camera") { }
                            Button("Light") { }
                            Button("Rigidbody") { }
                            Button("Collider") { }
                            Button("Audio Source") { }
                        } label: {
                            Label("Add Component", systemImage: "plus.circle")
                                .frame(maxWidth: .infinity)
                        }
                        .menuStyle(.borderlessButton)
                        .padding(.horizontal)
                    }
                    .padding(.vertical)
                }
            } else {
                VStack {
                    Image(systemName: "cube.transparent")
                        .font(.system(size: 48))
                        .foregroundColor(.secondary)
                    Text("No entity selected")
                        .foregroundColor(.secondary)
                }
                .frame(maxWidth: .infinity, maxHeight: .infinity)
            }
        }
        .background(Color(nsColor: .textBackgroundColor))
    }
}

struct EntityHeader: View {
    let entity: Entity
    @State private var name: String
    
    init(entity: Entity) {
        self.entity = entity
        _name = State(initialValue: entity.name)
    }
    
    var body: some View {
        VStack(alignment: .leading, spacing: 8) {
            HStack {
                Toggle("", isOn: .constant(entity.isVisible))
                    .labelsHidden()
                
                TextField("Name", text: $name)
                    .textFieldStyle(.plain)
                    .font(.headline)
            }
            
            HStack {
                Label("Static", systemImage: "pin")
                    .font(.caption)
                    .foregroundColor(.secondary)
                
                Spacer()
                
                Label("Tag: Untagged", systemImage: "tag")
                    .font(.caption)
                    .foregroundColor(.secondary)
            }
        }
        .padding(.horizontal)
    }
}

struct TransformComponent: View {
    let entity: Entity
    @State private var position = SIMD3<Float>(0, 0, 0)
    @State private var rotation = SIMD3<Float>(0, 0, 0)
    @State private var scale = SIMD3<Float>(1, 1, 1)
    
    var body: some View {
        ComponentSection(title: "Transform", icon: "move.3d") {
            VStack(spacing: 8) {
                Vector3Field(label: "Position", value: $position, icon: "arrow.up.and.down.and.arrow.left.and.right")
                Vector3Field(label: "Rotation", value: $rotation, icon: "rotate.3d")
                Vector3Field(label: "Scale", value: $scale, icon: "arrow.up.left.and.arrow.down.right")
            }
        }
    }
}

struct MeshRendererComponent: View {
    @State private var castShadows = true
    @State private var receiveShadows = true
    
    var body: some View {
        ComponentSection(title: "Mesh Renderer", icon: "cube") {
            VStack(alignment: .leading, spacing: 8) {
                ObjectField(label: "Mesh", value: "Cube")
                ObjectField(label: "Material", value: "Default")
                
                Toggle("Cast Shadows", isOn: $castShadows)
                Toggle("Receive Shadows", isOn: $receiveShadows)
            }
        }
    }
}

struct CameraComponent: View {
    @State private var fov: Float = 60
    @State private var nearPlane: Float = 0.1
    @State private var farPlane: Float = 1000
    
    var body: some View {
        ComponentSection(title: "Camera", icon: "camera") {
            VStack(spacing: 8) {
                SliderField(label: "FOV", value: $fov, range: 1...179)
                FloatField(label: "Near Plane", value: $nearPlane)
                FloatField(label: "Far Plane", value: $farPlane)
            }
        }
    }
}

struct LightComponent: View {
    @State private var lightType = "Directional"
    @State private var color = Color.white
    @State private var intensity: Float = 1.0
    
    var body: some View {
        ComponentSection(title: "Light", icon: "lightbulb") {
            VStack(spacing: 8) {
                Picker("Type", selection: $lightType) {
                    Text("Directional").tag("Directional")
                    Text("Point").tag("Point")
                    Text("Spot").tag("Spot")
                }
                
                ColorPicker("Color", selection: $color)
                SliderField(label: "Intensity", value: $intensity, range: 0...10)
            }
        }
    }
}

// Helper Views
struct ComponentSection<Content: View>: View {
    let title: String
    let icon: String
    @ViewBuilder let content: Content
    @State private var isExpanded = true
    
    var body: some View {
        VStack(alignment: .leading, spacing: 8) {
            Button(action: { isExpanded.toggle() }) {
                HStack {
                    Image(systemName: icon)
                    Text(title)
                        .font(.headline)
                    Spacer()
                    Image(systemName: isExpanded ? "chevron.down" : "chevron.right")
                }
            }
            .buttonStyle(.plain)
            .padding(.horizontal)
            
            if isExpanded {
                content
                    .padding(.horizontal)
            }
        }
    }
}

struct Vector3Field: View {
    let label: String
    @Binding var value: SIMD3<Float>
    let icon: String
    
    var body: some View {
        VStack(alignment: .leading, spacing: 4) {
            Label(label, systemImage: icon)
                .font(.caption)
                .foregroundColor(.secondary)
            
            HStack {
                HStack {
                    Text("X")
                        .foregroundColor(.red)
                    TextField("", value: $value.x, format: .number)
                        .textFieldStyle(.roundedBorder)
                }
                
                HStack {
                    Text("Y")
                        .foregroundColor(.green)
                    TextField("", value: $value.y, format: .number)
                        .textFieldStyle(.roundedBorder)
                }
                
                HStack {
                    Text("Z")
                        .foregroundColor(.blue)
                    TextField("", value: $value.z, format: .number)
                        .textFieldStyle(.roundedBorder)
                }
            }
        }
    }
}

struct FloatField: View {
    let label: String
    @Binding var value: Float
    
    var body: some View {
        HStack {
            Text(label)
                .frame(width: 100, alignment: .leading)
            TextField("", value: $value, format: .number)
                .textFieldStyle(.roundedBorder)
        }
    }
}

struct SliderField: View {
    let label: String
    @Binding var value: Float
    let range: ClosedRange<Float>
    
    var body: some View {
        VStack(alignment: .leading, spacing: 4) {
            HStack {
                Text(label)
                Spacer()
                Text(String(format: "%.2f", value))
                    .foregroundColor(.secondary)
            }
            Slider(value: $value, in: range)
        }
    }
}

struct ObjectField: View {
    let label: String
    let value: String
    
    var body: some View {
        HStack {
            Text(label)
                .frame(width: 100, alignment: .leading)
            Text(value)
                .foregroundColor(.secondary)
                .frame(maxWidth: .infinity, alignment: .leading)
                .padding(6)
                .background(Color(nsColor: .controlBackgroundColor))
                .cornerRadius(4)
            
            Button(action: {}) {
                Image(systemName: "circle.fill")
                    .font(.system(size: 8))
            }
            .buttonStyle(.plain)
        }
    }
}

extension Entity {
    func hasComponent<T>(_ type: T.Type) -> Bool {
        return false // Mock implementation
    }
}

struct MeshRenderer {}
struct Camera {}
struct Light {}
