#include <metal_stdlib>
#include "Common.metal"

using namespace metal;

struct ShadowVertexOutput {
    float4 position [[position]];
};

vertex ShadowVertexOutput shadow_vertex(
    VertexInput in [[stage_in]],
    constant float4x4& lightViewProjection [[buffer(0)]],
    constant ModelUniforms& model [[buffer(1)]]
) {
    ShadowVertexOutput out;
    
    float4 worldPos = model.modelMatrix * float4(in.position, 1.0);
    out.position = lightViewProjection * worldPos;
    
    return out;
}

fragment void shadow_fragment(
    ShadowVertexOutput in [[stage_in]]
) {
}
