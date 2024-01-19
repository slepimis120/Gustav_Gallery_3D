#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNor; //Normale

out vec3 chFragPos; //Interpolirana pozicija fragmenta
out vec3 chNor; //Interpolirane normale

uniform mat4 uM;
uniform mat4 uV;
uniform mat4 uP;

void main()
{
	chFragPos = vec3(uM * vec4(inPos, 1.0));
	gl_Position = uP * uV * vec4(chFragPos, 1.0);
	chNor = mat3(transpose(inverse(uM))) * inNor; //Inverziju matrica bolje racunati na CPU
}