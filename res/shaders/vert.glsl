#version 330

uniform mat4 camera;
uniform mat4 model;

in vec3 vertex;
in vec2 vertTexCoord;
in vec3 vertNormal;

out vec3 fragVert;
out vec2 fragTexCoord;
out vec3 fragNormal;

void main() {
	fragTexCoord = vertTexCoord;
	fragNormal = vertNormal;
	fragVert = vertex;
	
    gl_Position = camera * model * vec4(vertex, 1);
}
