#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aPosTarget;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aNormalTarget;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float morphFactor;

out vec3 FragPos;
out vec3 Normal;

void main() {
    vec3 morphedPos = mix(aPos, aPosTarget, morphFactor);

    vec3 morphedNormal = aNormal;
    if (length(aNormalTarget) > 0.001) {
        morphedNormal = mix(aNormal, aNormalTarget, morphFactor);
    }

    FragPos = vec3(model * vec4(morphedPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * morphedNormal;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}