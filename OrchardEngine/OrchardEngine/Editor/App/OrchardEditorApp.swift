import SwiftUI
import MetalKit

@main
struct OrchardEditorApp: App {
    @StateObject private var editorState = EditorState()
    
    var body: some Scene {
        WindowGroup {
            ContentView()
                .environmentObject(editorState)
        }
        .commands {
            EditorCommands()
        }
        
        WindowGroup("Material Editor", id: "material-editor") {
            MaterialEditorView()
                .environmentObject(editorState)
        }
        
        WindowGroup("Shader Graph", id: "shader-graph") {
            ShaderGraphView()
                .environmentObject(editorState)
        }
        
        Settings {
            SettingsView()
        }
    }
}

class EditorState: ObservableObject {
    @Published var selectedEntity: Entity?
    @Published var currentScene: Scene?
    @Published var isPlaying: Bool = false
    @Published var showGrid: Bool = true
    @Published var showGizmos: Bool = true
    
    @Published var hierarchySearchText: String = ""
    @Published var assetSearchText: String = ""
    
    var undoManager = UndoManager()
    var project: Project?
    
    init() {
        setupDefaultProject()
    }
    
    func setupDefaultProject() {
        // Initialize default project
    }
    
    func play() {
        isPlaying = true
    }
    
    func pause() {
        isPlaying = false
    }
    
    func stop() {
        isPlaying = false
    }
}

struct ContentView: View {
    @EnvironmentObject var editorState: EditorState
    @State private var sceneViewSize: CGSize = .zero
    
    var body: some View {
        HSplitView {
            // Left sidebar - Hierarchy
            HierarchyPanel()
                .frame(minWidth: 200, idealWidth: 250, maxWidth: 400)
            
            // Center - Scene View
            VStack(spacing: 0) {
                ToolbarView()
                    .frame(height: 44)
                
                SceneViewWrapper()
                    .background(Color.black)
                
                TimelineView()
                    .frame(height: 200)
            }
            
            // Right sidebar - Inspector
            InspectorPanel()
                .frame(minWidth: 250, idealWidth: 300, maxWidth: 500)
        }
        .frame(minWidth: 1024, minHeight: 768)
    }
}

struct EditorCommands: Commands {
    var body: some Commands {
        CommandGroup(replacing: .newItem) {
            Button("New Scene") {
                // Create new scene
            }
            .keyboardShortcut("n", modifiers: [.command])
            
            Button("New Entity") {
                // Create new entity
            }
            .keyboardShortcut("n", modifiers: [.command, .shift])
        }
        
        CommandMenu("GameObject") {
            Button("Create Empty") {
                // Create empty entity
            }
            
            Divider()
            
            Menu("3D Object") {
                Button("Cube") { }
                Button("Sphere") { }
                Button("Cylinder") { }
                Button("Plane") { }
                Button("Capsule") { }
            }
            
            Menu("Light") {
                Button("Directional Light") { }
                Button("Point Light") { }
                Button("Spot Light") { }
            }
            
            Menu("Camera") {
                Button("Camera") { }
            }
        }
    }
}
