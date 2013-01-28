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


// GLOBAL CONSTANTS____________________________________________________________
const char* VS_TESS_FILE_NAME			= "src/tesselation.vert";
const char* VS_HIGHLIGHT_FILE_NAME		= "src/highlight.vert";

const char* GS_FILE_NAME				= "src/simple.geom";

const char* FS_TESS_FILE_NAME			= "src/tesselation.frag";
const char* FS_HIGHLIGHT_FILE_NAME		= "src/highlight.frag";

// GLOBAL VARIABLES____________________________________________________________
GLint    g_WindowWidth       = 800;    // Window width
GLint    g_WindowHeight      = 600;    // Window height

GLfloat  g_SceneRot[]        = { 0.0f, 0.0f, 0.0f, 1.0f }; // Scene orientation
GLfloat  g_SceneTraZ         = 5.0f;   // Scene translation along z-axis
bool     g_SceneRotEnabled   = false;  // Scene auto-rotation enabled/disabled
bool     g_WireMode          = true;  // Wire mode enabled/disabled
bool     g_FaceCulling       = false;  // Face culling enabled/disabled

bool     g_UseShaders        = true;  // Programmable pipeline on/off
bool     g_UseVertexShader   = true;  // Use vertex shader
bool     g_UseGeometryShader = false;  // Use geometry shader
bool     g_UseFragmentShader = true;  // Use fragment shader

GLuint emptyVBO = 0;
GLuint originalTrianglesVBO = 0;
GLuint vertexTBO = 0;
GLuint textureBO = 0;

GLuint meshID = 0;

int pass;
GLint g_tesselationFactor = 4;
GLint g_subtriangles;

enum EGeometry                         // Geometry type enum
{	
    ELEPHANT_GEOMETRY = 0, 
    CUBE_GEOMETRY,
    NUM_GEOMETRY_TYPES
};
int g_GeometryType = ELEPHANT_GEOMETRY; // Geometry type


// GLSL variables
GLuint g_tesselationProgramId = 0;                 // Shader program id
GLuint g_highlightProgramId = 0;				   // Shader program id



// FORWARD DECLARATIONS________________________________________________________
#ifdef USE_ANTTWEAKBAR
    void TW_CALL cbSetShaderStatus(const void*, void*);
    void TW_CALL cbGetShaderStatus(void*, void*);
#endif
void TW_CALL cbCompileShaderProgram(void *clientData);
void initGUI();

const int triangleCount = 3;
float triangles[9*triangleCount] =	{0, 0, 0,  0, 1, 0,  1, 0.5f, 0,
									 0, 0, 0,  0, -1, 0,  1, -0.5f, 0,
									 0, 0, 0,  1, 0.5f, 0,  1, -0.5f, 0};

float empty[] = {0};

//-----------------------------------------------------------------------------
// Name: cbDisplay()
// Desc: 
//-----------------------------------------------------------------------------
void cbDisplay()
{

    static GLfloat scene_rot = 0.0f;
    static GLuint s_tex_id = prg2CreateSimpleTexture(256, 256);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, g_WireMode ? GL_LINE : GL_FILL);
    (g_FaceCulling) ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);

    // Setup camera
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -g_SceneTraZ);
    pgr2AddQuaternionRotationToTransformation(g_SceneRot);
    glRotatef(scene_rot, 0.0f, 1.0f, 0.0f);

    // Turn on programmable pipeline
    if (g_UseShaders)
    {

        glUseProgram(g_tesselationProgramId);    // Active shader program

		g_subtriangles = g_tesselationFactor * g_tesselationFactor;
				
		//nahrajeme modelview a projection matrix do shaderu
		GLfloat matrix[16] = {0};
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		glUniformMatrix4fv(glGetUniformLocation(g_tesselationProgramId, "u_ModelViewMatrix"), 1, GL_FALSE, matrix);
		
		glGetFloatv(GL_PROJECTION_MATRIX, matrix);
		glUniformMatrix4fv(glGetUniformLocation(g_tesselationProgramId, "u_ProjectionMatrix"), 1, GL_FALSE, matrix);
		
		glUniform1i(glGetUniformLocation(g_tesselationProgramId, "u_subtriangles"), g_subtriangles);
		glUniform1i(glGetUniformLocation(g_tesselationProgramId, "u_tessFactor"), g_tesselationFactor);
				
		//vertex data in texture memory
		glBindBuffer(GL_TEXTURE_BUFFER, vertexTBO);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, originalTrianglesVBO);
		glBindBuffer(GL_TEXTURE_BUFFER, 0);
		
		//draw call with enough verticies
		glBindBuffer(GL_ARRAY_BUFFER, emptyVBO);
		glDrawArrays(GL_TRIANGLES, 0, 3 * triangleCount * g_subtriangles);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		

		//2nd pass to highlight original triangles
		glUseProgram(g_highlightProgramId);

		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		glUniformMatrix4fv(glGetUniformLocation(g_highlightProgramId, "u_ModelViewMatrix"), 1, GL_FALSE, matrix);
		
		glGetFloatv(GL_PROJECTION_MATRIX, matrix);
		glUniformMatrix4fv(glGetUniformLocation(g_highlightProgramId, "u_ProjectionMatrix"), 1, GL_FALSE, matrix);

		glBindBuffer(GL_ARRAY_BUFFER, originalTrianglesVBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
		glDrawArrays(GL_TRIANGLES, 0, 3 * triangleCount);
        glDisableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

	}

	
    // Turn off programmable pipeline
    glUseProgram(NULL);

    if (g_SceneRotEnabled)
    {
        scene_rot+=0.25;
    }
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
    (g_FaceCulling) ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);

	//texture buffer with vertex coordinates for random acces
	glGenBuffers(1, &vertexTBO);

	//buffer with original triangles
	glGenBuffers(1, &originalTrianglesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, originalTrianglesVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9 * triangleCount, &triangles, GL_STATIC_DRAW);

	//empty vertex buffer, using only for vertexID
	glGenBuffers(1, &emptyVBO);
	//glBindBuffer(GL_ARRAY_BUFFER, emptyVBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(empty), empty, GL_STATIC_DRAW);


	//meshTriangles = pgr2CreatePlaneMesh(2.0f, 10, 10, &meshVerticies);

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
    TwDefine(" Controls position='10 10' size='200 340' refresh=0.1 ");

    TwAddVarCB(controlBar, "use_shaders", TW_TYPE_BOOLCPP, cbSetShaderStatus, cbGetShaderStatus, NULL, " label='shaders' key=s help='Turn programmable pipeline on/off.' ");

    // Shader panel setup
    TwAddVarRW(controlBar, "vs", TW_TYPE_BOOLCPP, &g_UseVertexShader, " group='Shaders' label='vertex' key=v help='Toggle vertex shader.' ");
    TwAddVarRW(controlBar, "gs", TW_TYPE_BOOLCPP, &g_UseGeometryShader, " group='Shaders' label='geometry' key=g help='Toggle geometry shader.' ");
    TwAddVarRW(controlBar, "fs", TW_TYPE_BOOLCPP, &g_UseFragmentShader, " group='Shaders' label='fragment' key=f help='Toggle fragment shader.' ");
    TwAddButton(controlBar, "build", cbCompileShaderProgram, NULL, " group='Shaders' label='build' key=b help='Build shader program.' ");
//  TwDefine( " Controls/Shaders readonly=true "); 

    // Render panel setup
    TwAddVarRW(controlBar, "wiremode", TW_TYPE_BOOLCPP, &g_WireMode, " group='Render' label='wire mode' key=w help='Toggle wire mode.' ");
    TwAddVarRW(controlBar, "face_culling", TW_TYPE_BOOLCPP, &g_FaceCulling, " group=Render label='face culling' key=c help='Toggle face culling.' ");

    // Scene panel setup
    TwEnumVal geometry_type[] = 
    { 
        { ELEPHANT_GEOMETRY    , "Elephant"},
        { CUBE_GEOMETRY        , "Cube"    },
    };
    TwType geom_type = TwDefineEnum("Model", geometry_type, NUM_GEOMETRY_TYPES);
    TwAddVarRW(controlBar, "model", geom_type, &g_GeometryType, " group='Scene' keyIncr=Space help='Change model.' ");
    TwAddVarRW(controlBar, "auto-rotation", TW_TYPE_BOOLCPP, &g_SceneRotEnabled, " group='Scene' label='rotation' key=r help='Toggle scene rotation.' ");
    TwAddVarRW(controlBar, "Translate", TW_TYPE_FLOAT, &g_SceneTraZ, " group='Scene' label='translate' min=1 max=1000 step=0.5 keyIncr=t keyDecr=T help='Scene translation.' ");
    TwAddVarRW(controlBar, "SceneRotation", TW_TYPE_QUAT4F, &g_SceneRot, " group='Scene' label='rotation' open help='Toggle scene orientation.' ");

    TwAddVarRW(controlBar, "Tess. factor", TW_TYPE_INT32, &g_tesselationFactor, " group='Tesselation' label='tess. factor' min=1 help='help' ");
#endif
}


//-----------------------------------------------------------------------------
// Name: cbWindowSizeChanged()
// Desc: 
//-----------------------------------------------------------------------------
void cbWindowSizeChanged(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(55.0f, GLfloat(width)/height, 0.1f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);

    g_WindowWidth  = width;
    g_WindowHeight = height;
}


//-----------------------------------------------------------------------------
// Name: cbKeyboardChanged()
// Desc: 
//-----------------------------------------------------------------------------
void cbKeyboardChanged(int key, int action)
{
    switch (key)
    {
    case GLFW_KEY_SPACE:
        g_GeometryType = (g_GeometryType + 1) % NUM_GEOMETRY_TYPES;
        break;
    case 't' : g_SceneTraZ        += 0.5f;                               break;
    case 'T' : g_SceneTraZ        -= (g_SceneTraZ > 0.5) ? 0.5f : 0.0f;  break;
    case 'r' : g_SceneRotEnabled   = !g_SceneRotEnabled;                 break;
    case 'v' : g_UseVertexShader   = !g_UseVertexShader;                 break;
    case 'g' : g_UseGeometryShader = !g_UseGeometryShader;               break;
    case 'f' : g_UseFragmentShader = !g_UseFragmentShader;               break;
    case 'w' : g_WireMode          = !g_WireMode;                        break;
    case 'c' : g_FaceCulling       = !g_FaceCulling;                     break;
    case 's' : g_UseShaders = !g_UseShaders;                             break;
    case 'b' : 
        cbCompileShaderProgram(NULL);
        return;
        break;
    }

    printf("[t/T] g_SceneTraZ         = %f\n", g_SceneTraZ);
    printf("[r]   g_SceneRotEnabled   = %s\n", g_SceneRotEnabled ? "true" : "false");
    printf("[w]   g_WireMode          = %s\n", g_WireMode ? "true" : "false");
    printf("[c]   g_FaceCulling       = %s\n\n", g_FaceCulling ? "true" : "false");
    printf("[s]   g_UseShaders        = %s\n", g_UseShaders ? "true" : "false");
    printf("[v]   g_UseVertexShader   = %s\n", g_UseVertexShader ? "true" : "false");
    printf("[g]   g_UseGeometryShader = %s\n", g_UseGeometryShader ? "true" : "false");
    printf("[f]   g_UseFragmentShader = %s\n", g_UseFragmentShader ? "true" : "false");
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
#else
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
    static int s_LastMousePoxX = x;
    static int s_LastMousePoxY = y;

    if (g_MouseRotationEnabled)
    {
        g_SceneRot[1] +=  0.9f*(x - s_LastMousePoxX);
        g_SceneRot[2] +=  0.9f*(y - s_LastMousePoxY);
        s_LastMousePoxX = x;
        s_LastMousePoxY = y;
    }
}
#endif


//-----------------------------------------------------------------------------
// Name: main()
// Desc: 
//-----------------------------------------------------------------------------
int main(int argc, char* argv[]) 
{
    return common_main(g_WindowWidth, g_WindowHeight,
                       "[PGR2] Simple GLSL Example",
                       cbInitGL,              // init GL callback function
                       cbDisplay,             // display callback function
                       cbWindowSizeChanged,   // window resize callback function
                       cbKeyboardChanged,     // keyboard callback function
#ifdef USE_ANTTWEAKBAR
                       NULL,                  // mouse button callback function
                       NULL                   // mouse motion callback function
#else
                       cbMouseButtonChanged,  // mouse button callback function
                       cbMousePositionChanged // mouse motion callback function
#endif
                       );
}
