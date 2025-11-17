#pragma once

#include "../Utils/UUID.hpp"
#include "../Core/ResourceManager.hpp"
#include <string>
#include <vector>

#ifdef __APPLE__
#import <Metal/Metal.h>
#endif

namespace Orchard {

enum class TextureFormat {
    R8,
    RG8,
    RGBA8,
    R16F,
    RG16F,
    RGBA16F,
    R32F,
    RG32F,
    RGBA32F,
    Depth24Stencil8,
    Depth32F
};

enum class TextureFilter {
    Nearest,
    Linear,
    Trilinear
};

enum class TextureWrap {
    Repeat,
    Clamp,
    Mirror
};

class Texture : public Resource {
public:
    Texture();
    ~Texture();
    
    bool LoadFromFile(const std::string& path);
    bool LoadFromMemory(const void* data, uint32_t width, uint32_t height, TextureFormat format);
    
    uint32_t GetWidth() const { return m_Width; }
    uint32_t GetHeight() const { return m_Height; }
    TextureFormat GetFormat() const { return m_Format; }
    
    void SetFilter(TextureFilter filter) { m_Filter = filter; }
    TextureFilter GetFilter() const { return m_Filter; }
    
    void SetWrap(TextureWrap wrap) { m_Wrap = wrap; }
    TextureWrap GetWrap() const { return m_Wrap; }
    
    void GenerateMipmaps();
    
#ifdef __APPLE__
    id<MTLTexture> GetMetalTexture() const { return m_MetalTexture; }
#endif
    
private:
    uint32_t m_Width = 0;
    uint32_t m_Height = 0;
    TextureFormat m_Format = TextureFormat::RGBA8;
    TextureFilter m_Filter = TextureFilter::Linear;
    TextureWrap m_Wrap = TextureWrap::Repeat;
    
    std::vector<uint8_t> m_Data;
    
#ifdef __APPLE__
    id<MTLTexture> m_MetalTexture = nil;
#endif
};

class TextureImporter {
public:
    static std::shared_ptr<Texture> ImportPNG(const std::string& path);
    static std::shared_ptr<Texture> ImportJPG(const std::string& path);
    static std::shared_ptr<Texture> ImportEXR(const std::string& path);
    static std::shared_ptr<Texture> ImportHDR(const std::string& path);
};

}
