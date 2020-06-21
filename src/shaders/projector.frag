#version 150

in vec3 worldPosition;

out vec4 fragColor;

void main() {
    fragColor = vec4(abs(worldPosition), 1);
}