#version 330 core

uniform vec3 currentColor = vec3(1.0, 0.0, 0.0);

out vec4 fragmentColor;

void main()
{
	fragmentColor = vec4(currentColor, 1.0);
}
