#pragma once

#include <GL/glew.h>
#include "Bitmap.h"
#include "Texture.h"

namespace ogl {
    
    /**
     Represents an OpenGL texture
     */
    class Cubemap : public Texture {
    public:
        /**
         Creates a cubemap from six bitmaps
         
         The texture will be loaded upside down because tdogl::Bitmap pixel data
         is ordered from the top row down, but OpenGL expects the data to
         be from the bottom row up.
         
         @param bitmap  The bitmap to load the texture from
         @param minMagFiler  GL_NEAREST or GL_LINEAR
         @param wrapMode GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, or GL_CLAMP_TO_BORDER
         */
        Cubemap(const Bitmap& bk, const Bitmap& dn,
                const Bitmap& ft, const Bitmap& lf,
                const Bitmap& rt, const Bitmap& up,
                GLint minMagFiler = GL_LINEAR,
                GLint wrapMode = GL_CLAMP_TO_EDGE);

        GLuint _object;
        GLfloat _originalWidth;
        GLfloat _originalHeight;
        
        //copying disabled
        Cubemap(const Cubemap&);
        const Cubemap& operator=(const Texture&);
    };
    
}