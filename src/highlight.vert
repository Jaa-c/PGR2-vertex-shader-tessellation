#version 330

//uniform samplerBuffer vertexTBO;
uniform mat4 u_ModelViewMatrix; 
uniform mat4 u_ProjectionMatrix;

layout (location = 0) in vec4 a_Vertex;

void main () {	
	vec4 vertex = a_Vertex;
	vertex.z += 0.001;
	gl_Position = u_ProjectionMatrix * u_ModelViewMatrix * vertex;
}