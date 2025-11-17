#include <metal_stdlib>
using namespace metal;

struct FullscreenVertexOutput {
    float4 position [[position]];
    float2 texcoord;
};

vertex FullscreenVertexOutput fullscreen_vertex(uint vid [[vertex_id]]) {
    FullscreenVertexOutput out;
    
    float2 positions[6] = {
        float2(-1.0, -1.0),
        float2( 1.0, -1.0),
        float2(-1.0,  1.0),
        float2(-1.0,  1.0),
        float2( 1.0, -1.0),
        float2( 1.0,  1.0)
    };
    
    float2 texcoords[6] = {
        float2(0.0, 1.0),
        float2(1.0, 1.0),
        float2(0.0, 0.0),
        float2(0.0, 0.0),
        float2(1.0, 1.0),
        float2(1.0, 0.0)
    };
    
    out.position = float4(positions[vid], 0.0, 1.0);
    out.texcoord = texcoords[vid];
    
    return out;
}

fragment float4 tonemap_fragment(
    FullscreenVertexOutput in [[stage_in]],
    texture2d<float> hdrTexture [[texture(0)]],
    sampler texSampler [[sampler(0)]]
) {
    float3 hdrColor = hdrTexture.sample(texSampler, in.texcoord).rgb;
    
    float3 mapped = hdrColor / (hdrColor + float3(1.0));
    
    mapped = pow(mapped, float3(1.0 / 2.2));
    
    return float4(mapped, 1.0);
}

fragment float4 fxaa_fragment(
    FullscreenVertexOutput in [[stage_in]],
    texture2d<float> inputTexture [[texture(0)]],
    sampler texSampler [[sampler(0)]],
    constant float2& texelSize [[buffer(0)]]
) {
    float3 rgbNW = inputTexture.sample(texSampler, in.texcoord + float2(-1.0, -1.0) * texelSize).rgb;
    float3 rgbNE = inputTexture.sample(texSampler, in.texcoord + float2( 1.0, -1.0) * texelSize).rgb;
    float3 rgbSW = inputTexture.sample(texSampler, in.texcoord + float2(-1.0,  1.0) * texelSize).rgb;
    float3 rgbSE = inputTexture.sample(texSampler, in.texcoord + float2( 1.0,  1.0) * texelSize).rgb;
    float3 rgbM  = inputTexture.sample(texSampler, in.texcoord).rgb;
    
    float3 luma = float3(0.299, 0.587, 0.114);
    float lumaNW = dot(rgbNW, luma);
    float lumaNE = dot(rgbNE, luma);
    float lumaSW = dot(rgbSW, luma);
    float lumaSE = dot(rgbSE, luma);
    float lumaM  = dot(rgbM,  luma);
    
    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
    
    float2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));
    
    float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * 0.0625), 0.0078125);
    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
    
    dir = min(float2(8.0, 8.0), max(float2(-8.0, -8.0), dir * rcpDirMin)) * texelSize;
    
    float3 rgbA = 0.5 * (
        inputTexture.sample(texSampler, in.texcoord + dir * (1.0/3.0 - 0.5)).rgb +
        inputTexture.sample(texSampler, in.texcoord + dir * (2.0/3.0 - 0.5)).rgb
    );
    
    float3 rgbB = rgbA * 0.5 + 0.25 * (
        inputTexture.sample(texSampler, in.texcoord + dir * -0.5).rgb +
        inputTexture.sample(texSampler, in.texcoord + dir *  0.5).rgb
    );
    
    float lumaB = dot(rgbB, luma);
    
    if (lumaB < lumaMin || lumaB > lumaMax) {
        return float4(rgbA, 1.0);
    } else {
        return float4(rgbB, 1.0);
    }
}
