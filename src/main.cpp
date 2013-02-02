/**
 * @file       main.cpp
 * @author     Daniel Princ
 * @date       2012/02/03 
 * @brief	   Handles openGL stuff, shaders
 *
 *  [PGR2] Vertex shader tesselation
 *
 * Controls:
 *   [wasd] - moving in the sscene
 *   [mouse left button] - rotate the scene
 *   [r] - toggle wire mode
 *   [h] - highligt original triangles
 *   [f] - freeze adaptive tesselation in current camera position
 *
 */


#define USE_ANTTWEAKBAR

#include "../../common/common.h"

#include <iostream>
#include <sstream>
#include <Windows.h>

using namespace glm;
#include "declarations.h"
#include "controls.h"

/**
 * Main display loop
 */
void cbDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, g_WireMode ? GL_LINE : GL_FILL);
	glDisable(GL_CULL_FACE);

	// Update camera transformation matrix
    updateCameraViewMatrix();

	//count FPS
	fpsCounter++;
	fps_time = GetTickCount() - fps_begin;
	if(fps_time > 1000) {
		g_fps = fpsCounter;
		fps_begin = GetTickCount();
		fpsCounter = 0;
	}

    // Turn on programmable pipeline
    if (g_UseShaders)
    {
        glUseProgram(g_tesselationProgramId);    // Active shader program

		g_subtriangles = g_tesselationFactor * g_tesselationFactor;
				
		//modelview and projection matrix
		glUniformMatrix4fv(glGetUniformLocation(g_tesselationProgramId, "u_ProjectionMatrix"), 1, GL_FALSE, &g_CameraProjectionMatrix[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(g_tesselationProgramId, "u_ModelViewMatrix"), 1, GL_FALSE, &g_CameraViewMatrix[0][0]);
		
		glUniform3fv(glGetUniformLocation(g_tesselationProgramId, "u_lightPos"), 1, &lightPos.x);
		glUniform1i(glGetUniformLocation(g_tesselationProgramId, "u_subtriangles"), g_subtriangles);
		glUniform1i(glGetUniformLocation(g_tesselationProgramId, "u_tessFactor"), g_tesselationFactor);
		glUniform1f(glGetUniformLocation(g_tesselationProgramId, "u_maxTessDistance"), g_maxTessDistance);
		glUniform1i(glGetUniformLocation(g_tesselationProgramId, "u_freeze"), g_freeze);
		glUniform3fv(glGetUniformLocation(g_tesselationProgramId, "u_freezePos"), 1, &g_freezePos.x);

		//GLuint hIndicesTex = glGetUniformLocation(g_tesselationProgramId, "u_indicesTBO");
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_BUFFER, indicesTBO);
		//glUniform1i(hIndicesTex, 0);
		
		//vertex data
		GLuint hVertTex = glGetUniformLocation(g_tesselationProgramId, "u_vertexTBO");
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_BUFFER, vertexTBO);
		glUniform1i(hVertTex, 1);

		//height map
		GLuint hHeightTex = glGetUniformLocation(g_tesselationProgramId, "u_heightTexture");
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, g_HeightMapTexId);
		glUniform1i(hHeightTex, 2);

		//diffuse texture
		GLuint hDifftTex = glGetUniformLocation(g_tesselationProgramId, "u_diffTexture");
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, g_DiffuseTexId);
		glUniform1i(hDifftTex, 3);

		//draw call with enough verticies (only gl_VertexID is used in shader)
		glBindBuffer(GL_ARRAY_BUFFER, emptyVBO);
		glDrawArrays(GL_TRIANGLES, 0, 3 * triangleCount * g_subtriangles);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		if(g_highlightOrig && g_WireMode) {
			//highlight original triangles
			glUseProgram(g_highlightProgramId);

			//nahrajeme modelview a projection matrix do shaderu
			glUniformMatrix4fv(glGetUniformLocation(g_tesselationProgramId, "u_ProjectionMatrix"), 1, GL_FALSE, &g_CameraProjectionMatrix[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(g_tesselationProgramId, "u_ModelViewMatrix"), 1, GL_FALSE, &g_CameraViewMatrix[0][0]);
			
			//height map
			hHeightTex = glGetUniformLocation(g_highlightProgramId, "u_heightTexture");
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, g_HeightMapTexId);
			glUniform1i(hHeightTex, 0);

			//draw vertices
			glBindBuffer(GL_ARRAY_BUFFER, originalTrianglesVBO);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);
			glDrawArrays(GL_TRIANGLES, 0, 3 * triangleCount);
			glDisableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			
		}
	}
	
    // Turn off programmable pipeline
    glUseProgram(NULL);

}

/**
 * openGL initialization
 */
void cbInitGL()
{
	//generate mesh
	triangles = genPlainMesh(10.0, 100, 50, &triangleCount);

	//max factor, that produces less vertices than max_vertices
	int max_tesselation_factor = sqrt((max_vertices / (float) triangleCount)); 

    // Init app GUI
    initGUI(max_tesselation_factor);

	glClearColor(0.3f, 0.3f, 0.7f, 0);
    glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	//buffer with original triangles
	glGenBuffers(1, &originalTrianglesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, originalTrianglesVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * triangleCount * 9, triangles, GL_STATIC_DRAW);
	
	//glGenBuffers(1, &indicesVBO);
	//glBindBuffer(GL_ARRAY_BUFFER, indicesVBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(int) * triangleCount * 3, indices, GL_STATIC_DRAW);
	assert(originalTrianglesVBO > 0);

	//texture buffer with vertex coordinates for random acces
	glGenTextures(1, &vertexTBO);
	glBindTexture(GL_TEXTURE_BUFFER, vertexTBO);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, originalTrianglesVBO);

	//glGenTextures(1, &indicesTBO);
	//glBindTexture(GL_TEXTURE_BUFFER, indicesTBO);
	//glTexBuffer(GL_TEXTURE_BUFFER, GL_R16I, indicesVBO);


	//empty vertex buffer, using only for vertexID
	glGenBuffers(1, &emptyVBO);
	//glBindBuffer(GL_ARRAY_BUFFER, emptyVBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(empty), empty, GL_STATIC_DRAW);

	assert((emptyVBO > 0) && (vertexTBO > 0));
	
    // Load heightmap texture from raw-file
    g_HeightMapTexId = prg2LoadRGBTextureFromRawFile(HEIGHTMAP_TEXTURE_FILE_NAME);

	//deifuuse texture
	g_DiffuseTexId = prg2LoadRGBTextureFromRawFile(DIFFUSE_TEXTURE_FILE_NAME);

	assert(g_HeightMapTexId > 0 && g_DiffuseTexId > 0);

    cbCompileShaderProgram(NULL);
}

/**
 * Shader compilation
 */
void TW_CALL cbCompileShaderProgram(void *clientData)
{
    // Delete shader program if exists
    if (g_tesselationProgramId)
    {
        glDeleteProgram(g_tesselationProgramId);
	}
    // Create shader program object
    g_tesselationProgramId = glCreateProgram();

    if (g_UseVertexShader)
    {
        // Create shader objects for vertex shader
        GLuint id = pgr2CreateShaderFromFile(GL_VERTEX_SHADER, VS_TESS_FILE_NAME);
        glAttachShader(g_tesselationProgramId, id);
        glDeleteShader(id);
    }
    if (g_UseGeometryShader)
    {
        // Create shader objects for geometry shader
        GLuint id = pgr2CreateShaderFromFile(GL_GEOMETRY_SHADER, GS_FILE_NAME);
        glAttachShader(g_tesselationProgramId, id);
        glDeleteShader(id);
    }
    if (g_UseFragmentShader)
    {
        // Create shader objects for fragment shader
        GLuint id = pgr2CreateShaderFromFile(GL_FRAGMENT_SHADER, FS_TESS_FILE_NAME);
        glAttachShader(g_tesselationProgramId, id);
        glDeleteShader(id);
    }

	
	if (g_highlightProgramId)
    {
        glDeleteProgram(g_highlightProgramId);
    }
	g_highlightProgramId = glCreateProgram();
	if (g_UseVertexShader)
    {
        // Create shader objects for vertex shader
        GLuint id = pgr2CreateShaderFromFile(GL_VERTEX_SHADER, VS_HIGHLIGHT_FILE_NAME);
        glAttachShader(g_highlightProgramId, id);
        glDeleteShader(id);
    }
    if (g_UseFragmentShader)
    {
        // Create shader objects for fragment shader
        GLuint id = pgr2CreateShaderFromFile(GL_FRAGMENT_SHADER, FS_HIGHLIGHT_FILE_NAME);
        glAttachShader(g_highlightProgramId, id);
        glDeleteShader(id);
    }

    // Link shader program
	glLinkProgram(g_tesselationProgramId);
    glLinkProgram(g_highlightProgramId);
    if (!pgr2CheckProgramLinkStatus(g_highlightProgramId) || !pgr2CheckProgramLinkStatus(g_tesselationProgramId))
    {
        pgr2CheckProgramInfoLog(g_tesselationProgramId);
		pgr2CheckProgramInfoLog(g_highlightProgramId);
        printf("Shader program creation failed.\n\n");
        glDeleteProgram(g_tesselationProgramId);
        glDeleteProgram(g_highlightProgramId);
        g_tesselationProgramId  = 0;
        g_highlightProgramId  = 0;
        g_UseShaders = false;
        return;
    }
    else
    {
        printf("Shader program compiled successfully.\n\n");
    }

}

/**
 * Callback for compile button
 */
void TW_CALL cbSetShaderStatus(const void *value, void *clientData)
{
    g_UseShaders = *(bool*)(value);
    // Try to compile shader program
    if (g_UseShaders)
    {
        cbCompileShaderProgram(NULL);
    }
}

/**
 * Callback for compile button
 */
void TW_CALL cbGetShaderStatus(void *value, void *clientData)
{
    *(bool*)(value) = g_UseShaders;
} 

/**
 * Generates plane mesh, no inices
 */
float* genPlainMesh(float size, int width, int height, int * count) {
	*count = width*height*2;

	float* vert = new float[width*height*3*6];

	const float xd = size / (float) width;
	const float yd = xd;

	const float startX = -size * 0.5f;
	const float startY = -size * 0.5f;

	float* ptr = vert;
	for(int x = 0; x < width; x++) {
		for(int y = 0; y < height; y++) {
			*ptr++ = startX + x * xd;
			*ptr++ = 0;
			*ptr++ = startY + y * yd;

			*ptr++ = startX + (x+1) * xd;
			*ptr++ = 0;
			*ptr++ = startY + (y+1) * yd;

			*ptr++ = startX + x * xd;
			*ptr++ = 0;
			*ptr++ = startY + (y+1) * yd;

			*ptr++ = startX + x * xd;
			*ptr++ = 0;
			*ptr++ = startY + y * yd;

			*ptr++ = startX + (x+1) * xd;
			*ptr++ = 0;
			*ptr++ = startY + y * yd;

			*ptr++ = startX + (x+1) * xd;
			*ptr++ = 0;
			*ptr++ = startY + (y+1) * yd;
		}
	}

	return vert;
}

void freeMemory() {
	//delete[] indices;
	delete[] triangles;
}

int main(int argc, char* argv[]) 
{
    int r = common_main(g_WindowWidth, g_WindowHeight,
                    "[PGR2] Vertex shader tesselation",
                    cbInitGL,              // init GL callback function
                    cbDisplay,             // display callback function
                    cbWindowSizeChanged,   // window resize callback function
                    cbKeyboardChanged,     // keyboard callback function
                    cbMouseButtonChanged,  // mouse button callback function
                    cbMousePositionChanged // mouse motion callback function
                    );
	freeMemory();
	return r;
}
