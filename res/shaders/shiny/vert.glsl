#version 330
 
in vec3 vertex;
in vec3 normal;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
out vec3 pos_eye;
out vec3 n_eye;
 
void main() {
	pos_eye = vec3 ( view * model * vec4 (vertex, 1.0));
	n_eye = vec3( view * model * vec4(normal, 0.0));
    gl_Position = projection * view * model * vec4(vertex, 1.0);
}