/**
 * @file	controls.h
 * @brief	This file handles AntTweakBar menu + mouse and keyboard actions 
 */


/**
 * Items in AntTweakBar control menu
 */
void initGUI(int max_tess_fact)
{
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
    TwAddVarRO(controlBar, "vs", TW_TYPE_BOOLCPP, &g_UseVertexShader, " group='Shaders' label='vertex' help='Toggle vertex shader.' ");
    TwAddVarRO(controlBar, "gs", TW_TYPE_BOOLCPP, &g_UseGeometryShader, " group='Shaders' label='geometry' help='Toggle geometry shader.' ");
    TwAddVarRO(controlBar, "fs", TW_TYPE_BOOLCPP, &g_UseFragmentShader, " group='Shaders' label='fragment'  help='Toggle fragment shader.' ");
    TwAddButton(controlBar, "build", cbCompileShaderProgram, NULL, " group='Shaders' label='build' help='Build shader program.' ");

    // Render panel setup
    TwAddVarRW(controlBar, "wiremode", TW_TYPE_BOOLCPP, &g_WireMode, " group='Render' label='wire mode' key=r help='Toggle wire mode.' ");

	std::ostringstream oss;
	oss << " group='Tesselation' label='tess. factor' min=1 max=" << max_tess_fact << " help='help'";
    TwAddVarRW(controlBar, "Tess. factor", TW_TYPE_INT32, &g_tesselationFactor, oss.str().c_str());
	TwAddVarRW(controlBar, "Tess. distance", TW_TYPE_FLOAT, &g_maxTessDistance, " group='Tesselation' label='max tess. dist' min=1 step=0.25 help='Distance from camera where tesselatio ends' ");
	TwAddVarCB(controlBar, "Freeze", TW_TYPE_BOOLCPP, cbFreeze, cbGetFreeze, NULL, " group='Tesselation' label='Freeze tess.' key=f help='freeze tesselation in current point' ");
	TwAddVarRW(controlBar, "Highlight", TW_TYPE_BOOLCPP, &g_highlightOrig, " group='Tesselation' label='Highlight grid' key=h help='Highlight original triangles' ");
	
	TwAddVarRO(controlBar, "fps", TW_TYPE_FLOAT, &g_fps, " group='INFO' label='Current fps' ");
}

/**
 * Use OpenGL to compute camera's projection and world-to-camera space 
 * transformation matrixes
 * this code is inspired by NVIDIA CUDA samples v5.0 (Smoke Particles)
 */
void updateCameraViewMatrix() {
	//camera alway above the ground
    if (cameraPos[1] > -0.1f) {
        cameraPos[1] = -0.1f;
    }
	//camera inertia
    cameraPosLag += (cameraPos - cameraPosLag) * inertia;
    cameraRotLag += (cameraRot - cameraRotLag) * inertia;

    // view transform
	g_CameraViewMatrix = rotate(mat4(1.0f), cameraRotLag[0], vec3(1.0, 0.0, 0.0));
	g_CameraViewMatrix = rotate(g_CameraViewMatrix, cameraRotLag[1], vec3(0.0, 1.0, 0.0));
	g_CameraViewMatrix = translate(g_CameraViewMatrix, cameraPosLag);

}

/**
 * Callback for keyboard actions
 */
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
		case 'a' : //left
			cameraPos[0] += g_CameraViewMatrix[0][0] * walkSpeed;
			cameraPos[1] += g_CameraViewMatrix[1][0] * walkSpeed;
			cameraPos[2] += g_CameraViewMatrix[2][0] * walkSpeed;
			break;
		case 'd' : //right
			cameraPos[0] -= g_CameraViewMatrix[0][0] * walkSpeed;
			cameraPos[1] -= g_CameraViewMatrix[1][0] * walkSpeed;
			cameraPos[2] -= g_CameraViewMatrix[2][0] * walkSpeed;
			break;
		case 'r' : 
			g_WireMode          = !g_WireMode;                       
			break;
		case 'h' : 
			g_highlightOrig     = !g_highlightOrig;
			break;
		case 'f' : 
			cbFreeze(NULL, NULL);
			break;
    }
}

/**
 * Callback for mouse click, rotation enabled while holding left button
 */
void GLFWCALL cbMouseButtonChanged(int button, int action)
{
    g_MouseRotationEnabled = ((button == GLFW_MOUSE_BUTTON_LEFT) && (action == GLFW_PRESS));
}

/**
 * Handles rotation with mouse
 */
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

/**
 * Callback for freeze buton in menu
 */
void TW_CALL cbFreeze(const void *value, void *clientData)
{
	g_freezePos = vec3(cameraPos);
	g_freeze = !g_freeze;
}

/**
 * Callback to get current freeze state
 */
void TW_CALL cbGetFreeze(void *value, void *clientData)
{
    *(bool*)(value) = g_freeze;
} 

/**
 * Resize the window
 */
void cbWindowSizeChanged(int width, int height)
{
    g_WindowWidth  = width;
    g_WindowHeight = height;

	glViewport(0, 0, g_WindowWidth, g_WindowHeight);
    g_CameraProjectionMatrix = glm::perspective(55.0f, GLfloat(g_WindowWidth)/g_WindowHeight, 0.010f, 100.0f);
}