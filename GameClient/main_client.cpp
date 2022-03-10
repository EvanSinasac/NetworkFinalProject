// Probably need a global things but we'll see
#include "globalThings.h"

#include <conio.h>
#include <iostream>
#include <random>
#include <time.h>
#include <assert.h>

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>


#pragma comment(lib, "ws2_32.lib")	//Winsock library


double deltaTime = 0.0;
double lastFrame = 0.0;

std::vector<std::string> modelLocations;

//Function signature for DrawObject()
void DrawObject(
	cMesh* pCurrentMesh,
	glm::mat4 matModel,
	GLint matModel_Location,
	GLint matModelInverseTranspose_Location,
	GLuint program,
	cVAOManager* pVAOManager);



int main(int argc, char** argv)
{
	//UDPClient client;
	::client.CreateSocket("127.0.0.1", 5155);

	printf("Press enter to start\n");

	//bool waitForStart = true;
	int ch;
	//do
	//{
		if (_kbhit)
		{
			ch = _getch();
			//client.Send((char*)(&ch), 1);
			switch (ch)
			{
			case 13:
				client.SendStart();
				break;
			default:
				break;
			}

			client.Update();
		}

	//} while (!::playTime);
	

	GLFWwindow* pWindow;
	GLuint program = 0;		//0 means no shader program

	GLint mvp_location = -1;
	std::stringstream ss;

	glfwSetErrorCallback(GLFW_error_callback);

	if (!glfwInit())
	{
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	pWindow = glfwCreateWindow(1200, 640, "Networking Pong", NULL, NULL);

	if (!pWindow)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(pWindow, GLFW_key_callback);

	glfwSetCursorEnterCallback(pWindow, GLFW_cursor_enter_callback);
	glfwSetCursorPosCallback(pWindow, GLFW_cursor_position_callback);
	glfwSetScrollCallback(pWindow, GLFW_scroll_callback);
	glfwSetMouseButtonCallback(pWindow, GLFW_mouse_button_callback);
	glfwSetWindowSizeCallback(pWindow, GLFW_window_size_callback);


	//glfwSetCursorPosCallback(pWindow, mouse_callback);
	//glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(pWindow);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	GLint max_uniform_location = 0;
	GLint* p_max_uniform_location = NULL;
	p_max_uniform_location = &max_uniform_location;
	glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, p_max_uniform_location);

	std::cout << "GL_MAX_UNIFORM_LOCATIONS: " << max_uniform_location << std::endl;

	// Create global things
	::g_StartUp(pWindow);

	/*::g_pFlyCamera->setEye(glm::vec3(0.0f, 0.0f, 20.0f));
	std::cout << "Fly Camera At: " << ::g_pFlyCamera->getAt().x << " " << ::g_pFlyCamera->getAt().y << " " << ::g_pFlyCamera->getAt().z << std::endl;*/

	// Shaders with textures and lights, probably going to disable lights unless it looks OK
	cShaderManager::cShader vertShader;
	ss.str("");
	ss << SOLUTION_DIR << "common\\assets\\shaders\\vertShader_01.glsl";
	vertShader.fileName = ss.str();

	cShaderManager::cShader geomShader;
	ss.str("");
	ss << SOLUTION_DIR << "common\\assets\\shaders\\geomShader.glsl";
	geomShader.fileName = ss.str();;

	cShaderManager::cShader fragShader;
	ss.str("");
	ss << SOLUTION_DIR << "common\\assets\\shaders\\fragShader_01.glsl";
	fragShader.fileName = ss.str();

	if (::g_pShaderManager->createProgramFromFile("Shader#1", vertShader, geomShader, fragShader))
	{
		std::cout << "Shader compiled OK" << std::endl;
		// 
		// Set the "program" variable to the one the Shader Manager used...
		program = ::g_pShaderManager->getIDFromFriendlyName("Shader#1");
	}
	else
	{
		std::cout << "Error making shader program: " << std::endl;
		std::cout << ::g_pShaderManager->getLastError() << std::endl;
	}

	// Select the shader program we want to use
	// (Note we only have one shader program at this point)
	glUseProgram(program);

	// *******************************************************
	// Now, I'm going to "load up" all the uniform locations
	// (This was to show how a map could be used)
	// Stuff we've started doing in graphics to make things faster 
	cShaderManager::cShaderProgram* pShaderProc = ::g_pShaderManager->pGetShaderProgramFromFriendlyName("Shader#1");
	int theUniformIDLoc = -1;
	theUniformIDLoc = glGetUniformLocation(program, "matModel");

	pShaderProc->mapUniformName_to_UniformLocation["matModel"] = theUniformIDLoc;

	// Or...
	pShaderProc->mapUniformName_to_UniformLocation["matModel"] = glGetUniformLocation(program, "matModel");

	pShaderProc->mapUniformName_to_UniformLocation["matView"] = glGetUniformLocation(program, "matView");
	pShaderProc->mapUniformName_to_UniformLocation["matProjection"] = glGetUniformLocation(program, "matProjection");
	pShaderProc->mapUniformName_to_UniformLocation["matModelInverseTranspose"] = glGetUniformLocation(program, "matModelInverseTranspose");

	// *******************************************************

	//GLint mvp_location = -1;
	mvp_location = glGetUniformLocation(program, "MVP");

	// Get "uniform locations" (aka the registers these are in)
	GLint matModel_Location = glGetUniformLocation(program, "matModel");
	//	GLint matView_Location = glGetUniformLocation(program, "matView");
	GLint matProjection_Location = glGetUniformLocation(program, "matProjection");
	GLint matModelInverseTranspose_Location = glGetUniformLocation(program, "matModelInverseTranspose");

	GLint bDiscardTransparencyWindowsON_LocID = glGetUniformLocation(program, "bDiscardTransparencyWindowsOn");

	// Lights stuff here but I'mma just set all the meshes to not use light

	// Get the uniform locations of the light shader values
	::g_pTheLights->SetUpUniformLocations(program);

	ss.str("");
	ss << SOLUTION_DIR << "common\\assets\\models\\";

	::g_pVAOManager->setFilePath(ss.str());

	// Load ALL the models
	modelLocations.push_back("Isosphere_Smooth_Normals.ply");
	modelLocations.push_back("SM_Env_Construction_Wall_01_xyz_n_rgba_uv.ply");
	modelLocations.push_back("Isosphere_Smooth_Inverted_Normals_for_SkyBox.ply");

	unsigned int totalVerticesLoaded = 0;
	unsigned int totalTrianglesLoaded = 0;
	for (std::vector<std::string>::iterator itModel = modelLocations.begin(); itModel != modelLocations.end(); itModel++)
	{
		sModelDrawInfo theModel;
		std::string modelName = *itModel;
		std::cout << "Loading " << modelName << "...";
		if (!::g_pVAOManager->LoadModelIntoVAO(modelName, theModel, program))
		{
			std::cout << "didn't work because: " << std::endl;
			std::cout << ::g_pVAOManager->getLastError(true) << std::endl;
		}
		else
		{
			std::cout << "OK." << std::endl;
			std::cout << "\t" << theModel.numberOfVertices << " vertices and " << theModel.numberOfTriangles << " triangles loaded." << std::endl;
			totalTrianglesLoaded += theModel.numberOfTriangles;
			totalVerticesLoaded += theModel.numberOfVertices;
		}
	} //end of for (std::vector<std::string>::iterator itModel

	std::cout << "Done loading models." << std::endl;
	std::cout << "Total vertices loaded = " << totalVerticesLoaded << std::endl;
	std::cout << "Total triangles loaded = " << totalTrianglesLoaded << std::endl;

	cMesh* pBall = new cMesh();
	pBall->meshName = "Isosphere_Smooth_Normals.ply";
	pBall->positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
	pBall->scale = glm::vec3(0.5f);
	pBall->bDontLight = true;
	pBall->bUseWholeObjectDiffuseColour = true;
	pBall->bUseObjectDebugColour = true;
	pBall->wholeObjectDiffuseRGBA = glm::vec4(1.0f, 0.8f, 1.0f, 1.0f);
	pBall->objectDebugColourRGBA = glm::vec4(1.0f, 0.8f, 1.0f, 1.0f);
	pBall->alphaTransparency = 1.0f;
	pBall->clearTextureRatiosToZero();
	pBall->friendlyName = "Ball";
	pBall->friendlyID = 0;
	::g_vec_pMeshes.push_back(pBall);

	cMesh* pPlayer1Sphere = new cMesh();
	pPlayer1Sphere->meshName = "Isosphere_Smooth_Normals.ply";
	pPlayer1Sphere->positionXYZ = glm::vec3(9.0f, 0.0f, 0.0f);
	pPlayer1Sphere->scale = glm::vec3(0.5f);
	pPlayer1Sphere->bDontLight = true;
	pPlayer1Sphere->bUseWholeObjectDiffuseColour = true;
	pPlayer1Sphere->bUseObjectDebugColour = true;
	pPlayer1Sphere->wholeObjectDiffuseRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	pPlayer1Sphere->objectDebugColourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	pPlayer1Sphere->alphaTransparency = 1.0f;
	pPlayer1Sphere->clearTextureRatiosToZero();
	pPlayer1Sphere->friendlyName = "Player 1";
	pPlayer1Sphere->friendlyID = 1;
	::g_vec_pMeshes.push_back(pPlayer1Sphere);

	cMesh* pPlayer2Sphere = new cMesh();
	pPlayer2Sphere->meshName = "Isosphere_Smooth_Normals.ply";
	pPlayer2Sphere->positionXYZ = glm::vec3(-9.0f, 0.0f, 0.0f);
	pPlayer2Sphere->scale = glm::vec3(0.5f);
	pPlayer2Sphere->bDontLight = true;
	pPlayer2Sphere->bUseWholeObjectDiffuseColour = true;
	pPlayer2Sphere->bUseObjectDebugColour = true;
	pPlayer2Sphere->wholeObjectDiffuseRGBA = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	pPlayer2Sphere->objectDebugColourRGBA = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	pPlayer2Sphere->alphaTransparency = 1.0f;
	pPlayer2Sphere->clearTextureRatiosToZero();
	pPlayer2Sphere->friendlyName = "Player 2";
	pPlayer2Sphere->friendlyID = 2;
	::g_vec_pMeshes.push_back(pPlayer2Sphere);

	cMesh* pPlayer3Sphere = new cMesh();
	pPlayer3Sphere->meshName = "Isosphere_Smooth_Normals.ply";
	pPlayer3Sphere->positionXYZ = glm::vec3(0.0f, 4.0f, 0.0f);
	pPlayer3Sphere->scale = glm::vec3(0.5f);
	pPlayer3Sphere->bDontLight = true;
	pPlayer3Sphere->bUseWholeObjectDiffuseColour = true;
	pPlayer3Sphere->bUseObjectDebugColour = true;
	pPlayer3Sphere->wholeObjectDiffuseRGBA = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	pPlayer3Sphere->objectDebugColourRGBA = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	pPlayer3Sphere->alphaTransparency = 1.0f;
	pPlayer3Sphere->clearTextureRatiosToZero();
	pPlayer3Sphere->friendlyName = "Player 3";
	pPlayer3Sphere->friendlyID = 3;
	::g_vec_pMeshes.push_back(pPlayer3Sphere);

	cMesh* pPlayer4Sphere = new cMesh();
	pPlayer4Sphere->meshName = "Isosphere_Smooth_Normals.ply";
	pPlayer4Sphere->positionXYZ = glm::vec3(0.0f, -4.0f, 0.0f);
	pPlayer4Sphere->scale = glm::vec3(0.5f);
	pPlayer4Sphere->bDontLight = true;
	pPlayer4Sphere->bUseWholeObjectDiffuseColour = true;
	pPlayer4Sphere->bUseObjectDebugColour = true;
	pPlayer4Sphere->wholeObjectDiffuseRGBA = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
	pPlayer4Sphere->objectDebugColourRGBA = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
	pPlayer4Sphere->alphaTransparency = 1.0f;
	pPlayer4Sphere->clearTextureRatiosToZero();
	pPlayer4Sphere->friendlyName = "Player 4";
	pPlayer4Sphere->friendlyID = 4;
	::g_vec_pMeshes.push_back(pPlayer4Sphere);

	// Forgot the default texture
	ss.str("");
	ss << SOLUTION_DIR << "common\\assets\\textures\\";
	::g_pTextureManager->SetBasePath(ss.str());

	// Default don't have a texture
	if (::g_pTextureManager->Create2DTextureFromBMPFile("BrightColouredUVMap.bmp", true))
		//if (::g_pTextureManager->Create2DTextureFromBMPFile("Pebbleswithquarzite.bmp", true))
	{
		std::cout << "Loaded the texture" << std::endl;
	}
	else
	{
		std::cout << "DIDN'T load the texture" << std::endl;
	}


	// Add a skybox texture
	std::string errorTextString;
	ss.str("");
	ss << SOLUTION_DIR << "common\\assets\\textures\\cubemaps\\";
	::g_pTextureManager->SetBasePath(ss.str());		// update base path to cube texture location

	if (!::g_pTextureManager->CreateCubeTextureFromBMPFiles("TropicalSunnyDay",
		"TropicalSunnyDayRight2048.bmp",	/* +X */	"TropicalSunnyDayLeft2048.bmp" /* -X */,
		"TropicalSunnyDayUp2048.bmp",		/* +Y */	"TropicalSunnyDayDown2048.bmp" /* -Y */,
		"TropicalSunnyDayFront2048.bmp",	/* +Z */	"TropicalSunnyDayBack2048.bmp" /* -Z */,
		true, errorTextString))
	{
		std::cout << "Didn't load because: " << errorTextString << std::endl;
	}


	if (!::g_pTextureManager->CreateCubeTextureFromBMPFiles("Space01",
		"SpaceBox_right1_posX.bmp",		/* +X */	"SpaceBox_left2_negX.bmp" /* -X */,
		"SpaceBox_top3_posY.bmp",		/* +Y */	"SpaceBox_bottom4_negY.bmp" /* -Y */,
		"SpaceBox_front5_posZ.bmp",		/* +Z */	"SpaceBox_back6_negZ.bmp" /* -Z */,
		true, errorTextString))
	{
		std::cout << "Didn't load because: " << errorTextString << std::endl;
	}

	if (!::g_pTextureManager->CreateCubeTextureFromBMPFiles("City01",
		"city_lf.bmp",		/* +X */	"city_rt.bmp" /* -X */,
		"city_dn.bmp",		/* +Y */	"city_up.bmp" /* -Y */,
		"city_ft.bmp",		/* +Z */	"city_bk.bmp" /* -Z */,
		true, errorTextString))
	{
		std::cout << "Didn't load because: " << errorTextString << std::endl;
	}

	if (!::g_pTextureManager->CreateCubeTextureFromBMPFiles("Skyrim",
		"Skyrim_Right.bmp",		/* +X */	"Skyrim_Left.bmp" /* -X */,
		"Skyrim_Top.bmp",		/* +Y */	"Skyrim_Bottom.bmp" /* -Y */,
		"Skyrim_Forward.bmp",		/* +Z */	"Skyrim_Back.bmp" /* -Z */,
		true, errorTextString))
	{
		std::cout << "Didn't load because: " << errorTextString << std::endl;
	}

	// Make sphere for "skybox" before world file
	cMesh* sphereSky = new cMesh();
	//sphereSky->meshName = "Isosphere_Smooth_Normals.ply";
	// We are using a sphere with INWARD facing normals, so we see the "back" of the sphere
	sphereSky->meshName = "Isosphere_Smooth_Inverted_Normals_for_SkyBox.ply";
	// 2 main ways we can do a skybox:
	//
	// - Make a sphere really big, so everything fits inside
	// (be careful of the far clipping plane)
	//
	// - Typical way is:
	//	- Turn off the depth test
	//	- Turn off the depth function (i.e. writing to the depth buffer)
	//	- Draw the skybox object (which can be really small, since it's not interacting with the depth buffer)
	//	- Once drawn:
	//		- Turn on the depth function
	//		- Turn on the depth test

	sphereSky->positionXYZ = ::cameraEye;	//glm::vec3(100.0f, -250.0f, 100.0f);
	//sphereSky->orientationXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
	sphereSky->scale = glm::vec3(10000.0f, 10000.0f, 10000.0f);
	//sphereSky->bUseWholeObjectDiffuseColour = false;
	//sphereSky->wholeObjectDiffuseRGBA = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	sphereSky->bDontLight = true;
	sphereSky->clearTextureRatiosToZero();
	//sphereSky->textureNames[1] = "2k_jupiter.bmp";
	sphereSky->textureRatios[1] = 1.0f;


	::cameraEye = glm::vec3(0.0f, 0.0f, -25.0f);
	::cameraTarget = glm::vec3(-1.f * ::cameraEye.x, 0, -1.f * ::cameraEye.z);
	glm::normalize(::cameraTarget);

	const double MAX_DELTA_TIME = 0.1;	//100 ms
	double previousTime = glfwGetTime();

	while (!glfwWindowShouldClose(pWindow))
	{
		float ratio;
		int width, height;
		glm::mat4 matModel;				// used to be "m"; Sometimes it's called "world"
		glm::mat4 matProjection;        // used to be "p";
		glm::mat4 matView;              // used to be "v";

		double currentTime = glfwGetTime();
		deltaTime = currentTime - previousTime;
		deltaTime = (deltaTime > MAX_DELTA_TIME ? MAX_DELTA_TIME : deltaTime);
		previousTime = currentTime;

		glfwGetFramebufferSize(pWindow, &width, &height);
		ratio = width / (float)height;

		//Turn on the depth buffer
		glEnable(GL_DEPTH);         // Turns on the depth buffer
		glEnable(GL_DEPTH_TEST);    // Check if the pixel is already closer

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// *******************************************************
		// Screen is cleared and we are ready to draw the scene...
		// *******************************************************

		// Update the title text
		glfwSetWindowTitle(pWindow, ::g_TitleText.c_str());



		// Copy the light information into the shader to draw the scene
		::g_pTheLights->CopyLightInfoToShader();

		/*matProjection = glm::perspective(0.6f,
			ratio,
			0.1f,
			1000.0f);*/

		matProjection = glm::perspective(0.6f,	// FOV variable later
			ratio,
			0.1f,								// Near plane
			1'000'000.0f);						// Far plane



		matView = glm::mat4(1.0f);
		//matView = glm::lookAt(	cameraEye,   // "eye"
		//						cameraAt,    // "at"
		//						cameraUp);

		matView = glm::lookAt(::cameraEye,
			::cameraEye + ::cameraTarget,
			upVector);

		//	glUniformMatrix4fv(matView_Location, 1, GL_FALSE, glm::value_ptr(matView));

		glUniformMatrix4fv(pShaderProc->getUniformID_From_Name("matView"),
			1, GL_FALSE, glm::value_ptr(matView));


		glUniformMatrix4fv(matProjection_Location, 1, GL_FALSE, glm::value_ptr(matProjection));


		// Since this is a space game (most of the screen is filled with "sky"),
		// I'll draw the skybox first

		GLint bIsSkyBox_LocID = glGetUniformLocation(program, "bIsSkyBox");
		glUniform1f(bIsSkyBox_LocID, (GLfloat)GL_TRUE);

		// Move the "skybox object" with the camera
		sphereSky->positionXYZ = ::cameraEye;
		DrawObject(sphereSky, glm::mat4(1.0f),
			matModel_Location, matModelInverseTranspose_Location,
			program, ::g_pVAOManager);

		glUniform1f(bIsSkyBox_LocID, (GLfloat)GL_FALSE);

		// If there were transparent objects, this is where I would sort them

		// Before drawing the objects, let's update the client  
		client.Update();

		// **********************************************************************
		// Draw the "scene" of all objects.
		// i.e. go through the vector and draw each one...
		// **********************************************************************
		for (unsigned int index = 0; index != ::g_vec_pMeshes.size(); index++)
		{
			// So the code is a little easier...
			cMesh* pCurrentMesh = ::g_vec_pMeshes[index];

			if (::deadReckoningOn && pCurrentMesh->friendlyID == 0)
			{
				// Predict the position of the ball using it's velocity
				pCurrentMesh->positionXYZ += ::ballVelocity / (float)deltaTime;
			}

			// Bounding
			if (pCurrentMesh->getUniqueID() != 0)
			{
				if (pCurrentMesh->positionXYZ.x < -9.0f)
				{
					pCurrentMesh->positionXYZ.x = -9.0f;
				}
				else if (pCurrentMesh->positionXYZ.x > 9.0f)
				{
					pCurrentMesh->positionXYZ.x = 9.0f;
				}

				if (pCurrentMesh->positionXYZ.y > 4.0f)
				{
					pCurrentMesh->positionXYZ.y = 4.0f;
				}
				else if (pCurrentMesh->positionXYZ.y < -4.0f)
				{
					pCurrentMesh->positionXYZ.y = -4.0f;
				}
			}
			else
			{
				if (pCurrentMesh->positionXYZ.x < -10.0f)
				{
					pCurrentMesh->positionXYZ.x = -10.0f;
				}
				else if (pCurrentMesh->positionXYZ.x > 10.0f)
				{
					pCurrentMesh->positionXYZ.x = 10.0f;
				}

				if (pCurrentMesh->positionXYZ.y > 5.0f)
				{
					pCurrentMesh->positionXYZ.y = 5.0f;
				}
				else if (pCurrentMesh->positionXYZ.y < -5.0f)
				{
					pCurrentMesh->positionXYZ.y = -5.0f;
				}
			}
			

			if (pCurrentMesh->positionXYZ.z > 0.0f || pCurrentMesh->positionXYZ.z < 0.0f)
			{
				pCurrentMesh->positionXYZ.z = 0.0f;
			}

			matModel = glm::mat4(1.0f);  // "Identity" ("do nothing", like x1)
			//mat4x4_identity(m);

			DrawObject(pCurrentMesh,
				matModel,
				matModel_Location,
				matModelInverseTranspose_Location,
				program,
				::g_pVAOManager);

		}//for (unsigned int index
		// Scene is drawn
		// **********************************************************************
		// "Present" what we've drawn.
		glfwSwapBuffers(pWindow);        // Show what we've drawn

		// Process any events that have happened
		glfwPollEvents();

		// Handle OUR keyboard, mouse stuff
		handleAsyncKeyboard(pWindow, deltaTime);
		handleAsyncMouse(pWindow, deltaTime);
	}//while (!glfwWindowShouldClose(window))

	// All done, so delete things...
	::g_ShutDown(pWindow);


	glfwDestroyWindow(pWindow);

	glfwTerminate();
	exit(EXIT_SUCCESS);

	

	system("Pause");
}