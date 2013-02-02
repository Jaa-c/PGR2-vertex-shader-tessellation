//-----------------------------------------------------------------------------
//  [PGR2] Common function definitions
//  27/02/2008
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
#include "GLEW/glew.h"
#include "GLFW/glfw.h"

#include <assert.h>
#ifdef USE_ANTTWEAKBAR
#  include "../common/AntTweakBar/AntTweakBar.h"
#else
#  include <stdio.h>
#  include <stdlib.h>
#  define TW_CALL
#endif

#include "../common/glm/glm.hpp"
#include "../common/glm/gtc/matrix_transform.hpp"
#include "tools.h"
#include "math.h"

// FUNCTION POINTER TYPES______________________________________________________
/* Function pointer types */
typedef void (* TInitGLCallback)(void);
typedef void (* TDisplayCallback)(void);
typedef void (GLFWCALL * TWindowSizeChangedCallback)(int,int);
typedef void (GLFWCALL * TWindowRefreshCallback)(void);
typedef void (GLFWCALL * TMouseButtonChangedCallback)(int,int);
typedef void (GLFWCALL * TMousePositionChangedCallback)(int,int);
typedef void (GLFWCALL * TMouseWheelChangedCallback)(int);
typedef void (GLFWCALL * TKeyboardChangedCallback)(int,int);
typedef void (GLFWCALL * TThreadCallback)(void *);
//typedef int  (GLFWCALL * TWindowClosedCallback)(void);
//typedef void (GLFWCALL * TCharacterChangedCallback)(int,int);


// INTERNAL USER CALLBACK FUNCTION POINTERS____________________________________
TDisplayCallback              g_cbUserDisplay               = NULL;
TWindowSizeChangedCallback    g_cbUserWindowSizeChanged     = NULL;
TMouseButtonChangedCallback   g_cbUserMouseButtonChanged    = NULL;
TMousePositionChangedCallback g_cbUserMousePositionChanged  = NULL;
TKeyboardChangedCallback      g_cbUserKeyboardChanged       = NULL;


// INTERNAL CALLBACK FUNCTION DEFINITIONS______________________________________

//-----------------------------------------------------------------------------
// Name: _cbWindowSizeChanged()
// Desc: internal
//-----------------------------------------------------------------------------
void GLFWCALL _cbWindowSizeChanged(int width, int height)
{
    if (height < 1) 
    {
        height = 1;
    }

    if (g_cbUserWindowSizeChanged)
    {
        g_cbUserWindowSizeChanged(width, height);
    }

    // Send the new window size to AntTweakBar
#ifdef USE_ANTTWEAKBAR
    TwWindowSize(width, height);
#endif
}

//-----------------------------------------------------------------------------
// Name: _cbKeyboardChanged()
// Desc: internal
//-----------------------------------------------------------------------------
void GLFWCALL _cbKeyboardChanged(int key, int action)
{
//   TwEventKeyGLFW(key, action);
#ifdef USE_ANTTWEAKBAR
    if (!TwEventCharGLFW(key, action))
#endif
    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_ESC)
        {
            exit(0);
        }
      
        if (g_cbUserKeyboardChanged)
        {
            g_cbUserKeyboardChanged(key, action);
        }
    }
}


//-----------------------------------------------------------------------------
// Name: _cbMouseButtonChanged()
// Desc: internal
//-----------------------------------------------------------------------------
void GLFWCALL _cbMouseButtonChanged(int button, int action)
{
#ifdef USE_ANTTWEAKBAR
    if (!TwEventMouseButtonGLFW(button, action))
#endif
    if (g_cbUserMouseButtonChanged)
    {
        g_cbUserMouseButtonChanged(button, action);
    }
}


//-----------------------------------------------------------------------------
// Name: _cbMousePositionChanged()
// Desc: internal
//-----------------------------------------------------------------------------
void GLFWCALL _cbMousePositionChanged(int x, int y)
{
#ifdef USE_ANTTWEAKBAR
     if (!TwEventMousePosGLFW(x, y))
#endif
    if (g_cbUserMousePositionChanged)
    {
        g_cbUserMousePositionChanged(x, y);
    }
}


//-----------------------------------------------------------------------------
// Name: _cbPrintOGLDebugLog()
// Desc: 
//-----------------------------------------------------------------------------
void __stdcall _cbPrintOGLDebugLog(GLenum source, GLenum type, GLuint id, GLenum severity,
                        GLsizei length, const GLchar* message, GLvoid* userParam)
{
	switch(source)
    {
    case GL_DEBUG_SOURCE_API_ARB            : printf("Source  : API\n"); break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB  : printf("Source  : window system\n"); break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB: printf("Source  : shader compiler\n"); break;
	case GL_DEBUG_SOURCE_THIRD_PARTY_ARB    : printf("Source  : third party\n"); break;
	case GL_DEBUG_SOURCE_APPLICATION_ARB    : printf("Source  : application\n"); break;
	case GL_DEBUG_SOURCE_OTHER_ARB          : printf("Source  : other\n"); break;
	default                                 : printf("Source  : unknown\n"); break;
	}

	switch(type)
    {
    case GL_DEBUG_TYPE_ERROR_ARB                : printf("Type    : error\n"); break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB  : printf("Type    : deprecated behaviour\n"); break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB   : printf("Type    : undefined behaviour\n"); break;
	case GL_DEBUG_TYPE_PORTABILITY_ARB          : printf("Type    : portability issue\n"); break;
	case GL_DEBUG_TYPE_PERFORMANCE_ARB          : printf("Type    : performance issue\n"); break;
	case GL_DEBUG_TYPE_OTHER_ARB                : printf("Type    : other\n"); break;
	default                                     : printf("Type    : unknown\n"); break;
	}

    printf("ID      : %d\n", id);

    switch(severity)
    {
    case GL_DEBUG_SEVERITY_HIGH_ARB     : printf("Severity: high\n"); break;
	case GL_DEBUG_SEVERITY_MEDIUM_ARB   : printf("Severity: medium\n"); break;
	case GL_DEBUG_SEVERITY_LOW_ARB      : printf("Severity: low\n"); break;
	default                             : printf("Severity: unknown\n"); break;
	}

    printf("Message : %s\n", message);
    printf("-------------------------------------------------------------------------------\n");
//  assert(type != GL_DEBUG_TYPE_ERROR_ARB);
}


//-----------------------------------------------------------------------------
// Name: common_main()
// Desc: 
//-----------------------------------------------------------------------------
int common_main(int window_width, int window_height, const char* window_title,
                int* opengl_config,
                TInitGLCallback               cbUserInitGL,
                TDisplayCallback              cbUserDisplay,
                TWindowSizeChangedCallback    cbUserWindowSizeChanged,
                TKeyboardChangedCallback      cbUserKeyboardChanged,
                TMouseButtonChangedCallback   cbUserMouseButtonChanged,
                TMousePositionChangedCallback cbUserMousePositionChanged) 
{
    // Setup user callback functions
    assert(cbUserDisplay && cbUserInitGL);
    g_cbUserDisplay               = cbUserDisplay;
    g_cbUserWindowSizeChanged     = cbUserWindowSizeChanged;
    g_cbUserKeyboardChanged       = cbUserKeyboardChanged;
    g_cbUserMouseButtonChanged    = cbUserMouseButtonChanged;
    g_cbUserMousePositionChanged  = cbUserMousePositionChanged;

    // Intialize GLFW   
    glfwInit();
    bool bDebugOutput = false;

    if (opengl_config != NULL)
    {
        while (*opengl_config != NULL)
        {
            glfwOpenWindowHint(opengl_config[0], opengl_config[1]);
            if ((opengl_config[0] == GLFW_OPENGL_DEBUG_CONTEXT) && (opengl_config[1] == GL_TRUE))
            {
                bDebugOutput = true;
            }
            opengl_config += 2;
        }
    }

    // Create a window
    glfwOpenWindow(window_width, window_height, 0, 0, 0, 0, 32, 0, GLFW_WINDOW);
    glfwSetWindowTitle(window_title);
    glfwSetWindowPos(100, 100);

    glfwEnable(GLFW_MOUSE_CURSOR);
    glfwEnable(GLFW_KEY_REPEAT);

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        printf("Error: %s\n", glewGetErrorString(err));
        assert(0);
        return 1;
    }

    // Enable OGL debug 
    if (bDebugOutput)
    {
        if (glfwExtensionSupported("GL_ARB_debug_output"))
        {
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
            glDebugMessageCallbackARB(_cbPrintOGLDebugLog, NULL);
            glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW_ARB, 0, NULL, GL_TRUE);
        }
    }
    // Init OGL
    if (cbUserInitGL)
    {
        cbUserInitGL();
    }

    // Set GLFW event callbacks
    glfwSetWindowSizeCallback(_cbWindowSizeChanged);
    glfwSetCharCallback(_cbKeyboardChanged);
    glfwSetMouseButtonCallback(_cbMouseButtonChanged);
    glfwSetMousePosCallback(_cbMousePositionChanged);
    //   glfwSetKeyCallback(cbKeyboardChanged);
  
   // Main loop
    while(glfwGetWindowParam(GLFW_OPENED) && !glfwGetKey(GLFW_KEY_ESC))
    {
        if (g_cbUserDisplay)
        {
            g_cbUserDisplay();
        }

#ifdef USE_ANTTWEAKBAR
        // Draw tweak bars
        TwDraw();
#endif
        // Present frame buffer
        glfwSwapBuffers();
    }
 
    // Terminate AntTweakBar and GLFW
#ifdef USE_ANTTWEAKBAR
    TwTerminate();
#endif
    glfwTerminate();    // Terminate GLFW

    return 0;
} 


//-----------------------------------------------------------------------------
// Name: common_main()
// Desc: 
//-----------------------------------------------------------------------------
int common_main(int window_width, int window_height, const char* window_title,
                TInitGLCallback               cbUserInitGL,
                TDisplayCallback              cbUserDisplay,
                TWindowSizeChangedCallback    cbUserWindowSizeChanged,
                TKeyboardChangedCallback      cbUserKeyboardChanged,
                TMouseButtonChangedCallback   cbUserMouseButtonChanged,
                TMousePositionChangedCallback cbUserMousePositionChanged) 
{
    return common_main(window_width, window_height, 
                       window_title,
                       NULL,
                       cbUserInitGL,
                       cbUserDisplay,
                       cbUserWindowSizeChanged,
                       cbUserKeyboardChanged,
                       cbUserMouseButtonChanged,
                       cbUserMousePositionChanged);
}
