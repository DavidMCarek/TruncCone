#version 150
uniform mat4 transform;

in vec3 vPosition;
in vec3 vColor;
out vec4 passedColor;

void main() {
	gl_Position = transform * vec4(vPosition, 1.0);
	passedColor = vec4(vColor, 1.0);
}