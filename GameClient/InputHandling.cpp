//Stuff done in class with Michael Feeney
#include "GLCommon.h" 
#include "globalThings.h"

#include <sstream>
#include <iostream>

// Handle async IO (keyboard, joystick, mouse, etc.)


// This is so the "fly camera" won't pay attention to the mouse if it's 
// not directly over the window. 
bool g_MouseIsInsideWindow = false;



void handleAsyncKeyboard(GLFWwindow* pWindow, double deltaTime)
{
   // float cameraMoveSpeed = 5.0f * deltaTime;   //20.0f for space scene
    float lightMoveSpeed = 5.0f * deltaTime;
    float playerMoveSpeed = 5.0f * deltaTime;

    if (cGFLWKeyboardModifiers::areAllModsUp(pWindow))
    {
        
        // Depending on the player number determines which mesh the player can control

        if (glfwGetKey(pWindow, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            // Players 3 and 4 can move left and right
            if (::playerNumber == 3 || ::playerNumber == 4)
            {
                if (::clientSidePredictionOn)
                {
                    ::g_vec_pMeshes[::playerNumber]->positionXYZ.x += playerMoveSpeed;
                }
                else
                {
                    client.SendNonPredictedPosition(glm::vec3(playerMoveSpeed, 0.0f, 0.0f));
                }
                if (::g_vec_pMeshes[::playerNumber]->positionXYZ.x > 9.0f)
                {
                    ::g_vec_pMeshes[::playerNumber]->positionXYZ.x = 9.0f;
                }
            }
        }
        if (glfwGetKey(pWindow, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            // Players 3 and 4 can move left and right
            if (::playerNumber == 3 || ::playerNumber == 4)
            {
                if (::clientSidePredictionOn)
                {
                    ::g_vec_pMeshes[::playerNumber]->positionXYZ.x -= playerMoveSpeed;
                }
                else
                {
                    client.SendNonPredictedPosition(glm::vec3(-playerMoveSpeed, 0.0f, 0.0f));
                }
                if (::g_vec_pMeshes[::playerNumber]->positionXYZ.x < -9.0f)
                {
                    ::g_vec_pMeshes[::playerNumber]->positionXYZ.x = -9.0f;
                }
            }
        }
        if (glfwGetKey(pWindow, GLFW_KEY_UP) == GLFW_PRESS)
        {
            // Players 1 and 2 move up and down
            if (::playerNumber == 1 || ::playerNumber == 2)
            {
                if (::clientSidePredictionOn)
                {
                    ::g_vec_pMeshes[::playerNumber]->positionXYZ.y += playerMoveSpeed;
                }
                else
                {
                    client.SendNonPredictedPosition(glm::vec3(0.0f, playerMoveSpeed, 0.0f));
                }
                
                if (::g_vec_pMeshes[::playerNumber]->positionXYZ.y > 4.0f)
                {
                    ::g_vec_pMeshes[::playerNumber]->positionXYZ.y = 4.0f;
                }
            }
        }
        if (glfwGetKey(pWindow, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            // Players 1 and 2 move up and down
            if (::playerNumber == 1 || ::playerNumber == 2)
            {
                if (::clientSidePredictionOn)
                {
                    ::g_vec_pMeshes[::playerNumber]->positionXYZ.y -= playerMoveSpeed;
                }
                else
                {
                    client.SendNonPredictedPosition(glm::vec3(0.0f, -playerMoveSpeed, 0.0f));
                }
                
                if (::g_vec_pMeshes[::playerNumber]->positionXYZ.y < -4.0f)
                {
                    ::g_vec_pMeshes[::playerNumber]->positionXYZ.y = -4.0f;
                }
            }
        }



        std::stringstream strTitle;
        // std::cout << 
        //glm::vec3 cameraEye = ::g_pFlyCamera->getEye();
        if (::playerNumber > 0)
        {
            strTitle << "Networking Pong!  Player " << ::playerNumber << " is at : "
                << ::g_vec_pMeshes[::playerNumber]->positionXYZ.x << ", "
                << ::g_vec_pMeshes[::playerNumber]->positionXYZ.y << ", "
                << ::g_vec_pMeshes[::playerNumber]->positionXYZ.z; //<< std::endl;
        }
        else
        {
            strTitle << "No player Number yet!";
        }
        

        ::g_TitleText = strTitle.str();

    }//if ( cGFLWKeyboardModifiers::areAllModsUp(pWindow) )


    // If JUST the shift is down, move the "selected" object
    if (cGFLWKeyboardModifiers::isModifierDown(pWindow, true, false, false))
    {
        // Move the spotlight up and down
        if (glfwGetKey(pWindow, GLFW_KEY_UP) == GLFW_PRESS) { ::g_pTheLights->theLights[::g_selectedLight].position.y += lightMoveSpeed; }
        if (glfwGetKey(pWindow, GLFW_KEY_DOWN) == GLFW_PRESS) { ::g_pTheLights->theLights[::g_selectedLight].position.y -= lightMoveSpeed; }
    }//if ( cGFLWKeyboardModifiers::...


    // If JUST the CTRL is down, move the "selected" light
    if (cGFLWKeyboardModifiers::isModifierDown(pWindow, false, true, false))
    {
        // Move the spotlight around
        if (glfwGetKey(pWindow, GLFW_KEY_UP) == GLFW_PRESS) { ::g_pTheLights->theLights[::g_selectedLight].position.z += lightMoveSpeed; }
        if (glfwGetKey(pWindow, GLFW_KEY_DOWN) == GLFW_PRESS) { ::g_pTheLights->theLights[::g_selectedLight].position.z -= lightMoveSpeed; }
        if (glfwGetKey(pWindow, GLFW_KEY_LEFT) == GLFW_PRESS) { ::g_pTheLights->theLights[::g_selectedLight].position.x += lightMoveSpeed; }
        if (glfwGetKey(pWindow, GLFW_KEY_RIGHT) == GLFW_PRESS) { ::g_pTheLights->theLights[::g_selectedLight].position.x -= lightMoveSpeed; }
    }//if ( cGFLWKeyboardModifiers::...

    return;
}


// We call these every frame
void handleAsyncMouse(GLFWwindow* window, double deltaTime)
{

    double x, y;
    glfwGetCursorPos(window, &x, &y);

    //::g_pFlyCamera->setMouseXY(x, y);

    const float MOUSE_SENSITIVITY = 2.0f;


    // Mouse left (primary?) button pressed? 
    // AND the mouse is inside the window...
    //if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    //    && ::g_MouseIsInsideWindow)
    //{
    //    // Mouse button is down so turn the camera
    //    ::g_pFlyCamera->Yaw_LeftRight(::g_pFlyCamera->getDeltaMouseX() * MOUSE_SENSITIVITY, deltaTime);

    //    ::g_pFlyCamera->Pitch_UpDown(-::g_pFlyCamera->getDeltaMouseY() * MOUSE_SENSITIVITY, deltaTime);

    //}

    // Adjust the mouse speed
    if (::g_MouseIsInsideWindow)
    {
        const float MOUSE_WHEEL_SENSITIVITY = 0.1f;

        // Adjust the movement speed based on the wheel position
        //::g_pFlyCamera->movementSpeed -= (::g_pFlyCamera->getMouseWheel() * MOUSE_WHEEL_SENSITIVITY);

        //// Clear the mouse wheel delta (or it will increase constantly)
        //::g_pFlyCamera->clearMouseWheelValue();


        //if (::g_pFlyCamera->movementSpeed <= 0.0f)
        //{
        //    ::g_pFlyCamera->movementSpeed = 0.0f;
        //}
    }


    return;
}

void GLFW_cursor_enter_callback(GLFWwindow* window, int entered)
{
    if (entered)
    {
        //std::cout << "Mouse cursor is over the window" << std::endl;
        ::g_MouseIsInsideWindow = true;
    }
    else
    {
       // std::cout << "Mouse cursor is no longer over the window" << std::endl;
        ::g_MouseIsInsideWindow = false;
    }
    return;
}

// Called when the mouse scroll wheel is moved
void GLFW_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    float mouseScrollWheelSensitivity = 0.1f;

    //::g_pFlyCamera->setMouseWheelDelta(yoffset * mouseScrollWheelSensitivity);

    return;
}

void GLFW_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{

//#ifdef YO_NERDS_WE_USING_WINDOWS_CONTEXT_MENUS_IN_THIS_THANG
//    // Right button is pop-up
//    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
//    {
//        ShowWindowsContextMenu(window, button, action, mods);
//    }
//#endif

    return;
}


void GLFW_cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    return;
}


