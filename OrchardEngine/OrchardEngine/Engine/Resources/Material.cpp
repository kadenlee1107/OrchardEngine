#include "Material.hpp"
#include "Texture.hpp"

namespace Orchard {

Material::Material() {
}

Material::~Material() {
}

void Material::SetTexture(const std::string& name, std::shared_ptr<Texture> texture) {
    m_Textures[name] = texture;
}

std::shared_ptr<Texture> Material::GetTexture(const std::string& name) const {
    auto it = m_Textures.find(name);
    if (it != m_Textures.end()) {
        return it->second;
    }
    return nullptr;
}

void Material::SetFloat(const std::string& name, float value) {
    m_Floats[name] = value;
}

void Material::SetVector3(const std::string& name, const Math::Vector3& value) {
    m_Vector3s[name] = value;
}

void Material::SetVector4(const std::string& name, const Math::Vector4& value) {
    m_Vector4s[name] = value;
}

void Material::SetColor(const std::string& name, const Math::Vector4& color) {
    SetVector4(name, color);
}

float Material::GetFloat(const std::string& name, float defaultValue) const {
    auto it = m_Floats.find(name);
    if (it != m_Floats.end()) {
        return it->second;
    }
    return defaultValue;
}

Math::Vector3 Material::GetVector3(const std::string& name, const Math::Vector3& defaultValue) const {
    auto it = m_Vector3s.find(name);
    if (it != m_Vector3s.end()) {
        return it->second;
    }
    return defaultValue;
}

Math::Vector4 Material::GetVector4(const std::string& name, const Math::Vector4& defaultValue) const {
    auto it = m_Vector4s.find(name);
    if (it != m_Vector4s.end()) {
        return it->second;
    }
    return defaultValue;
}

}
