#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices=3) out;

uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ModelViewMatrix;

in block {
	vec4 v_Position;
	vec3 v_Normal;
	vec2 v_texCoord;
} In[];

out block {
	vec4 v_Position;
	vec3 v_Normal;
	vec2 v_texCoord;
} Out;

in int[] v_discard;

void main() {

	if(v_discard[0] == 0) {
		for (int i = 0; i < gl_in.length(); i++) {
			Out.v_texCoord = In[i].v_texCoord;
			Out.v_Normal = In[i].v_Normal; 
			Out.v_Position = In[i].v_Position;
			gl_Position = u_ProjectionMatrix *u_ModelViewMatrix *  In[i].v_Position;
			EmitVertex();
		}
	}
	EndPrimitive();

}