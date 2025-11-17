#include "Texture.hpp"
#include <iostream>
#include <fstream>

namespace Orchard {

Texture::Texture() {
}

Texture::~Texture() {
#ifdef __APPLE__
    if (m_MetalTexture) {
        [m_MetalTexture release];
        m_MetalTexture = nil;
    }
#endif
}

bool Texture::LoadFromFile(const std::string& path) {
    std::cout << "Loading texture: " << path << std::endl;
    m_Path = path;
    
    return false;
}

bool Texture::LoadFromMemory(const void* data, uint32_t width, uint32_t height, TextureFormat format) {
    m_Width = width;
    m_Height = height;
    m_Format = format;
    
    size_t bytesPerPixel = 4;
    switch (format) {
        case TextureFormat::R8:
            bytesPerPixel = 1;
            break;
        case TextureFormat::RG8:
            bytesPerPixel = 2;
            break;
        case TextureFormat::RGBA8:
            bytesPerPixel = 4;
            break;
        case TextureFormat::RGBA16F:
            bytesPerPixel = 8;
            break;
        case TextureFormat::RGBA32F:
            bytesPerPixel = 16;
            break;
        default:
            break;
    }
    
    size_t dataSize = width * height * bytesPerPixel;
    m_Data.resize(dataSize);
    std::memcpy(m_Data.data(), data, dataSize);
    
#ifdef __APPLE__
#endif
    
    return true;
}

void Texture::GenerateMipmaps() {
#ifdef __APPLE__
#endif
}

std::shared_ptr<Texture> TextureImporter::ImportPNG(const std::string& path) {
    std::cout << "Importing PNG: " << path << std::endl;
    return nullptr;
}

std::shared_ptr<Texture> TextureImporter::ImportJPG(const std::string& path) {
    std::cout << "Importing JPG: " << path << std::endl;
    return nullptr;
}

std::shared_ptr<Texture> TextureImporter::ImportEXR(const std::string& path) {
    std::cout << "Importing EXR: " << path << std::endl;
    return nullptr;
}

std::shared_ptr<Texture> TextureImporter::ImportHDR(const std::string& path) {
    std::cout << "Importing HDR: " << path << std::endl;
    return nullptr;
}

}
