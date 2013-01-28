#version 330

uniform samplerBuffer vertexTBO;
uniform mat4 u_ModelViewMatrix; 
uniform mat4 u_ProjectionMatrix;

void main () {
	vec4 a_Vertex = texelFetch(vertexTBO, gl_VertexID);
	
	a_Vertex.z += 0.0001;

	gl_Position = u_ProjectionMatrix * u_ModelViewMatrix * a_Vertex;
}