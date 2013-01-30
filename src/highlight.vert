#version 330

//uniform samplerBuffer vertexTBO;
uniform mat4 u_ModelViewMatrix; 
uniform mat4 u_ProjectionMatrix;

uniform sampler2D u_heightTexture;

layout (location = 0) in vec4 a_Vertex;

const float heightMult = 0.1f;

void main () {
	vec4 vertex = a_Vertex;
	vec2 texCoord = ((vertex.xz/5.0f) * 0.5) + 0.5;
	float height = texture2D(u_heightTexture, texCoord).r * heightMult;

	vertex.y += height + 0.001;

	gl_Position = u_ProjectionMatrix * u_ModelViewMatrix * vertex;
}