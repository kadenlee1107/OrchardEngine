#pragma once

#include "../Math/Vector.hpp"
#include "../Utils/UUID.hpp"
#include "../Core/ResourceManager.hpp"
#include <string>
#include <unordered_map>

namespace Orchard {

class Texture;
class Shader;

class Material : public Resource {
public:
    Material();
    ~Material();
    
    void SetShader(std::shared_ptr<Shader> shader) { m_Shader = shader; }
    std::shared_ptr<Shader> GetShader() const { return m_Shader; }
    
    void SetTexture(const std::string& name, std::shared_ptr<Texture> texture);
    std::shared_ptr<Texture> GetTexture(const std::string& name) const;
    
    void SetFloat(const std::string& name, float value);
    void SetVector3(const std::string& name, const Math::Vector3& value);
    void SetVector4(const std::string& name, const Math::Vector4& value);
    void SetColor(const std::string& name, const Math::Vector4& color);
    
    float GetFloat(const std::string& name, float defaultValue = 0.0f) const;
    Math::Vector3 GetVector3(const std::string& name, const Math::Vector3& defaultValue = Math::Vector3()) const;
    Math::Vector4 GetVector4(const std::string& name, const Math::Vector4& defaultValue = Math::Vector4()) const;
    
    void SetAlbedo(const Math::Vector4& albedo) { m_Albedo = albedo; }
    const Math::Vector4& GetAlbedo() const { return m_Albedo; }
    
    void SetMetallic(float metallic) { m_Metallic = metallic; }
    float GetMetallic() const { return m_Metallic; }
    
    void SetRoughness(float roughness) { m_Roughness = roughness; }
    float GetRoughness() const { return m_Roughness; }
    
    void SetAO(float ao) { m_AO = ao; }
    float GetAO() const { return m_AO; }
    
private:
    std::shared_ptr<Shader> m_Shader;
    
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_Textures;
    std::unordered_map<std::string, float> m_Floats;
    std::unordered_map<std::string, Math::Vector3> m_Vector3s;
    std::unordered_map<std::string, Math::Vector4> m_Vector4s;
    
    Math::Vector4 m_Albedo{1, 1, 1, 1};
    float m_Metallic = 0.0f;
    float m_Roughness = 0.5f;
    float m_AO = 1.0f;
};

}
