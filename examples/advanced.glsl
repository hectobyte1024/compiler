#version 330 core

// Shader: advancedVertex
in vec3 position;
in vec3 normal;
in vec2 texCoord;
out vec3 worldPos;
out vec3 worldNormal;
out vec2 vTexCoord;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat3 normalMatrix;
vec3 transformPosition(vec3 pos) {
}

vec3 transformNormal(vec3 norm) {
}

void main() {
    worldPos = transformPosition(position);
    worldNormal = transformNormal(normal);
    vTexCoord = texCoord;
    gl_Position = (projectionMatrix * viewPos);
}

// Shader: imageBlur
uniform sampler2D inputImage;
uniform vec2 imageSize;
uniform float blurRadius;
vec4 sampleBlur(vec2 coord) {
float totalWeight = 0.0    ;
    (y <= radius);
vec2 sampleCoord = (coord + offset)    ;
}

vec4 blurredColor = sampleBlur(coord);
