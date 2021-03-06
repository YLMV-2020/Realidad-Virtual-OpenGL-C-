#ifndef APPLICATION_ATTRIBUTES_HEADER
#define APPLICATION_ATTRIBUTES_HEADER

#define MIRROR_ALLOW_OVR true

const int WIDTH = 1280;
const int HEIGHT = 800;

#include <functional>
#include <GL/glew.h> 

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <irrKlang.h>
using namespace irrklang;

#include <iostream>
using namespace std;

//#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Shader.h"
#include "Util.h"
#include "BulletWorldController.h"
#include "Camera.h"

#include "Avatar.h"
#include "Terrain.h"
#include "Skybox.h"
#include "DebugDrawer.h"
#include "AnimationController.h"
#include "ModelController.h"
#include "Tablet.h"
#include "PokebolaController.h"
#include "AvatarController.h"
#include "PlayerController.h"
#include "InputProcessor.h"
#include "Lighting.h"
#include "Player.h"
#include "Text.h"
#include "NPC.h"

using namespace Cogravi;

namespace Cogravi
{
	class ApplicationAttributes
	{
	public:

        GLFWwindow* window;

        bool isEngine = true;
        bool isVr = false;

        ImVec2 mouse;

        float deltaTime;
        float lastFrame;

        GLuint framebufferEngine;
        GLuint textureEngine;

        GLuint framebufferTablet;
        GLuint textureTablet;

        GLuint framebufferVR;
        GLuint textureVR;

        long long frameIndex = 0;
        GLint lastFont = 0;

        ImVec2 displayRender;
        ImVec2 display;

        Camera* camera;
        vector<GLuint> textureFloors;

        vector<GLuint> textureSkyboxs;
        vector<GLuint> textureObjects;

        vector<GLuint> texturesImGui;
        Text* text;

        Terrain* terrain;
        Skybox* skybox;

        BulletWorldController* bulletWorldController;
        DebugDrawer* debugDrawer;

        Util* util;
        InputProcessor* input;

        ModelController* modelController;
        AnimationController* animationController;

        PokebolaController* pokebolaController;

        Player* player;
        Tablet* tablet;

        Shader* shaderModel;
        Shader* shaderAnimation;

        Shader* shaderInstance;
        Shader* shaderInstanceDynamic;

        Lighting* luz;
        Shader* shaderSol;

        NPC* NPC1;
        ISoundEngine* SoundEngine;

        bool isDrawWireframe = true;
        bool isDrawAABB = false;
        bool isWireframe = false;

        Model* modelSelect = NULL;

        //VR
        int WIDTH_VR = 1920;
        int HEIGHT_VR = 986;

        Avatar* avatar;
        AvatarController* avatarController;
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