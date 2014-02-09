#version 330

in vec3 vertex;
in vec3 normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 light_position;

out vec3 light_vector;
out vec3 normal_vector;
out vec3 halfway_vector;
out vec3 reflected;
out vec2 tex_coord;

void main() {

	vec3 pos_eye = normalize(vec3 ( view * model * vec4 (vertex, 1.0)));
	vec3 n_eye = normalize(vec3( view * model * vec4(normal, 0.0)));

    reflected = vec3(inverse(view) * vec4(reflect(pos_eye, n_eye), 0.0));


	gl_Position = projection * view * model * vec4(vertex, 1.0);

	vec4 v = view * model * vec4(vertex, 1.0);
	vec3 normal1 = normalize(normal);

	light_vector = normalize((view * vec4(light_position, 1.0)).xyz - v.xyz);
	normal_vector = (inverse(transpose(view * model)) * vec4(normal1, 0.0)).xyz;
        halfway_vector = light_vector + normalize(-v.xyz);

}