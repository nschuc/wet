#version 330
 
in vec3 vertex;
in vec3 normal;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
out vec3 reflected;
 
void main() {
	vec3 pos_eye = normalize(vec3 ( view * model * vec4 (vertex, 1.0)));
	vec3 n_eye = normalize(vec3( view * model * vec4(normal, 0.0)));

    reflected = vec3(inverse(view) * vec4(reflect(pos_eye, n_eye), 0.0));

    gl_Position = projection * view * model * vec4(vertex, 1.0);
}