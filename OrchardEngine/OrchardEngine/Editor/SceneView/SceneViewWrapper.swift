import SwiftUI
import MetalKit

struct SceneViewWrapper: NSViewRepresentable {
    @EnvironmentObject var editorState: EditorState
    
    func makeNSView(context: Context) -> MTKView {
        let mtkView = MTKView()
        mtkView.device = MTLCreateSystemDefaultDevice()
        mtkView.colorPixelFormat = .bgra8Unorm_srgb
        mtkView.depthStencilPixelFormat = .depth32Float
        mtkView.clearColor = MTLClearColor(red: 0.2, green: 0.2, blue: 0.2, alpha: 1.0)
        
        mtkView.delegate = context.coordinator
        
        return mtkView
    }
    
    func updateNSView(_ nsView: MTKView, context: Context) {
        // Update view if needed
    }
    
    func makeCoordinator() -> Coordinator {
        Coordinator(self)
    }
    
    class Coordinator: NSObject, MTKViewDelegate {
        var parent: SceneViewWrapper
        var commandQueue: MTLCommandQueue?
        var pipelineState: MTLRenderPipelineState?
        
        init(_ parent: SceneViewWrapper) {
            self.parent = parent
            super.init()
            
            guard let device = MTLCreateSystemDefaultDevice() else { return }
            self.commandQueue = device.makeCommandQueue()
            
            setupPipeline(device: device)
        }
        
        func setupPipeline(device: MTLDevice) {
            let library = device.makeDefaultLibrary()
            
            let pipelineDescriptor = MTLRenderPipelineDescriptor()
            pipelineDescriptor.vertexFunction = library?.makeFunction(name: "pbr_vertex")
            pipelineDescriptor.fragmentFunction = library?.makeFunction(name: "pbr_fragment")
            pipelineDescriptor.colorAttachments[0].pixelFormat = .bgra8Unorm_srgb
            pipelineDescriptor.depthAttachmentPixelFormat = .depth32Float
            
            self.pipelineState = try? device.makeRenderPipelineState(descriptor: pipelineDescriptor)
        }
        
        func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize) {
            // Handle resize
        }
        
        func draw(in view: MTKView) {
            guard let drawable = view.currentDrawable,
                  let commandQueue = commandQueue,
                  let renderPassDescriptor = view.currentRenderPassDescriptor else {
                return
            }
            
            let commandBuffer = commandQueue.makeCommandBuffer()
            let renderEncoder = commandBuffer?.makeRenderCommandEncoder(descriptor: renderPassDescriptor)
            
            // Render scene here
            if let pipelineState = pipelineState {
                renderEncoder?.setRenderPipelineState(pipelineState)
            }
            
            // Draw grid if enabled
            if parent.editorState.showGrid {
                drawGrid(renderEncoder: renderEncoder)
            }
            
            // Draw gizmos if enabled
            if parent.editorState.showGizmos {
                drawGizmos(renderEncoder: renderEncoder)
            }
            
            renderEncoder?.endEncoding()
            
            commandBuffer?.present(drawable)
            commandBuffer?.commit()
        }
        
        func drawGrid(renderEncoder: MTLRenderCommandEncoder?) {
            // Draw grid implementation
        }
        
        func drawGizmos(renderEncoder: MTLRenderCommandEncoder?) {
            // Draw transform gizmos
        }
    }
}

struct ToolbarView: View {
    @EnvironmentObject var editorState: EditorState
    
    var body: some View {
        HStack {
            // Transform tools
            HStack(spacing: 2) {
                ToolButton(icon: "arrow.up.and.down.and.arrow.left.and.right", isSelected: true)
                ToolButton(icon: "rotate.3d", isSelected: false)
                ToolButton(icon: "arrow.up.left.and.arrow.down.right", isSelected: false)
            }
            .padding(.horizontal, 8)
            
            Divider()
                .frame(height: 24)
            
            // Play controls
            HStack(spacing: 8) {
                Button(action: { editorState.play() }) {
                    Image(systemName: "play.fill")
                }
                .disabled(editorState.isPlaying)
                
                Button(action: { editorState.pause() }) {
                    Image(systemName: "pause.fill")
                }
                .disabled(!editorState.isPlaying)
                
                Button(action: { editorState.stop() }) {
                    Image(systemName: "stop.fill")
                }
                .disabled(!editorState.isPlaying)
            }
            .padding(.horizontal, 8)
            
            Spacer()
            
            // View options
            HStack(spacing: 8) {
                Toggle("Grid", isOn: $editorState.showGrid)
                    .toggleStyle(.button)
                
                Toggle("Gizmos", isOn: $editorState.showGizmos)
                    .toggleStyle(.button)
            }
            .padding(.horizontal, 8)
        }
        .padding(.vertical, 4)
        .background(Color(nsColor: .windowBackgroundColor))
    }
}

struct ToolButton: View {
    let icon: String
    let isSelected: Bool
    
    var body: some View {
        Button(action: {}) {
            Image(systemName: icon)
                .frame(width: 32, height: 32)
                .background(isSelected ? Color.accentColor.opacity(0.2) : Color.clear)
                .cornerRadius(4)
        }
        .buttonStyle(.plain)
    }
}

struct TimelineView: View {
    @State private var currentFrame: Int = 0
    @State private var frameRange: ClosedRange<Int> = 0...120
    
    var body: some View {
        VStack(spacing: 0) {
            Divider()
            
            HStack {
                // Playback controls
                HStack(spacing: 4) {
                    Button(action: {}) {
                        Image(systemName: "backward.frame")
                    }
                    
                    Button(action: {}) {
                        Image(systemName: "play.fill")
                    }
                    
                    Button(action: {}) {
                        Image(systemName: "forward.frame")
                    }
                }
                .padding(.horizontal)
                
                // Timeline slider
                Slider(value: .init(get: { Double(currentFrame) },
                                   set: { currentFrame = Int($0) }),
                      in: Double(frameRange.lowerBound)...Double(frameRange.upperBound))
                    .padding(.horizontal)
                
                // Frame display
                Text("\(currentFrame) / \(frameRange.upperBound)")
                    .font(.system(.body, design: .monospaced))
                    .padding(.horizontal)
            }
            .padding(.vertical, 8)
            
            Divider()
            
            // Animation tracks would go here
            ScrollView {
                VStack {
                    Text("Animation Timeline")
                        .foregroundColor(.secondary)
                }
                .frame(maxWidth: .infinity, maxHeight: .infinity)
            }
        }
        .background(Color(nsColor: .textBackgroundColor))
    }
}

struct MaterialEditorView: View {
    var body: some View {
        Text("Material Editor")
            .frame(maxWidth: .infinity, maxHeight: .infinity)
    }
}

struct ShaderGraphView: View {
    var body: some View {
        Text("Shader Graph Editor")
            .frame(maxWidth: .infinity, maxHeight: .infinity)
    }
}

struct SettingsView: View {
    var body: some View {
        TabView {
            Text("General Settings")
                .tabItem {
                    Label("General", systemImage: "gear")
                }
            
            Text("Rendering Settings")
                .tabItem {
                    Label("Rendering", systemImage: "paintbrush")
                }
            
            Text("Physics Settings")
                .tabItem {
                    Label("Physics", systemImage: "atom")
                }
        }
        .frame(width: 500, height: 400)
    }
}
