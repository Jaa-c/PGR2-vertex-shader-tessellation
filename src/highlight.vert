#version 330 core

uniform mat4 u_ModelViewMatrix; 
uniform mat4 u_ProjectionMatrix;

uniform sampler2D u_heightTexture;

layout (location = 0) in vec4 a_Vertex;

const float heightMult = 0.1f;		//height correction in height map

/**
 * This shader only draws the original triangles above the current grid
 * and draws them red (fragment shader)
 */
void main () {
	vec4 vertex = a_Vertex;
	vec2 texCoord = ((vertex.xz/5.0f) * 0.5) + 0.5;
	texCoord.y += 0.25;

	float height = texture2D(u_heightTexture, texCoord).r * heightMult;

	vertex.y += height + 0.001;

	gl_Position = u_ProjectionMatrix * u_ModelViewMatrix * vertex;
}
