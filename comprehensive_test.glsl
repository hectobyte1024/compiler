#version 330 core

// Shader: testVertex
in vec3 position;
in vec2 texCoord;
out vec2 vTexCoord;
void main() {
    vTexCoord = texCoord;
    gl_Position = vec4(position, 1.0);
}

// Shader: testFragment
in vec2 vTexCoord;
out vec4 fragColor;
uniform sampler2D diffuseTexture;
void main() {
vec3 color = texture(diffuseTexture, vTexCoord).rgb    ;
    fragColor = vec4(color, 1.0);
}

