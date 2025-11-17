set(EDITOR_SWIFT_SOURCES
    App/OrchardEditorApp.swift
    Panels/HierarchyPanel.swift
    Panels/InspectorPanel.swift
    SceneView/SceneViewWrapper.swift
)

add_executable(OrchardEditor MACOSX_BUNDLE ${EDITOR_SWIFT_SOURCES})

target_link_libraries(OrchardEditor PRIVATE
    OrchardEngineCore
    "-framework SwiftUI"
    "-framework AppKit"
)

set_target_properties(OrchardEditor PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist"
    MACOSX_BUNDLE_BUNDLE_NAME "Orchard Editor"
    MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.orchard.editor"
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.orchard.editor"
    XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME YES
    XCODE_ATTRIBUTE_SWIFT_VERSION "5.9"
)
