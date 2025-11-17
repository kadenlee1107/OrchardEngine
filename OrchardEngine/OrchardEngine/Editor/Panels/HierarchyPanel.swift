import SwiftUI

struct HierarchyPanel: View {
    @EnvironmentObject var editorState: EditorState
    @State private var expandedEntities: Set<UUID> = []
    
    var body: some View {
        VStack(spacing: 0) {
            // Search bar
            HStack {
                Image(systemName: "magnifyingglass")
                    .foregroundColor(.secondary)
                TextField("Search", text: $editorState.hierarchySearchText)
                    .textFieldStyle(.plain)
                
                if !editorState.hierarchySearchText.isEmpty {
                    Button(action: { editorState.hierarchySearchText = "" }) {
                        Image(systemName: "xmark.circle.fill")
                            .foregroundColor(.secondary)
                    }
                    .buttonStyle(.plain)
                }
            }
            .padding(8)
            .background(Color(nsColor: .controlBackgroundColor))
            
            Divider()
            
            // Entity list
            ScrollView {
                if let scene = editorState.currentScene {
                    LazyVStack(alignment: .leading, spacing: 2) {
                        ForEach(scene.rootEntities) { entity in
                            EntityRow(entity: entity, level: 0)
                        }
                    }
                    .padding(.vertical, 4)
                } else {
                    Text("No scene loaded")
                        .foregroundColor(.secondary)
                        .frame(maxWidth: .infinity, maxHeight: .infinity)
                }
            }
        }
        .frame(maxHeight: .infinity)
        .background(Color(nsColor: .textBackgroundColor))
    }
}

struct EntityRow: View {
    let entity: Entity
    let level: Int
    @EnvironmentObject var editorState: EditorState
    @State private var isExpanded: Bool = true
    
    var body: some View {
        VStack(spacing: 0) {
            HStack(spacing: 4) {
                // Indent
                Rectangle()
                    .fill(Color.clear)
                    .frame(width: CGFloat(level * 16))
                
                // Expand/collapse button
                if !entity.children.isEmpty {
                    Button(action: { isExpanded.toggle() }) {
                        Image(systemName: isExpanded ? "chevron.down" : "chevron.right")
                            .font(.system(size: 10))
                            .frame(width: 16, height: 16)
                    }
                    .buttonStyle(.plain)
                } else {
                    Spacer()
                        .frame(width: 16)
                }
                
                // Icon
                Image(systemName: entity.icon)
                    .foregroundColor(.secondary)
                
                // Name
                Text(entity.name)
                    .lineLimit(1)
                
                Spacer()
                
                // Visibility toggle
                Button(action: { entity.isVisible.toggle() }) {
                    Image(systemName: entity.isVisible ? "eye" : "eye.slash")
                        .foregroundColor(.secondary)
                }
                .buttonStyle(.plain)
            }
            .padding(.horizontal, 8)
            .padding(.vertical, 4)
            .background(
                editorState.selectedEntity?.id == entity.id ?
                    Color.accentColor.opacity(0.3) : Color.clear
            )
            .contentShape(Rectangle())
            .onTapGesture {
                editorState.selectedEntity = entity
            }
            .contextMenu {
                EntityContextMenu(entity: entity)
            }
            
            // Children
            if isExpanded {
                ForEach(entity.children) { child in
                    EntityRow(entity: child, level: level + 1)
                }
            }
        }
    }
}

struct EntityContextMenu: View {
    let entity: Entity
    
    var body: some View {
        Button("Duplicate") {
            // Duplicate entity
        }
        
        Button("Delete") {
            // Delete entity
        }
        
        Divider()
        
        Button("Rename") {
            // Rename entity
        }
    }
}

// Mock data structures (these would be implemented in the actual engine)
struct Entity: Identifiable {
    let id = UUID()
    var name: String
    var children: [Entity] = []
    var isVisible: Bool = true
    var icon: String = "cube"
}

struct Scene {
    var rootEntities: [Entity] = []
}

struct Project {
    var name: String
    var scenes: [Scene] = []
}
