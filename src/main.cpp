#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// standard C++ libraries
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <list>

// ogl classes
#include "ogl/Program.h"
#include "ogl/Texture.h"
#include "ogl/Cubemap.h"
#include "ogl/Camera.h"
#include "ogl/ObjLoader.h"
#include "Helper.h"

// Entities
#include "entities/Ocean.h"

using namespace std;

struct Light {
    glm::vec3 position;
    glm::vec3 intensities;
    float attenuation;
    float ambientCoefficient;
};

struct ModelAsset {
    ogl::Program* shaders;
    ogl::Texture* texture;
    GLuint vbo;
    GLuint vao;
    GLenum drawType;
    GLint drawStart;
    GLint drawCount;
    GLfloat shininess;
    glm::vec3 specularColor;

    ModelAsset() :
        shaders(NULL),
        texture(NULL),
        vbo(0),
        vao(0),
        drawType(GL_TRIANGLES),
        drawStart(0),
        drawCount(0)
        {}
};

struct ModelInstance {
    ModelAsset* asset;
    glm::mat4 transform;

    ModelInstance() :
        asset(NULL),
        transform()
        {}
};

// constants
const glm::vec2 SCREEN_SIZE(1024, 768);

// globals
Light gLight;
ogl::Camera gCamera;
ModelAsset gWoodenCrate, gSkybox, gDragon;
std::list<ModelInstance> gInstances;
GLuint cubemap;
ogl::cObj* dragon;

Ocean* ocean;
ogl::Program* oceanShader;

double elapsed = 0;

// returns the full path to the file `fileName` in the same folder as the executable
static std::string ResourcePath(std::string fileName) {
        return GetProcessPath() + "/../" + fileName;
}

static ogl::Cubemap* loadCubemap(std::string dir, std::string extension) {
    ogl::Bitmap bk = ogl::Bitmap::bitmapFromFile(ResourcePath(dir + "bk." + extension));
    bk.flipVertically();
    ogl::Bitmap dn = ogl::Bitmap::bitmapFromFile(ResourcePath(dir + "dn." + extension));
    dn.flipVertically();
    dn.rotate90CounterClockwise();
    dn.rotate90CounterClockwise();
    dn.rotate90CounterClockwise();
    ogl::Bitmap ft = ogl::Bitmap::bitmapFromFile(ResourcePath(dir + "ft." + extension));
    ft.flipVertically();
    ogl::Bitmap lf = ogl::Bitmap::bitmapFromFile(ResourcePath(dir + "lf." + extension));
    lf.flipVertically();
    ogl::Bitmap rt = ogl::Bitmap::bitmapFromFile(ResourcePath(dir + "rt." + extension));
    rt.flipVertically();
    ogl::Bitmap up = ogl::Bitmap::bitmapFromFile(ResourcePath(dir + "up." + extension));
    up.flipVertically();
    up.rotate90CounterClockwise();

    glGenTextures(1, &cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Load pixel data
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_SRGB, 
                     (GLsizei)rt.width(), (GLsizei)rt.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, rt.pixelBuffer());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_SRGB,
                     (GLsizei)lf.width(), (GLsizei)lf.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, lf.pixelBuffer());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_SRGB,
                     (GLsizei)up.width(), (GLsizei)up.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, up.pixelBuffer());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_SRGB,
                     (GLsizei)dn.width(), (GLsizei)dn.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, dn.pixelBuffer());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_SRGB,
                     (GLsizei)ft.width(), (GLsizei)ft.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, ft.pixelBuffer());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_SRGB,
                     (GLsizei)bk.width(), (GLsizei)bk.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, bk.pixelBuffer());
    return  NULL; //new ogl::Cubemap(bk,dn,ft,lf,rt,up);
}

static ogl::Texture* LoadTexture(const char* filename) {
    ogl::Bitmap bmp = ogl::Bitmap::bitmapFromFile(ResourcePath(filename));
    bmp.flipVertically();
    return new ogl::Texture(bmp);
}

// loads the vertex shader and fragment shader, and links them to make the global gProgram
static ogl::Program* LoadShaders(const char* vertFilename, const char* fragFilename) {
    std::vector<ogl::Shader> shaders;
    shaders.push_back(ogl::Shader::shaderFromFile(ResourcePath(vertFilename), GL_VERTEX_SHADER));
    shaders.push_back(ogl::Shader::shaderFromFile(ResourcePath(fragFilename), GL_FRAGMENT_SHADER));
    return new ogl::Program(shaders);
}

// convenience function that returns a translation matrix
glm::mat4 translate(GLfloat x, GLfloat y, GLfloat z) {
    return glm::translate(glm::mat4(), glm::vec3(x,y,z));
}

// convenience function that returns a scaling matrix
glm::mat4 scale(GLfloat x, GLfloat y, GLfloat z) {
    return glm::scale(glm::mat4(), glm::vec3(x,y,z));
}

//create all the `instance` structs for the 3D scene, and add them to `gInstances`
static void CreateInstances() {
    ModelInstance dot;
    dot.asset = &gWoodenCrate;
    dot.transform = glm::mat4();
    gInstances.push_back(dot);

    ModelInstance i;
    i.asset = &gWoodenCrate;
    i.transform = translate(0,-4,0) * scale(1,2,1);
    gInstances.push_back(i);

    ModelInstance hLeft;
    hLeft.asset = &gWoodenCrate;
    hLeft.transform = translate(-8,0,0) * scale(1,6,1);
    gInstances.push_back(hLeft);

    ModelInstance hRight;
    hRight.asset = &gWoodenCrate;
    hRight.transform = translate(-4,0,0) * scale(1,6,1);
    gInstances.push_back(hRight);

    ModelInstance hMid;
    hMid.asset = &gWoodenCrate;
    hMid.transform = translate(-6,0,0) * scale(2,1,0.8);
    gInstances.push_back(hMid);
}

static void RenderInstance(const ModelInstance& inst) {
    ModelAsset* asset = inst.asset;
    ogl::Program* shaders = asset->shaders;

    //bind the shaders
    shaders->use();

    shaders->setUniform("model", inst.transform);
    shaders->setUniform("view", gCamera.view());
    shaders->setUniform("projection", gCamera.projection());
    shaders->setUniform("model", glm::mat4());


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);


    //bind VAO and draw
    glBindVertexArray(asset->vao);
    glDrawArrays(asset->drawType, asset->drawStart, asset->drawCount);

    //unbind everything
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    shaders->stopUsing();
}

// draws a single frame
static void render() {
    // clear everything
    glClearColor(0, 0, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    std::list<ModelInstance>::const_iterator it;
    for(it = gInstances.begin(); it != gInstances.end(); ++it){
        RenderInstance(*it);
    }
    
    // swap the display buffers (displays what was just drawn)
    glfwSwapBuffers();
}

static void update(float dt) {
    const GLfloat moveSpeed = 20.0f;

    elapsed += dt / 5.f;

    ocean->evaluateWavesFFT(elapsed);

    gLight.position = gCamera.position();

    if(glfwGetKey('S')){
        gCamera.offsetPosition(dt * moveSpeed * -gCamera.forward());
    } else if(glfwGetKey('W')){
        gCamera.offsetPosition(dt * moveSpeed * gCamera.forward());
    }

    if(glfwGetKey('A')){
        gCamera.offsetPosition(dt * moveSpeed * -gCamera.right());
    } else if(glfwGetKey('D')){
        gCamera.offsetPosition(dt * moveSpeed * gCamera.right());
    }

    if(glfwGetKey('Z')){
        gCamera.offsetPosition(dt * moveSpeed * -glm::vec3(0,1,0));
    } else if(glfwGetKey('X')){
        gCamera.offsetPosition(dt * moveSpeed * glm::vec3(0,1,0));
    }
}


// the program starts here
int main(int argc, char *argv[]) {
    // initialise GLFW
    if(!glfwInit())
        throw std::runtime_error("glfwInit failed");
    
    // // open a window with GLFW
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
    glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
    if(!glfwOpenWindow(SCREEN_SIZE.x, SCREEN_SIZE.y, 8, 8, 8, 8, 16, 0, GLFW_WINDOW))
        throw std::runtime_error("glfwOpenWindow failed. Can your hardware handle OpenGL 3.2?");
        
    glfwDisable(GLFW_MOUSE_CURSOR);
    glfwSetMousePos(0,0);

    // initialise GLEW
    glewExperimental = GL_TRUE; //stops glew crashing on OSX :-/
    if(glewInit() != GLEW_OK)
        throw std::runtime_error("glewInit failed");
    glGetError();
    
    // // make sure OpenGL version 3.2 API is available
    if(!GLEW_VERSION_3_2)
        throw std::runtime_error("OpenGL 3.2 API is not available.");


    CreateInstances();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Setup lights
    gLight.position = gCamera.position();
    gLight.intensities = glm::vec3(1, 1, 1);
    gLight.attenuation = 0.2f;
    gLight.ambientCoefficient = 0.05f;

    // Setup Camera
    gCamera.setPosition(glm::vec3(0,0,4));
    gCamera.setViewportAspectRatio(SCREEN_SIZE.x / SCREEN_SIZE.y);
    gCamera.setNearAndFar(0.05f, 1000.0f);

    
    // run while the window is open
    double endTime = glfwGetTime();
    while(glfwGetWindowParam(GLFW_OPENED)){
        double startTime = glfwGetTime();
        // draw one frame
        update(startTime - endTime);
        endTime = startTime;

        //rotate camera based on mouse movement
        const float mouseSensitivity = 0.1;
        int mouseX, mouseY;
        glfwGetMousePos(&mouseX, &mouseY);
        gCamera.offsetOrientation(mouseSensitivity * mouseY, mouseSensitivity * mouseX);
        glfwSetMousePos(0, 0); //reset the mouse, so it doesn't go out of the window

        render();

        GLenum error = glGetError();
        if(error != GL_NO_ERROR)
            std::cerr << "OpenGL Error " << error << ": " << (const char*)gluErrorString(error) << std::endl;

        if(glfwGetKey(GLFW_KEY_ESC))
            glfwCloseWindow();
    }
    
    // clean up and exit
    // glfwTerminate();
    return EXIT_SUCCESS;
}
