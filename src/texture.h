#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "image.h"

CLASS_PTR(Texture)
class Texture{
public:
    static TextureUPtr Create(int width, int height, uint32_t format);
    // just pointer, not unique one.
    // -- why? there is no reason to handle SoYouKun
    static TextureUPtr CreateFromImage(const Image* image);
    ~Texture();

    int GetWidth() const { return m_width;}
    int GetHeight() const { return m_height;}
    uint32_t GetFormat() const { return m_format;}

    const uint32_t Get() const {return m_texture; }
    void Bind() const;
    void SetFilter(uint32_t minFilter, uint32_t magFilter) const;
    void SetWrap(uint32_t sWrap, uint32_t tWrap) const;

private:
    Texture() {}
    void CreateTexture();
    void SetTextureFromImage(const Image* image);
    void SetTextureFormat(int width, int height, uint32_t format);
    uint32_t m_texture {0};
    int m_width {0};
    int m_height {0};
    uint32_t m_format {GL_RGBA};
};


#endif // __TEXTURE_H__