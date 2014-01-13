#ifndef OCEAN_H
#define OCEAN_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../ogl/Program.h"
#include "Complex.h"
#include "vector.h"
#include "fft.h"


struct vertex_ocean {
    GLfloat   x,   y,   z; // vertex
    GLfloat  nx,  ny,  nz; // normal
    GLfloat   a,   b,   c; // htilde0
    GLfloat  _a,  _b,  _c; // htilde0mk conjugate
    GLfloat  ox,  oy,  oz; // original position
};




struct complex_vector_normal {  // structure used with discrete fourier transform
    complex h;      // wave height
    vector2 D;      // displacement
    vector3 n;      // normal
};




class Ocean {
  private:
    float g;                // gravity constant
    bool geometry;              // flag to render geometry or surface
    int N, Nplus1;              // dimension -- N should be a power of 2
    float A;                // phillips spectrum parameter -- affects heights of waves
    vector2 w;              // wind parameter
    float length;               // length parameter
    unsigned int *indices;          // indicies for vertex buffer object
    unsigned int indices_count;     // number of indices to render
    vertex_ocean *vertices;         // vertices for vertex buffer object
    GLuint vbo_vertices, vbo_indices, vao;   // vertex buffer objects

    complex *h_tilde,           // for fast fourier transform
        *h_tilde_slopex, *h_tilde_slopez,
        *h_tilde_dx, *h_tilde_dz;
    cFFT *fft;              // fast fourier transform


    GLint light_position, projection, view, model; // attributes and uniforms

  protected:
  public:
    Ocean(const int N, const float A, const vector2 w, const float length, bool geometry);
    ~Ocean();
    void release();

    float dispersion(int n_prime, int m_prime);     // deep water
    float phillips(int n_prime, int m_prime);       // phillips spectrum
    complex hTilde_0(int n_prime, int m_prime);
    complex hTilde(float t, int n_prime, int m_prime);
    complex_vector_normal h_D_and_n(vector2 x, float t);
    void evaluateWaves(float t);
    void enableAttribs(GLint vertex, GLint normal);
    void evaluateWavesFFT(float t);
    void render(ogl::Program* shader);
};


#endif