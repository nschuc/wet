#version 330
 
in vec3 reflected;
uniform samplerCube cubemap;
uniform mat4 view;
out vec4 fragColor;
 
void main (void) {
    fragColor = texture(cubemap, reflected);
}