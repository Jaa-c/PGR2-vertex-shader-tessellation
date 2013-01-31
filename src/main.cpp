//-----------------------------------------------------------------------------
//  [PGR2] Simple GLSL Example
//  27/02/2008
//-----------------------------------------------------------------------------
//  Controls: 
//    [mouse-left-button]  ... scene rotation
//    [t], [T]             ... move scene forward/backward
//    [r]                  ... toggle scene rotation
//    [v]                  ... toggle vertex shader
//    [g]                  ... toggle geometry shader
//    [f]                  ... toggle fragment shader
//    [s]                  ... toggle programmable pipeline (shaders)
//    [b]                  ... build shader programs
//    [w]                  ... toggle wire mode
//    [c]                  ... toggle face culling
//    [space]              ... change model type
//-----------------------------------------------------------------------------
#define USE_ANTTWEAKBAR

#include "../../common/common.h"
#include "../../common/models/elephant.h"
#include "../../common/models/cube.h"

#include <iostream>
#include <Windows.h>

using namespace glm;

// GLOBAL CONSTANTS____________________________________________________________
//const char*	  HEIGHTMAP_TEXTURE_FILE_NAME = "data/test_1024.raw";
//const char*	  DIFFUSE_TEXTURE_FILE_NAME = "data/test_1024_diffuse.raw";
const char*	  HEIGHTMAP_TEXTURE_FILE_NAME = "data/world_height_8192.raw";
const char*	  DIFFUSE_TEXTURE_FILE_NAME = "data/world_diffuse_8192.raw";

const char* VS_TESS_FILE_NAME			= "src/tesselation.vert";
const char* VS_HIGHLIGHT_FILE_NAME		= "src/highlight.vert";

const char* GS_FILE_NAME				= "src/simple.geom";

const char* FS_TESS_FILE_NAME			= "src/tesselation.frag";
const char* FS_HIGHLIGHT_FILE_NAME		= "src/highlight.frag";


const float STEP = 0.5f;
// GLOBAL VARIABLES____________________________________________________________
GLint    g_WindowWidth       = 800;    // Window width
GLint    g_WindowHeight      = 600;    // Window height

bool     g_WireMode          = true;  // Wire mode enabled/disabled

bool     g_UseShaders        = true;  // Programmable pipeline on/off
bool     g_UseVertexShader   = true;  // Use vertex shader
bool     g_UseGeometryShader = false;  // Use geometry shader
bool     g_UseFragmentShader = true;  // Use fragment shader

bool	g_highlightOrig		 = false; //highlight original triangles

bool	g_freeze			 = false; //freeze tesselation in current location


float	 g_fps					= 0;

float	g_maxTessDistance	 = 4.0f;

GLint g_tesselationFactor = 10;


// Transformation matrixes
mat4 g_CameraProjectionMatrix;             // Camera projection transformation
mat4 g_CameraViewMatrix;                   // Camera view transformation


GLuint emptyVBO = 0;
GLuint originalTrianglesVBO = 0;
GLuint originalTrianglesIBO = 0;
GLuint vertexTBO = 0;
GLuint textureBO = 0;

GLint g_subtriangles;


GLuint   g_HeightMapTexId    = 0;       // Height texture
GLsizei  g_NumHeightMapTexels   = 0;    // Number of heigtmap texels

GLuint g_DiffuseTexId = 0;

// GLSL variables
GLuint g_tesselationProgramId = 0;                 // Shader program id
GLuint g_highlightProgramId = 0;				   // Shader program id



///moving and stuff
vec3 cameraPos(0.0f, -1.0f, -4.0f);
vec3 cameraRot(0.0f, 0.0f, 0.0f);
vec3 cameraPosLag(cameraPos);
vec3 cameraRotLag(cameraRot);


vec3	g_freezePos(cameraPos);
// view params
int ox, oy;

const float inertia = 0.1f;//.1
const float rotateSpeed = 0.5f;
const float walkSpeed = 0.02f;//.05, .1


// FORWARD DECLARATIONS________________________________________________________
#ifdef USE_ANTTWEAKBAR
    void TW_CALL cbSetShaderStatus(const void*, void*);
    void TW_CALL cbGetShaderStatus(void*, void*);
#endif
void TW_CALL cbCompileShaderProgram(void *clientData);
void initGUI();



//-----------------------------------------------------------------------------
// Name: updateCameraViewMatrix()
// Desc: Use OpenGL to compute camera's projection and world-to-camera space 
//       transformation matrixes
//-----------------------------------------------------------------------------
void updateCameraViewMatrix()
{
	// move camera
    if (cameraPos[1] > -0.1f)
    {
        cameraPos[1] = -0.1f;
    }

    cameraPosLag += (cameraPos - cameraPosLag) * inertia;
    cameraRotLag += (cameraRot - cameraRotLag) * inertia;

    // view transform
	g_CameraViewMatrix = rotate(mat4(1.0f), cameraRotLag[0], vec3(1.0, 0.0, 0.0));
	g_CameraViewMatrix = rotate(g_CameraViewMatrix, cameraRotLag[1], vec3(0.0, 1.0, 0.0));
	g_CameraViewMatrix = translate(g_CameraViewMatrix, cameraPosLag);

}



//plain mesh, no inices
float* genPlainMesh(float size, int width, int height, int * count) {
	
	*count = width*height*2;
	float* vert = new float[width*height*3*6];
	
	const float xd = size / (float) width;
	const float yd = size / (float) height;
	
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

int triangleCount = 0;
float * triangles = genPlainMesh(10.0, 100, 100, &triangleCount);

/* Additional variables storing timestamps for time measuring */
long fps_begin, fps_time;
/* Additional variables for object counting */
int fpsCounter;

float empty[] = {0};
//-----------------------------------------------------------------------------
// Name: cbDisplay()
// Desc: 
//-----------------------------------------------------------------------------
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
				
		//nahrajeme modelview a projection matrix do shaderu
		glUniformMatrix4fv(glGetUniformLocation(g_tesselationProgramId, "u_ProjectionMatrix"), 1, GL_FALSE, &g_CameraProjectionMatrix[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(g_tesselationProgramId, "u_ModelViewMatrix"), 1, GL_FALSE, &g_CameraViewMatrix[0][0]);
		
		glUniform1i(glGetUniformLocation(g_tesselationProgramId, "u_subtriangles"), g_subtriangles);
		glUniform1i(glGetUniformLocation(g_tesselationProgramId, "u_tessFactor"), g_tesselationFactor);
		glUniform1f(glGetUniformLocation(g_tesselationProgramId, "u_maxTessDistance"), g_maxTessDistance);
		glUniform1i(glGetUniformLocation(g_tesselationProgramId, "u_freeze"), g_freeze);
		glUniform3fv(glGetUniformLocation(g_tesselationProgramId, "u_freezePos"), 1, &g_freezePos.x);

		//vertex data in texture memory
		//glBindBuffer(GL_TEXTURE_BUFFER, vertexTBO);
		//glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, originalTrianglesVBO);
		//glBindBuffer(GL_TEXTURE_BUFFER, 0);

		GLuint hVertTex = glGetUniformLocation(g_tesselationProgramId, "u_vertexTBO");
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, vertexTBO);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, originalTrianglesVBO);
		glUniform1i(hVertTex, 0);

		GLuint hHeightTex = glGetUniformLocation(g_tesselationProgramId, "u_heightTexture");
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, g_HeightMapTexId);
		glUniform1i(hHeightTex, 1);

		GLuint hDifftTex = glGetUniformLocation(g_tesselationProgramId, "u_diffTexture");
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, g_DiffuseTexId);
		glUniform1i(hDifftTex, 2);

		
		//draw call with enough verticies
		glBindBuffer(GL_ARRAY_BUFFER, emptyVBO);
		glDrawArrays(GL_TRIANGLES, 0, 3 * triangleCount * g_subtriangles);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		if(g_highlightOrig && g_WireMode) {
			//highlight original triangles
			glUseProgram(g_highlightProgramId);

			//nahrajeme modelview a projection matrix do shaderu
			glUniformMatrix4fv(glGetUniformLocation(g_tesselationProgramId, "u_ProjectionMatrix"), 1, GL_FALSE, &g_CameraProjectionMatrix[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(g_tesselationProgramId, "u_ModelViewMatrix"), 1, GL_FALSE, &g_CameraViewMatrix[0][0]);
		

			hHeightTex = glGetUniformLocation(g_highlightProgramId, "u_heightTexture");
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, g_HeightMapTexId);
			glUniform1i(hHeightTex, 0);

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


//-----------------------------------------------------------------------------
// Name: cbInitGL()
// Desc: 
//-----------------------------------------------------------------------------
void cbInitGL()
{



    // Init app GUI
    initGUI();

    // Set OpenGL state variables
    glClearColor(0.4f, 0.4f, 0.7f, 0);

    glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	//texture buffer with vertex coordinates for random acces
	glGenTextures(1, &vertexTBO);

	//meshTriangles = pgr2CreatePlaneMesh(2.0f, 10, 10, &triangleCount);

	//buffer with original triangles
	glGenBuffers(1, &originalTrianglesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, originalTrianglesVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9 * triangleCount, triangles, GL_STATIC_DRAW);


	//empty vertex buffer, using only for vertexID
	glGenBuffers(1, &emptyVBO);
	//glBindBuffer(GL_ARRAY_BUFFER, emptyVBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(empty), empty, GL_STATIC_DRAW);
	assert((emptyVBO > 0) && (originalTrianglesVBO > 0) && (vertexTBO > 0));
	
    // Load heightmap texture from raw-file
    g_HeightMapTexId = prg2LoadRGBTextureFromRawFile(HEIGHTMAP_TEXTURE_FILE_NAME, &g_NumHeightMapTexels);

	//deifuuse texture
	g_DiffuseTexId = prg2LoadRGBTextureFromRawFile(DIFFUSE_TEXTURE_FILE_NAME);

	assert(g_HeightMapTexId > 0);


    cbCompileShaderProgram(NULL);
}


//-----------------------------------------------------------------------------
// Name: cbCompileShaderProgram()
// Desc: 
//-----------------------------------------------------------------------------
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
        glProgramParameteriEXT(g_tesselationProgramId, GL_GEOMETRY_VERTICES_OUT_EXT, 6);
        glProgramParameteriEXT(g_tesselationProgramId, GL_GEOMETRY_INPUT_TYPE_EXT, GL_TRIANGLES);
        glProgramParameteriEXT(g_tesselationProgramId, GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP);
    }
    if (g_UseFragmentShader)
    {
        // Create shader objects for fragment shader
        GLuint id = pgr2CreateShaderFromFile(GL_FRAGMENT_SHADER, FS_TESS_FILE_NAME);
        glAttachShader(g_tesselationProgramId, id);
        glDeleteShader(id);
    }

	//2nd pass
	
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

/** Anttweakbar callback for gpu vs cpu button */
void TW_CALL cbFreeze(const void *value, void *clientData)
{
	g_freezePos = vec3(cameraPos);
	g_freeze = !g_freeze;
}

/** Anttweakbar callback for gpu vs cpu button */
void TW_CALL cbGetFreeze(void *value, void *clientData)
{
    *(bool*)(value) = g_freeze;
} 

//-----------------------------------------------------------------------------
// Name: initGUI()
// Desc: 
//-----------------------------------------------------------------------------
void initGUI()
{
#ifdef USE_ANTTWEAKBAR
    // Initialize AntTweakBar GUI
    if (!TwInit(TW_OPENGL, NULL))
    {
        assert(0);
    }

    TwWindowSize(g_WindowWidth, g_WindowHeight);
    TwBar *controlBar = TwNewBar("Controls");
    TwDefine(" Controls position='10 10' size='200 300' refresh=0.1 ");

    TwAddVarCB(controlBar, "use_shaders", TW_TYPE_BOOLCPP, cbSetShaderStatus, cbGetShaderStatus, NULL, " label='shaders' key=q help='Turn programmable pipeline on/off.' ");

    // Shader panel setup
    TwAddVarRW(controlBar, "vs", TW_TYPE_BOOLCPP, &g_UseVertexShader, " group='Shaders' label='vertex' key=v help='Toggle vertex shader.' ");
    //TwAddVarRW(controlBar, "gs", TW_TYPE_BOOLCPP, &g_UseGeometryShader, " group='Shaders' label='geometry' key=g help='Toggle geometry shader.' ");
    TwAddVarRW(controlBar, "fs", TW_TYPE_BOOLCPP, &g_UseFragmentShader, " group='Shaders' label='fragment' key=f help='Toggle fragment shader.' ");
    TwAddButton(controlBar, "build", cbCompileShaderProgram, NULL, " group='Shaders' label='build' key=b help='Build shader program.' ");

    // Render panel setup
    TwAddVarRW(controlBar, "wiremode", TW_TYPE_BOOLCPP, &g_WireMode, " group='Render' label='wire mode' key=m help='Toggle wire mode.' ");

    TwAddVarRW(controlBar, "Tess. factor", TW_TYPE_INT32, &g_tesselationFactor, " group='Tesselation' label='tess. factor' min=1 help='help' ");
	TwAddVarRW(controlBar, "Max tess. dist.", TW_TYPE_FLOAT, &g_maxTessDistance, " group='Tesselation' label='max tess. dist' min=1 step=0.25 help='Distance from camera where tesselatio ends' ");
	TwAddVarRW(controlBar, "Highlight", TW_TYPE_BOOLCPP, &g_highlightOrig, " group='Tesselation' label='highlight original' key=h help='Highlight original triangles' ");
	TwAddVarCB(controlBar, "Freeze", TW_TYPE_BOOLCPP, cbFreeze, cbGetFreeze, NULL, " group='Tesselation' label='Freeze tess.' help='freeze tesselation in current point' ");
	
	TwAddVarRO(controlBar, "fps", TW_TYPE_FLOAT, &g_fps, " group='INFO' label='Current fps' ");
	
#endif
}


//-----------------------------------------------------------------------------
// Name: cbWindowSizeChanged()
// Desc: 
//-----------------------------------------------------------------------------
void cbWindowSizeChanged(int width, int height)
{
    g_WindowWidth  = width;
    g_WindowHeight = height;

	glViewport(0, 0, g_WindowWidth, g_WindowHeight);
    g_CameraProjectionMatrix = glm::perspective(55.0f, GLfloat(g_WindowWidth)/g_WindowHeight, 0.010f, 100.0f);
}


//-----------------------------------------------------------------------------
// Name: cbKeyboardChanged()
// Desc: 
//-----------------------------------------------------------------------------
void cbKeyboardChanged(int key, int action)
{
    switch (key)
    {
		case 's' : // backwards
			cameraPos[0] -= g_CameraViewMatrix[0][2] * walkSpeed;
			cameraPos[1] -= g_CameraViewMatrix[1][2] * walkSpeed;
			cameraPos[2] -= g_CameraViewMatrix[2][2] * walkSpeed;
			break;
		case 'w' : // forwards
			cameraPos[0] += g_CameraViewMatrix[0][2] * walkSpeed;
			cameraPos[1] += g_CameraViewMatrix[1][2] * walkSpeed;
			cameraPos[2] += g_CameraViewMatrix[2][2] * walkSpeed;
			
			break;
		case 'a' : 
			cameraPos[0] += g_CameraViewMatrix[0][0] * walkSpeed;
			cameraPos[1] += g_CameraViewMatrix[1][0] * walkSpeed;
			cameraPos[2] += g_CameraViewMatrix[2][0] * walkSpeed;
			
			break;
		case 'd' : 
			cameraPos[0] -= g_CameraViewMatrix[0][0] * walkSpeed;
			cameraPos[1] -= g_CameraViewMatrix[1][0] * walkSpeed;
			cameraPos[2] -= g_CameraViewMatrix[2][0] * walkSpeed;
			break;

    case 'v' : g_UseVertexShader   = !g_UseVertexShader;                break;
    case 'g' : g_UseGeometryShader = !g_UseGeometryShader;              break;
    case 'f' : g_UseFragmentShader = !g_UseFragmentShader;				break;
    case 'm' : g_WireMode          = !g_WireMode;                       break;
    //case 's' : g_UseShaders		   = !g_UseShaders;                     break;
    case 'h' : g_highlightOrig     = !g_highlightOrig;                  break;
    case 'b' : 
        cbCompileShaderProgram(NULL);
        return;
        break;
    }
	return;

    printf("[m]   g_WireMode          = %s\n", g_WireMode ? "true" : "false");
    //printf("[s]   g_UseShaders        = %s\n", g_UseShaders ? "true" : "false");
    printf("[v]   g_UseVertexShader   = %s\n", g_UseVertexShader ? "true" : "false");
    printf("[g]   g_UseGeometryShader = %s\n", g_UseGeometryShader ? "true" : "false");
    printf("[f]   g_UseFragmentShader = %s\n", g_UseFragmentShader ? "true" : "false");
    printf("[h]   g_highlightOrig     = %s\n", g_highlightOrig ? "true" : "false");
    printf("[b]   re-compile shaders\n\n");
}

#ifdef USE_ANTTWEAKBAR
//-----------------------------------------------------------------------------
// Name: cbSetShaderStatus()
// Desc: 
//-----------------------------------------------------------------------------
void TW_CALL cbSetShaderStatus(const void *value, void *clientData)
{
    g_UseShaders = *(bool*)(value);
    // Try to compile shader program
    if (g_UseShaders)
    {
        cbCompileShaderProgram(NULL);
    }
//  TwDefine((g_UseShaders) ? " Controls/Shaders readonly=false " : " Controls/Shaders readonly=true "); 
}


//-----------------------------------------------------------------------------
// Name: cbGetShaderStatus()
// Desc: 
//-----------------------------------------------------------------------------
void TW_CALL cbGetShaderStatus(void *value, void *clientData)
{
    *(bool*)(value) = g_UseShaders;
} 

bool g_MouseRotationEnabled = false;

//-----------------------------------------------------------------------------
// Name: cbMouseButtonChanged()
// Desc: internal
//-----------------------------------------------------------------------------
void GLFWCALL cbMouseButtonChanged(int button, int action)
{
    g_MouseRotationEnabled = ((button == GLFW_MOUSE_BUTTON_LEFT) && (action == GLFW_PRESS));
}


//-----------------------------------------------------------------------------
// Name: cbMousePositionChanged()
// Desc: 
//-----------------------------------------------------------------------------
void cbMousePositionChanged(int x, int y)
{
	float dx, dy;
    dx = (float)(x - ox);
    dy = (float)(y - oy);

	ox = x;
    oy = y;

    if (g_MouseRotationEnabled)
    {
		cameraRot[0] += dy * rotateSpeed;
        cameraRot[1] += dx * rotateSpeed;
    }
}
#endif;

//-----------------------------------------------------------------------------
// Name: main()
// Desc: 
//-----------------------------------------------------------------------------
int main(int argc, char* argv[]) 
{
    return common_main(g_WindowWidth, g_WindowHeight,
                       "[PGR2] Vertex shader tesselation",
                       cbInitGL,              // init GL callback function
                       cbDisplay,             // display callback function
                       cbWindowSizeChanged,   // window resize callback function
                       cbKeyboardChanged,     // keyboard callback function
                       cbMouseButtonChanged,  // mouse button callback function
                       cbMousePositionChanged // mouse motion callback function
                       );
}
