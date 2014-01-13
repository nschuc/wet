#version 330
 
in vec3 pos_eye;
in vec3 n_eye;
uniform samplerCube cubemap;
uniform mat4 view;
out vec4 fragColor;
 
void main (void) {
    vec3 incident_eye = normalize (pos_eye);
    vec3 normal = normalize (n_eye);

    vec3 reflected = reflect(incident_eye, normal);
    reflected = vec3(inverse(view) * vec4(reflected, 0.0));
    fragColor = texture(cubemap, reflected);
}