#include "Cubemap.h"
#include <stdexcept>

using namespace ogl;

static GLenum TextureFormatForBitmapFormat(Bitmap::Format format, bool srgb)
{
    switch (format) {
        case Bitmap::Format_Grayscale: return GL_LUMINANCE;
        case Bitmap::Format_GrayscaleAlpha: return GL_LUMINANCE_ALPHA;
        case Bitmap::Format_RGB: return (srgb ? GL_SRGB : GL_RGB);
        case Bitmap::Format_RGBA: return (srgb ? GL_SRGB_ALPHA : GL_RGBA);
        default: throw std::runtime_error("Unrecognised Bitmap::Format");
    }
}

Cubemap::Cubemap(const Bitmap& bk, const Bitmap& dn,
                 const Bitmap& ft, const Bitmap& lf,
                 const Bitmap& rt, const Bitmap& up, 
                 GLint minMagFiler, GLint wrapMode) :
    _originalWidth((GLfloat)bk.width()),
    _originalHeight((GLfloat)bk.height())
{
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &_object);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _object);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minMagFiler);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, minMagFiler);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrapMode);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrapMode);

    // Load pixel data
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, TextureFormatForBitmapFormat(rt.format(), true), 
                     (GLsizei)rt.width(), (GLsizei)rt.height(), 0, TextureFormatForBitmapFormat(rt.format(), false), GL_UNSIGNED_BYTE, rt.pixelBuffer());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0  , TextureFormatForBitmapFormat(lf.format(), true),
                     (GLsizei)lf.width(), (GLsizei)lf.height(), 0, TextureFormatForBitmapFormat(lf.format(), false), GL_UNSIGNED_BYTE, lf.pixelBuffer());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, TextureFormatForBitmapFormat(up.format(), true),
                     (GLsizei)up.width(), (GLsizei)up.height(), 0, TextureFormatForBitmapFormat(up.format(), false), GL_UNSIGNED_BYTE, up.pixelBuffer());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, TextureFormatForBitmapFormat(dn.format(), true),
                     (GLsizei)dn.width(), (GLsizei)dn.height(), 0, TextureFormatForBitmapFormat(dn.format(), false), GL_UNSIGNED_BYTE, dn.pixelBuffer());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, TextureFormatForBitmapFormat(ft.format(), true),
                     (GLsizei)ft.width(), (GLsizei)ft.height(), 0, TextureFormatForBitmapFormat(ft.format(), false), GL_UNSIGNED_BYTE, ft.pixelBuffer());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, TextureFormatForBitmapFormat(bk.format(), true),
                     (GLsizei)bk.width(), (GLsizei)bk.height(), 0, TextureFormatForBitmapFormat(bk.format(), false), GL_UNSIGNED_BYTE, bk.pixelBuffer());

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
