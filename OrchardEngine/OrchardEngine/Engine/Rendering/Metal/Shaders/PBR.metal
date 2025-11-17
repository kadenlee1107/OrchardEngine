#include <metal_stdlib>
#include "Common.metal"

using namespace metal;

vertex VertexOutput pbr_vertex(
    VertexInput in [[stage_in]],
    constant CameraUniforms& camera [[buffer(0)]],
    constant ModelUniforms& model [[buffer(1)]]
) {
    VertexOutput out;
    
    float4 worldPos = model.modelMatrix * float4(in.position, 1.0);
    out.worldPosition = worldPos.xyz;
    out.position = camera.viewProjectionMatrix * worldPos;
    
    float3x3 normalMat = float3x3(model.normalMatrix[0].xyz,
                                   model.normalMatrix[1].xyz,
                                   model.normalMatrix[2].xyz);
    
    out.normal = normalize(normalMat * in.normal);
    out.tangent = normalize(normalMat * in.tangent);
    out.bitangent = cross(out.normal, out.tangent);
    
    out.texcoord = in.texcoord;
    
    return out;
}

fragment float4 pbr_fragment(
    VertexOutput in [[stage_in]],
    constant CameraUniforms& camera [[buffer(0)]],
    constant MaterialUniforms& material [[buffer(1)]],
    constant DirectionalLight& dirLight [[buffer(2)]],
    texture2d<float> albedoMap [[texture(0)]],
    texture2d<float> normalMap [[texture(1)]],
    texture2d<float> metallicRoughnessMap [[texture(2)]],
    texture2d<float> aoMap [[texture(3)]],
    sampler texSampler [[sampler(0)]]
) {
    float3 albedo = albedoMap.sample(texSampler, in.texcoord).rgb * material.albedo.rgb;
    float metallic = metallicRoughnessMap.sample(texSampler, in.texcoord).b * material.metallic;
    float roughness = metallicRoughnessMap.sample(texSampler, in.texcoord).g * material.roughness;
    float ao = aoMap.sample(texSampler, in.texcoord).r * material.ao;
    
    float3 N = normalize(in.normal);
    float3 V = normalize(camera.cameraPosition - in.worldPosition);
    
    float3 F0 = float3(0.04);
    F0 = mix(F0, albedo, metallic);
    
    float3 Lo = float3(0.0);
    
    {
        float3 L = normalize(-dirLight.direction);
        float3 H = normalize(V + L);
        
        float3 radiance = dirLight.color * dirLight.intensity;
        
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        float3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);
        
        float3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        float3 specular = numerator / denominator;
        
        float3 kS = F;
        float3 kD = float3(1.0) - kS;
        kD *= 1.0 - metallic;
        
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }
    
    float3 ambient = float3(0.03) * albedo * ao;
    float3 color = ambient + Lo;
    
    color = color / (color + float3(1.0));
    color = pow(color, float3(1.0/2.2));
    
    return float4(color, 1.0);
}
