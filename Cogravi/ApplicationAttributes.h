#ifndef APPLICATION_ATTRIBUTES_HEADER
#define APPLICATION_ATTRIBUTES_HEADER

#define MIRROR_ALLOW_OVR true

const int WIDTH = 1280;
const int HEIGHT = 800;

float NEAR = 0.1f;
float FAR = 1000.0f;

#include <functional>
#include <GL/glew.h> 

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Util/Shader.h>

#include <iostream>
using namespace std;

//#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Util.h"
#include "BulletWorldController.h"
#include "Camera.h"

#include "Avatar.h"
#include "Terrain.h"
#include "Skybox.h"
#include "DebugDrawer.h"
#include "PlayerCamera.h"
#include "ModelController.h"
#include "AnimationController.h"
#include "PlayerController.h"
#include "VehicleController.h"
#include "InputProcessor.h"


using namespace Cogravi;

namespace Cogravi
{
	class ApplicationAttributes
	{
	public:

        GLFWwindow* window;

        float lastX;
        float lastY;

        bool mouseCursorDisabled = true;
        bool firstMouse = true;

        bool isEngine = true;
        bool isPc = false;
        bool isVr = false;

        float dx, dy;

        float startFrame;
        float deltaTime;

        float lastFrame;
        float animationTime;

        unsigned int framebufferEngine;
        unsigned int textureEngine;

        long long frameIndex = 0;

        ImVec2 displayRender;
        ImVec2 display;

        Camera* camera;
        vector<GLuint> textures;

        Terrain* terrain;
        Skybox* skybox;

        BulletWorldController* bulletWorldController;
        DebugDrawer* debugDrawer;

        Util* util;
        PlayerController* player;

        ModelController* models;
        AnimationController* animations;

        VehicleController* vehicle;

        //VR
        int WIDTH_VR = 1920;
        int HEIGHT_VR = 986;

        Avatar* avatar;
        ovrSession ovr;

        ovrMicrophoneHandle mic;
        ovrHmdDesc hmdDesc;

        GLuint mirrorFBO = 0;
        ovrTextureSwapChain eyeSwapChains[2];
        GLuint eyeFrameBuffers[2];
        GLuint eyeDepthBuffers[2];
        ovrSizei eyeSizes[2];

        std::function<void(int width, int height)> redimensionar;
        std::function<void(int key, int scancode, int action, int mods)> teclado;
        std::function<void(double xpos, double ypos)> mousePos;
        std::function<void(double xoffset, double yoffset)> desplazar;
        std::function<void(int button, int action, int mods)> mouseButton;


	};
}
#endif