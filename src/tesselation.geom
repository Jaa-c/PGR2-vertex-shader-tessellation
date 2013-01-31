#version 330

layout (triangles) in;
layout (triangle_strip, max_vertices=3) out;
//layout (line_strip, max_vertices=2) out;

in block {
	vec4 v_Vertex;
	vec3 v_Normal;
	vec2 v_texCoord;
} In[];

out block {
	vec4 v_Vertex;
	vec3 v_Normal;
	vec2 v_texCoord;
} Out;

in int[] v_discard;


void main() {

	if(v_discard[0] == 0) {
		for (int i = 0; i < gl_in.length(); i++) {
			Out.v_texCoord = In[i].v_texCoord;
			Out.v_Normal = In[i].v_Normal; 
			Out.v_Vertex = In[i].v_Vertex;
			gl_Position = In[i].v_Vertex;
			EmitVertex();
		}
	}
	EndPrimitive();
	

	/* Vykreslení normál: * /
	for(int i = 0; i < gl_in.length(); i++){
		gl_Position = In[i].v_Position;
		Out.v_Normal = n;
		Out.v_viewPos = In[i].v_viewPos;
		Out.v_Color =  In[i].v_Color;
		EmitVertex();

		gl_Position = In[i].v_Position +  vec4(n , 0.0) * SCALE;
		Out.v_Normal = n;
		Out.v_viewPos = In[i].v_viewPos;
		Out.v_Color = vec4(1.0, 0.0, 0.0, 0.0);//In[i].v_Color;
		EmitVertex();

		EndPrimitive();
	}
	/**/


}