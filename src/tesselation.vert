#version 330

uniform mat4 u_ModelViewMatrix; 
uniform mat4 u_ProjectionMatrix;
uniform int u_subtriangles;
uniform int u_tessFactor;

uniform samplerBuffer vertexTBO;

out vec3 v_Vertex;
out vec3 v_Normal;

vec4 a_Vertex;

void main () {

	int subTriID = (gl_VertexID / 3) % u_subtriangles;

	//barycentric coords u, v , w
	float fRow = floor(sqrt(subTriID));
	float incuv = 1.0f / u_tessFactor;
	float u = (1.0f + fRow) / u_tessFactor;
	
	float fCol = subTriID - (fRow * fRow);
	int uCol = int(fCol);
	float v = incuv * floor(fCol * .5f);

	u-= v;

	float w = 1.0f - u - v;

	//vertex index in current mesh
	int vertexID = ((gl_VertexID / 3) / u_subtriangles) * 3 + (gl_VertexID % 3);
	

	int index = (vertexID / 3) * 3;
	//original triangle verticies	
	vec4 v1 = texelFetch(vertexTBO, index);
	vec4 v2 = texelFetch(vertexTBO, index+1);
	vec4 v3 = texelFetch(vertexTBO, index+2);


	switch(vertexID % 3) {
		case 0:
			if((uCol & 1) != 0) {
				v += incuv;
				u -= incuv;
			}
			break;
		case 1:
			if((uCol & 1) == 0) {
				v += incuv;
				u -= incuv;
			}
			else {
				v += incuv;
				u -= incuv;
				w += incuv;
				u -= incuv;		
			}
			break;
		case 2:
			if((uCol & 1) == 0) {
				w += incuv;
				u -= incuv;
			}
			else {
				w += incuv;
				u -= incuv;		
			}
			break;
	}

	a_Vertex = u * v1 + v * v2 + w * v3;
	
	vec4 viewPos = u_ModelViewMatrix * a_Vertex;
	v_Normal = normalize(mat3(u_ModelViewMatrix) * vec3(0, 0, 1.0));
	v_Vertex = viewPos.xyz;
	
	gl_Position = u_ProjectionMatrix * viewPos;
}