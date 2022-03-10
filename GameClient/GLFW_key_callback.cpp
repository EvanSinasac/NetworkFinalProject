#include "globalThings.h"
//Stuff done in class with Michael Feeney

#include <sstream>
#include <iostream>


// Turns off the: warning C4005: 'APIENTRY': macro redefinition
#pragma warning( disable: 4005)
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

/*static*/ void GLFW_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }


    // From the help review session.
    // Move the camera to a location and look somewhere
    //if (key == GLFW_KEY_L && action == GLFW_PRESS)
    //{
    //    // Move the eye
    //    ::g_pFlyCamera->setEye(glm::vec3(8631.0f, -1487.0f, 13010.0f));

    //    // "look at" something.
    //    // Since we are using the fly camera, we can't just set the eye.
    //    //::g_pFlyCamera->setAt(glm::vec3(0.0f, 0.0f, 0.0f));

    //    ::g_pFlyCamera->Yaw_LeftRight(-80.0f);
    //    ::g_pFlyCamera->Pitch_UpDown(-10.0f);

    //}

    float cameraSpeed = 1.0f;
    float objectMovementSpeed = 1.0f;
    float lightMovementSpeed = 1.0f;

    bool bShiftDown = false;
    bool bControlDown = false;
    bool bAltDown = false;

    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
    {
        ::client.UPDATES_PER_SEC = 1;
        printf("Client Update Rate: %f\n", ::client.UPDATES_PER_SEC);
    }
    if (key == GLFW_KEY_2 && action == GLFW_PRESS)
    {
        ::client.UPDATES_PER_SEC = 10;
        printf("Client Update Rate: %f\n", ::client.UPDATES_PER_SEC);
    }
    if (key == GLFW_KEY_3 && action == GLFW_PRESS)
    {
        ::client.UPDATES_PER_SEC = 60;
        printf("Client Update Rate: %f\n", ::client.UPDATES_PER_SEC);
    }
    if (key == GLFW_KEY_4 && action == GLFW_PRESS)  // Ahh, this was a 3... whoops lol
    {
        ::client.UPDATES_PER_SEC = 100;
        printf("Client Update Rate: %f\n", ::client.UPDATES_PER_SEC);
    }

    if (key == GLFW_KEY_5 && action == GLFW_PRESS)
    {
        ::clientSidePredictionOn = !::clientSidePredictionOn;
        printf("Client side prediction is: %s\n", ((clientSidePredictionOn) ? "ON" : "OFF"));
    }
    if (key == GLFW_KEY_6 && action == GLFW_PRESS)
    {
        ::deadReckoningOn = !deadReckoningOn;
        printf("Dead reconing is: %s\n", ((deadReckoningOn) ? "ON" : "OFF"));
    }
  

    //    // Shift down?
    //    if ( mods == GLFW_MOD_SHIFT )       // 0x0001   0000 0001
    //    {
    //        // ONLY shift is down
    //    }
    //    // Control down?
    //    if ( mods == GLFW_MOD_CONTROL  )    // 0x0002   0000 0010
    //    // Alt down?
    //    if ( mods == GLFW_MOD_ALT   )       // 0x0004   0000 0100

        //   0000 0111 
        // & 0000 0001
        // -----------
        //   0000 0001 --> Same as the shift mask

        // Use bitwise mask to filter out just the shift
    if ((mods & GLFW_MOD_SHIFT) == GLFW_MOD_SHIFT)
    {
        // Shift is down and maybe other things, too
        bShiftDown = true;
    }
    if ((mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL)
    {
        // Shift is down and maybe other things, too
        bControlDown = true;
    }
    if ((mods & GLFW_MOD_ALT) == GLFW_MOD_ALT)
    {
        // Shift is down and maybe other things, too
        bAltDown = true;
    }


    //   // If JUST the shift is down, move the "selected" object
       if ( bShiftDown && (!bControlDown) && (!bAltDown) )
       {
           
    
    
       }//if ( bShiftDown && ( ! bControlDown ) && ( ! bAltDown ) )

       // If JUST the ALT is down, move the "selected" light
    if ((!bShiftDown) && (!bControlDown) && bAltDown)
    {
        

    }//if ( bShiftDown && ( ! bControlDown ) && ( ! bAltDown ) )

    return;
}
