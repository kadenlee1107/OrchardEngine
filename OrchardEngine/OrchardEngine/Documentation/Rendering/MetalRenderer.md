# Metal Rendering Pipeline

## Overview

Orchard Engine uses Metal 3 for high-performance graphics rendering. The rendering system is built around a node-based render graph that automatically handles resource dependencies and transitions.

## Render Graph Architecture

The render graph organizes rendering into passes:

```
┌──────────────┐
│ Shadow Pass  │ (Depth-only rendering for shadows)
└──────┬───────┘
       ↓
┌──────────────┐
│ Geometry Pass│ (G-Buffer generation)
└──────┬───────┘
       ↓
┌──────────────┐
│Lighting Pass │ (Deferred lighting)
└──────┬───────┘
       ↓
┌──────────────┐
│ Forward Pass │ (Transparent objects)
└──────┬───────┘
       ↓
┌──────────────┐
│ Post Process │ (Tone mapping, AA, etc.)
└──────┬───────┘
       ↓
┌──────────────┐
│   Present    │
└──────────────┘
```

## PBR (Physically Based Rendering)

### Material Model

Orchard uses the Cook-Torrance BRDF:

```
f(l, v) = (D * F * G) / (4 * (n·l) * (n·v))
```

Where:
- **D**: Normal Distribution Function (GGX)
- **F**: Fresnel (Schlick approximation)
- **G**: Geometry function (Smith)

### Shader Implementation

The PBR shader is in `Engine/Rendering/Metal/Shaders/PBR.metal`:

```metal
fragment float4 pbr_fragment(...) {
    // Sample material textures
    float3 albedo = albedoMap.sample(...);
    float metallic = metallicMap.sample(...);
    float roughness = roughnessMap.sample(...);
    
    // Calculate lighting
    float3 Lo = calculateDirectLighting(...);
    
    // Apply tone mapping
    float3 color = toneMap(Lo);
    
    return float4(color, 1.0);
}
```

## Shadow Mapping

### Cascaded Shadow Maps (CSM)

For directional lights, we use 4 cascades:

1. **Cascade 0**: 0-10m (highest detail)
2. **Cascade 1**: 10-25m
3. **Cascade 2**: 25-50m
4. **Cascade 3**: 50-100m (lowest detail)

Each cascade renders from the light's perspective into a depth texture.

### Shadow Filtering

- **PCF**: Percentage-Closer Filtering for soft shadows
- **PCSS**: Percentage-Closer Soft Shadows (optional, expensive)

## Lighting

### Light Types

**Directional Light**:
```cpp
struct DirectionalLight {
    float3 direction;
    float3 color;
    float intensity;
    bool castShadows;
};
```

**Point Light**:
```cpp
struct PointLight {
    float3 position;
    float3 color;
    float intensity;
    float range;
};
```

**Spot Light**:
```cpp
struct SpotLight {
    float3 position;
    float3 direction;
    float3 color;
    float intensity;
    float range;
    float spotAngle;
};
```

### Clustered Forward+ Lighting

For scenes with many lights:

1. Divide screen into tiles (e.g., 16x16 pixels)
2. Calculate which lights affect each tile
3. In fragment shader, only iterate lights in current tile

Benefits:
- O(n) instead of O(n²) light evaluation
- Supports thousands of lights
- Works with transparency

## Post-Processing

### Tone Mapping

ACES tone mapping curve:

```metal
float3 ACESFilm(float3 x) {
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return saturate((x*(a*x+b))/(x*(c*x+d)+e));
}
```

### Anti-Aliasing

**FXAA** (Fast Approximate Anti-Aliasing):
- Edge detection using luminance
- Directional blur along edges
- Minimal performance cost

**TAA** (Temporal Anti-Aliasing):
- Accumulates multiple frames
- Better quality than FXAA
- Requires motion vectors

### Bloom

1. Downsample bright pixels
2. Apply Gaussian blur
3. Upsample and combine with original

## GPU-Driven Rendering

### Indirect Drawing

```metal
// Build command buffer on GPU
kernel void buildDrawCommands(...) {
    // Frustum culling
    if (isVisible(object)) {
        // Add draw command
        drawCommands[index] = MTLDrawIndexedPrimitivesIndirectArguments{
            .indexCount = object.indexCount,
            .instanceCount = 1,
            .indexStart = object.indexStart,
            .baseVertex = 0,
            .baseInstance = index
        };
    }
}

// Execute on GPU
renderEncoder.drawIndexedPrimitives(..., indirectBuffer, 0);
```

### Occlusion Culling

1. Render scene depth (previous frame)
2. Test bounding boxes against depth buffer
3. Build visible object list on GPU
4. Use for current frame rendering

## MetalFX Integration

### Temporal Upscaling

```swift
let upscaler = MTLFXTemporalScaler(device: device)
upscaler.inputWidth = renderWidth
upscaler.inputHeight = renderHeight
upscaler.outputWidth = displayWidth
upscaler.outputHeight = displayHeight

// Per frame:
upscaler.encode(
    commandBuffer: commandBuffer,
    colorTexture: lowResColor,
    depthTexture: depth,
    motionTexture: motionVectors,
    outputTexture: highResOutput
)
```

Benefits:
- Render at lower resolution (e.g., 1080p)
- Upscale to native (e.g., 4K)
- 2-3x performance improvement
- Maintains visual quality

## Ray Tracing (Optional)

### Acceleration Structures

```swift
// Build acceleration structure
let accelerationStructure = MTLAccelerationStructureDescriptor()
accelerationStructure.geometryDescriptors = geometries

let as = device.makeAccelerationStructure(descriptor: accelerationStructure)

// Trace rays
kernel void rayTrace(...) {
    ray_data ray;
    ray.origin = cameraPosition;
    ray.direction = calculateRayDirection(...);
    
    intersection_result result = intersector.intersect(
        ray,
        accelerationStructure
    );
    
    if (result.type != intersection_type::none) {
        // Handle hit
    }
}
```

### Hybrid Rendering

- Rasterize primary visibility
- Ray trace shadows
- Ray trace reflections
- Combine results

## Performance Optimization

### Best Practices

1. **Minimize State Changes**
   - Sort draw calls by material
   - Batch similar objects

2. **Use Bindless Resources**
   - Argument buffers for textures
   - Reduce bind overhead

3. **Async Compute**
   - Run post-processing on compute queue
   - Overlap with next frame's rendering

4. **Memory Management**
   - Triple buffering for uniforms
   - Reuse temporary buffers

### Profiling

Use Xcode's Metal Frame Debugger:

1. Capture a frame (Cmd+Option+C)
2. View GPU timeline
3. Check shader performance
4. Identify bottlenecks

### Common Issues

**Problem**: Low FPS
- Check draw call count (aim for < 1000)
- Verify shadow map resolution isn't too high
- Disable expensive features (AO, reflections)

**Problem**: High memory usage
- Check texture resolution
- Use texture compression (BC7, ASTC)
- Implement texture streaming

## HDR & Wide Color

### EDR (Extended Dynamic Range)

```swift
let layer = view.layer as! CAMetalLayer
layer.wantsExtendedDynamicRangeContent = true
layer.pixelFormat = .rgba16Float

// In shader:
float3 color = ...; // Linear HDR color
// Values > 1.0 will be displayed brighter on XDR displays
```

### P3 Color Space

```swift
layer.colorspace = CGColorSpace(name: CGColorSpace.displayP3)
```

## Future Enhancements

- **Mesh Shaders**: Object/mesh shader pipeline
- **Variable Rate Shading**: Per-region shading rate
- **Ray Traced GI**: Global illumination
- **Volumetric Rendering**: Clouds, fog
