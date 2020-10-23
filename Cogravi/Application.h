#ifndef APPLICATION_HEADER
#define APPLICATION_HEADER

#include "ApplicationAttributes.h"

namespace Cogravi
{
    class Application :public ApplicationAttributes
    {
    public:

        Shader* shaderModel;
        Shader* shaderAnimation;
        Shader* shaderInstance;
        Shader* shaderInstanceDynamic;
        GameObject* model;
        GameObject* model1;


        //vector<Skeletal*> skeletal;
       DynamicGameObject* animation;
        //DynamicGameObject* animation;

        static Application* Instance()
        {
            static Application instance;
            return &instance;
        }

        Application()
        {

            if (glfwInit() == GL_FALSE) return;

            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
            //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

            window = glfwCreateWindow(WIDTH, HEIGHT, "Computacion Grafica y Visual", NULL, NULL);
            if (!window) return;

            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            glfwMakeContextCurrent(window);

            if (GLenum err = glewInit()) return;

            glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
                auto w = (Application*)glfwGetWindowUserPointer(window); if (w->redimensionar) w->redimensionar(width, height);
            });

            glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
                auto w = (Application*)glfwGetWindowUserPointer(window); if (w->teclado) w->teclado(key, scancode, action, mods);
            });

            glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
                auto w = (Application*)glfwGetWindowUserPointer(window); if (w->mousePos) w->mousePos(xpos, ypos);
            });

            glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
                auto w = (Application*)glfwGetWindowUserPointer(window); if (w->desplazar) w->desplazar(xoffset, yoffset);
            });

            glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
                auto w = (Application*)glfwGetWindowUserPointer(window); if (w->mouseButton) w->mouseButton(button, action, mods);
            });

            glfwSetWindowUserPointer(window, this);
            inicializarEventos();

            glfwSwapInterval(0);
        }

        ~Application()
        {
            glfwMakeContextCurrent(window);
            glfwDestroyWindow(window);
            glfwTerminate();
        }

        void inicializarEventos()
        {
            teclado = [&](int key, int sancode, int action, int mods)
            {
                if (key == GLFW_KEY_G)
                {
                    models->getModelPhysics(0)->body->applyCentralImpulse(btVector3(0, 20, 0));
                }

                if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                {
                    isEngine = true;
                    isVr = false;
                    isPc = false;
                    glViewport(0, 0, WIDTH, HEIGHT);
                    debugDrawer->ToggleDebugFlag(btIDebugDraw::DBG_DrawWireframe);
                }

                if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
                {
                    switch (camera->getMode())
                    {
                    case CameraType::FIRST_PERSON:
                        camera->setMode(CameraType::THIRD_PERSON);
                        break;
                    case CameraType::THIRD_PERSON:
                        camera->setMode(CameraType::FIRST_PERSON);
                    }
                }

                if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
                {
                    if (!input->isWireframe)
                    {
                        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

                    }
                    else
                    {
                        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    }
                    input->isWireframe = !input->isWireframe;
                }

            };

            redimensionar = [&](int width, int height)
            {
                if (isEngine)
                    glViewport(0, 0, WIDTH, HEIGHT);
                else if (isPc || isVr)
                    glViewport(0, 0, width, height);
            };

            mousePos = [&](double xpos, double ypos)
            {
                input->processMouse(xpos, ypos);
            };

            desplazar = [&](float xoffset, float yoffset)
            {
                //camera->ProcessMouseScroll(yoffset);
            };

            mouseButton = [&](int button, int action, int mods)
            {
                if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
                {
                    if (!input->mouseCursorDisabled)
                        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    else
                        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

                    input->mouseCursorDisabled = !input->mouseCursorDisabled;

                    if (input->mouseCursorDisabled)
                        input->firstMouse = true;
                }

                if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
                {

                }
            };

        }

        

        void addModels()
        {
            vector<Texture> g;
            Texture sd = Texture(Util::loadTexture("assets/objects/arbol/arbol.tga"), TextureType::DIFFUSE);
            g.push_back(sd);

            //Texture tx(Util::)
            //animation = new DynamicGameObject(glm::vec3(0.0f, 0.0f, 20.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.02f, 0.02f, 0.02f), "assets/animations/player/player.dae", *shaderAnimation);

                animation=new DynamicGameObject(glm::vec3(10.0f * ( 1), 0.0f, 20.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.03f, 0.03f, 0.03f), "assets/animations/player/player.dae", *shaderInstanceDynamic);
                animation->addAnimation("Hip Hop Dancing.dae");
                animation->addAnimation("Zombie Walk.dae");
                animation->configureInstance();
                //animation.back()->addAnimation("Victory Idle.dae");
          
            //animation = new DynamicGameObject(glm::vec3(10.0f, 0.0f, 20.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.04f, 0.04f, 0.04f), "assets/animations/Hip Hop Dancing.fbx", *shaderAnimation, g);
            //models->addModel(glm::vec3(20.0f, 0.0f, 20.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10.f, 10.f, 10.f), "assets/objects/aula/aula.obj", *util->myShaders[ShaderType::MODEL_STATIC], bulletWorldController);
            //models->addModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.f, 1.f, 1.f), "assets/objects/mesa/mesa.obj", *util->myShaders[ShaderType::MODEL_STATIC], bulletWorldController);

            //models->addModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.f, 1.f, 1.f), "assets/objects/mesa/mesa.obj", *util->myShaders[ShaderType::MODEL_STATIC], bulletWorldController);

            models->addModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10.f, 10.f, 10.f), "assets/objects/tunel/tunel.obj");
            models->addModel(glm::vec3(30.0f, 3.50f, 20.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.1f, 0.1f, 0.1f), "assets/objects/tierra/universe-m.3ds", ColliderType::SPHERE, bulletWorldController);
            models->addModel(glm::vec3(-30.0f, 0.00f, 40.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.1f, 1.1f, 1.1f), "assets/objects/mirana/mirana.obj");
            models->addModel(glm::vec3(-10.0f, 10.00f, 50.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.1f, 0.1f, 0.1f), "assets/animations/helicoptero/camcopters100.obj");
            model = new GameObject(glm::vec3(0.0f, 0.00f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "assets/objects/arbol/arbol.obj",g);
            model->configureInstance(); 
            model1 = new GameObject(glm::vec3(0.0f, 0.00f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "assets/objects/rock/rock.obj");
            model1->configureInstance();
            //models->addModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10.f, 10.f, 10.f), "assets/objects/tunel/tunel.obj", ColliderType::BOX, bulletWorldController);

        }

        void addAnimations()
        {

        }

        void loadTexturesImGui()
        {
            texturesImGui.push_back(Util::loadTexture("assets\\textures\\awesomeface.png"));
        }

        void loadTextureObjects()
        {
            textureObjects.push_back(Util::loadTexture("assets\\textures\\tienda.png"));
        }

        void loadTextureSkyboxs()
        {
            textureSkyboxs.push_back(Util::loadTexture("assets\\skyboxs\\nubes\\nubes_nx.jpg"));
            textureSkyboxs.push_back(Util::loadTexture("assets\\skyboxs\\blue\\blue_nx.png"));
            textureSkyboxs.push_back(Util::loadTexture("assets\\skyboxs\\rainbow\\rainbow_nx.png"));
            textureSkyboxs.push_back(Util::loadTexture("assets\\skyboxs\\Lan\\Lan_nx.jpg"));
            textureSkyboxs.push_back(Util::loadTexture("assets\\skyboxs\\bosque\\bosque_nx.png"));
        }

        void loadTextureFloors()
        {
            textureFloors.push_back(Util::loadTexture("assets\\textures\\wall.jpg"));
        }
        Aula* aula;
        void inicializarScene()
        {
            util = Util::Instance();
            bulletWorldController = new BulletWorldController();

            avatar = new Avatar(bulletWorldController);
            aula = new Aula(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.f, 1.f, 1.f), "assets/objects/aula/aula.obj", bulletWorldController);

            terrain = new Terrain("assets\\textures\\marble.jpg", glm::vec3(100, -0.01f, 100), 10.0f, *util->myShaders[ShaderType::TERRAIN], bulletWorldController);
            skybox = new Skybox("bosque", "png", *util->myShaders[ShaderType::CUBE_MAP], *util->myShaders[ShaderType::SKYBOX]);

            debugDrawer = new DebugDrawer();
            debugDrawer->setDebugMode(0);

            bulletWorldController->dynamicsWorld->setDebugDrawer(debugDrawer);
            debugDrawer->ToggleDebugFlag(btIDebugDraw::DBG_DrawWireframe);

            camera = new Camera(NULL);

            luz = Lighting::Instance();

            input = new InputProcessor(window, camera);

            models = new ModelController();

            loadTextureFloors();
            loadTextureObjects();
            loadTexturesImGui();
            loadTextureSkyboxs();

            shaderModel = util->myShaders[ShaderType::MODEL_STATIC];
            shaderAnimation = util->myShaders[ShaderType::MODEL_DYNAMIC];
            shaderInstance = util->myShaders[ShaderType::INSTANCE];
            shaderInstanceDynamic = util->myShaders[ShaderType::INSTANCE_DYNAMIC];

            addModels();
            addAnimations();

        }

        void inicializarImgui()
        {
            const char* glsl_version = "#version 460";
            // Setup Dear ImGui context
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
            //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
            //io.ConfigViewportsNoAutoMerge = true;
            //io.ConfigViewportsNoTaskBarIcon = true;

            // Setup Dear ImGui style
            ImGui::StyleColorsDark();

            // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
            ImGuiStyle& style = ImGui::GetStyle();
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                style.WindowRounding = 0.0f;
                style.Colors[ImGuiCol_WindowBg].w = 1.0f;
            }

            ImGuiWindowFlags window_flags = 0;
            ImGui_ImplGlfw_InitForOpenGL(window, true);
            ImGui_ImplOpenGL3_Init(glsl_version);
        }

        void inicializarVR()
        {
            ovr = MIRROR_ALLOW_OVR ? _initOVR() : 0;

            mic = ovr_Microphone_Create();
            if (mic)
            {
                ovr_Microphone_Start(mic);
            }

            if (ovr)
            {
                // Get the buffer size we need for rendering
                hmdDesc = ovr_GetHmdDesc(ovr);
                for (int eye = 0; eye < 2; ++eye)
                {
                    eyeSizes[eye] = ovr_GetFovTextureSize(ovr, (ovrEyeType)eye, hmdDesc.DefaultEyeFov[eye], 1.0f);

                    // Create the swap chain
                    ovrTextureSwapChainDesc desc;
                    memset(&desc, 0, sizeof(desc));
                    desc.Type = ovrTexture_2D;
                    desc.ArraySize = 1;
                    desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
                    desc.Width = eyeSizes[eye].w;
                    desc.Height = eyeSizes[eye].h;
                    desc.MipLevels = 1;
                    desc.SampleCount = 1;
                    desc.StaticImage = ovrFalse;
                    ovr_CreateTextureSwapChainGL(ovr, &desc, &eyeSwapChains[eye]);

                    int length = 0;
                    ovr_GetTextureSwapChainLength(ovr, eyeSwapChains[eye], &length);
                    for (int i = 0; i < length; ++i)
                    {
                        GLuint chainTexId;
                        ovr_GetTextureSwapChainBufferGL(ovr, eyeSwapChains[eye], i, &chainTexId);
                        glBindTexture(GL_TEXTURE_2D, chainTexId);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    }
                    glGenFramebuffers(1, &eyeFrameBuffers[eye]);

                    glGenTextures(1, &eyeDepthBuffers[eye]);
                    glBindTexture(GL_TEXTURE_2D, eyeDepthBuffers[eye]);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, eyeSizes[eye].w, eyeSizes[eye].h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
                }

                // Create mirror buffer
                ovrMirrorTextureDesc mirrorDesc;
                memset(&mirrorDesc, 0, sizeof(mirrorDesc));
                mirrorDesc.Width = WIDTH_VR;
                mirrorDesc.Height = HEIGHT_VR;
                mirrorDesc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;

                ovrMirrorTexture mirrorTexture;
                ovr_CreateMirrorTextureGL(ovr, &mirrorDesc, &mirrorTexture);

                GLuint mirrorTextureID;
                ovr_GetMirrorTextureBufferGL(ovr, mirrorTexture, &mirrorTextureID);

                glGenFramebuffers(1, &mirrorFBO);
                glBindFramebuffer(GL_READ_FRAMEBUFFER, mirrorFBO);
                glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mirrorTextureID, 0);
                glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
                glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
            }

            ovrAvatar_Initialize(MIRROR_SAMPLE_APP_ID);

            ovrID userID = ovr_GetLoggedInUserID();
            ovrAvatar_RequestAvatarSpecification(userID);

            if (ovr)
            {
                ovr_SetTrackingOriginType(ovr, ovrTrackingOrigin_FloorLevel);
                ovr_RecenterTrackingOrigin(ovr);
            }

        }

        void renderInicializar()
        {
            inicializarImgui();
            inicializarScene();
            /* inicializarVR();

             avatar->Init();*/
            vincularFrambufferEngine();
            startFrame = glfwGetTime();
        }

        void renderPc()
        {
            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glm::mat4 ProjectionMatrix = camera->GetProjectionMatrix();
            glm::mat4 ViewMatrix = camera->GetViewMatrix();

            bulletWorldController->physics_step(60.0f);


            /* skybox->render(*camera);
             terrain->render(*camera);*/
             /*models->render(*camera);
             animations->render(*camera, animationTime);*/

            debugDrawer->SetMatrices(ViewMatrix, ProjectionMatrix);
            bulletWorldController->dynamicsWorld->debugDrawWorld();
            debugDrawer->col = glm::vec3(0, 1, 0);

        }

        void renderRV()
        {
            if (ovr)
            {

                // Call ovr_GetRenderDesc each frame to get the ovrEyeRenderDesc, as the returned values (e.g. HmdToEyeOffset) may change at runtime.
                ovrEyeRenderDesc eyeRenderDesc[2];
                eyeRenderDesc[0] = ovr_GetRenderDesc(ovr, ovrEye_Left, hmdDesc.DefaultEyeFov[0]);
                eyeRenderDesc[1] = ovr_GetRenderDesc(ovr, ovrEye_Right, hmdDesc.DefaultEyeFov[1]);

                // Get eye poses, feeding in correct IPD offset
                ovrPosef                  eyeRenderPose[2];
                ovrVector3f               hmdToEyeOffset[2] = { eyeRenderDesc[0].HmdToEyeOffset, eyeRenderDesc[1].HmdToEyeOffset };
                double sensorSampleTime;
                ovr_GetEyePoses(ovr, frameIndex, ovrTrue, hmdToEyeOffset, eyeRenderPose, &sensorSampleTime);
                // If the avatar is initialized, update it

                // PRERENDER
                avatar->Prerender(ovr);
                bulletWorldController->physics_step(60.f);

                avatar->Update();
                //animations->update();
                models->update();

                // Render each eye
                for (int eye = 0; eye < 2; ++eye)
                {
                    // Switch to eye render target
                    int curIndex;
                    GLuint curTexId;
                    ovr_GetTextureSwapChainCurrentIndex(ovr, eyeSwapChains[eye], &curIndex);
                    ovr_GetTextureSwapChainBufferGL(ovr, eyeSwapChains[eye], curIndex, &curTexId);

                    glBindFramebuffer(GL_FRAMEBUFFER, eyeFrameBuffers[eye]);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, curTexId, 0);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, eyeDepthBuffers[eye], 0);

                    glViewport(0, 0, eyeSizes[eye].w, eyeSizes[eye].h);
                    glDepthMask(GL_TRUE);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
                    //glEnable(GL_FRAMEBUFFER_SRGB);

                    ovrMatrix4f ovrProjection = ovrMatrix4f_Projection(hmdDesc.DefaultEyeFov[eye], camera->NEAR, camera->FAR, ovrProjection_None);

                    avatar->Render(eyeRenderPose[eye], ovrProjection);
                    skybox->render(*avatar, isLightDirectional ? luz->luzDireccional.ambient : glm::vec3(1));
                    terrain->render(*avatar, isLightDirectional ? luz->luzDireccional.ambient : glm::vec3(1));

                    models->render(*avatar, *shaderModel);
                    aula->render(*avatar, *shaderModel);

                    animation->render(*avatar, numAnim, *shaderAnimation, animationTime * 1.0f);

                    model->renderInstance(*avatar, *shaderInstance);

                    aula->render(*avatar, *shaderModel);

                    debugDrawer->SetMatrices(avatar->view, avatar->proj);
                    bulletWorldController->dynamicsWorld->debugDrawWorld();
                    debugDrawer->col = glm::vec3(0, 1, 0);

                    // Unbind the eye buffer
                    glBindFramebuffer(GL_FRAMEBUFFER, eyeFrameBuffers[eye]);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);

                    // Commit changes to the textures so they get picked up frame
                    ovr_CommitTextureSwapChain(ovr, eyeSwapChains[eye]);
                }

                // Prepare the layers
                ovrLayerEyeFov layerDesc;
                memset(&layerDesc, 0, sizeof(layerDesc));
                layerDesc.Header.Type = ovrLayerType_EyeFov;
                layerDesc.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;   // Because OpenGL.
                for (int eye = 0; eye < 2; ++eye)
                {
                    layerDesc.ColorTexture[eye] = eyeSwapChains[eye];
                    layerDesc.Viewport[eye].Size = eyeSizes[eye];
                    layerDesc.Fov[eye] = hmdDesc.DefaultEyeFov[eye];
                    layerDesc.RenderPose[eye] = eyeRenderPose[eye];
                    layerDesc.SensorSampleTime = sensorSampleTime;
                }

                ovrLayerHeader* layers = &layerDesc.Header;
                ovr_SubmitFrame(ovr, frameIndex, NULL, &layers, 1);

                ovrSessionStatus sessionStatus;
                ovr_GetSessionStatus(ovr, &sessionStatus);
                /*if (sessionStatus.ShouldQuit)
                    running = false;*/
                if (sessionStatus.ShouldRecenter)
                    ovr_RecenterTrackingOrigin(ovr);

                // Blit mirror texture to back buffer
                glBindFramebuffer(GL_READ_FRAMEBUFFER, mirrorFBO);
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

                glBlitFramebuffer(0, HEIGHT_VR, WIDTH_VR, 0, 0, 0, WIDTH_VR, HEIGHT_VR, GL_COLOR_BUFFER_BIT, GL_NEAREST);
                glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
            }
        }
        int numAnim = 0;

        void renderEngine()
        {
            static bool presionado = false;
            static int index = 0;
            static Model* modelSelect = NULL;
            static btCollisionShape* aulaSelect = NULL;


            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            glm::mat4 ProjectionMatrix = camera->GetProjectionMatrix();
            glm::mat4 ViewMatrix = camera->GetViewMatrix();

            //bulletWorldController->physics_step(ImGui::GetIO().Framerate);
            bulletWorldController->physics_step(60.f);

            camera->update(bulletWorldController->dynamicsWorld);
            models->update();
            //animation->update();

            input->processInput(deltaTime);

            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
            {
                glm::vec3 out_origin;
                glm::vec3 out_direction;
                bulletWorldController->screenPosToWorldRay(
                    mouse.x, displayRender.y - mouse.y,
                    displayRender.x, displayRender.y,
                    ViewMatrix,
                    ProjectionMatrix,
                    out_origin,
                    out_direction
                );

                glm::vec3 out_end = out_origin + out_direction * 1000.0f;

                btCollisionWorld::ClosestRayResultCallback RayCallback(btVector3(out_origin.x, out_origin.y, out_origin.z), btVector3(out_end.x, out_end.y, out_end.z));
                bulletWorldController->dynamicsWorld->rayTest(btVector3(out_origin.x, out_origin.y, out_origin.z), btVector3(out_end.x, out_end.y, out_end.z), RayCallback);

                if (RayCallback.hasHit())
                {
                    if (!presionado)
                    {
                        index = (int)RayCallback.m_collisionObject->getUserIndex();
                        if (index != -1)
                            modelSelect = NULL;
                        if (index >= 0 && index < 100)
                        {
                            modelSelect = getModel(index);
                            //aulaSelect = aula->shape[index];
                        }
                        presionado = true;
                    }

                    if (modelSelect != NULL) {
                        btVector3 pos = RayCallback.m_hitPointWorld;

                        btTransform t;
                        t.setIdentity();
                        t.setOrigin(btVector3(pos.getX(), modelSelect->shapeScalar.y + 0.1f, pos.getZ()));

                        if (index >= 0)
                        {
                            modelSelect->body->setWorldTransform(t);
                            //modelSelect->position = glm::vec3(pos.getX(), pos.getY(), pos.getZ());
                            //modelSelect->body->applyForce(btVector3(0, 1, 0), btVector3(5, 1, 0));
                        }
                    }
                }                
            }
            else
            {
                index = -1;
                //modelSelect = NULL;
                presionado = false;
            }

            glBindFramebuffer(GL_FRAMEBUFFER, framebufferEngine);
            {
                glEnable(GL_DEPTH_TEST);

                glDepthMask(GL_TRUE);
                //glEnable(GL_CULL_FACE);
                //glCullFace(GL_BACK);
                //glFrontFace(GL_CW);

                glClearColor(0, 0, 0, 0);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                skybox->render(*camera, isLightDirectional ? luz->luzDireccional.ambient : glm::vec3(1));
                terrain->render(*camera, isLightDirectional ? luz->luzDireccional.ambient : glm::vec3(1));

                models->render(*camera, *shaderModel);
                aula->render(*camera, *shaderModel);
                /*for (int i = 0; i < animation.size(); i++)
                    animation[i]->render(*camera, numAnim, *shaderAnimation, animationTime*1.0f);*/
                animation->renderInstance(*camera, numAnim, *shaderInstanceDynamic, animationTime * 1.0f);

               /* model->renderInstance(*camera, *shaderInstance);
                model1->renderInstance(*camera, *shaderInstance);*/

                debugDrawer->SetMatrices(ViewMatrix, ProjectionMatrix);
                bulletWorldController->dynamicsWorld->debugDrawWorld();
                debugDrawer->col = glm::vec3(0, 1, 0);

            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            DockSpace();

            settingsImGui();
            renderImGui();
            lightingImGui();
            terrainImGui();
            cameraImGui();
            skyboxImGui();
            projectImGui();
            animationImGui();
            aulaImGui(aulaSelect);

            inspectorImGui(modelSelect);


            // Rendering
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            ImGuiIO& io = ImGui::GetIO(); (void)io;

            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                GLFWwindow* backup_current_context = glfwGetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(backup_current_context);
            }

        }


        int render()
        {
            renderInicializar();
            //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            while (!glfwWindowShouldClose(window))
            {
                
                float currentFrame = glfwGetTime();
                deltaTime = currentFrame - lastFrame;
                lastFrame = currentFrame;
                animationTime = currentFrame - startFrame;

                if (isEngine)
                    renderEngine();
                else if (isPc)
                    renderPc();
                else if (isVr)
                    renderRV();

                frameIndex++;
                glfwSwapBuffers(window);
                glfwPollEvents();
            }
            return 0;
        }

        Model* getModel(int index)
        {
            return models->getModelPhysics(index);
            //return NULL;
        }


        void aulaImGui(btCollisionShape* shape)
        {
            ImGui::Begin("Aula", NULL);
            if (shape != NULL)
            {



                if (ImGui::DragFloat3("Size", glm::value_ptr(aula->shapeScalar), 0.1f))
                {
                    aula->changeScalar(shape);
                }


            }


            ImGui::End();
        }

        void settingsImGui()
        {
            ImGui::Begin("Settings ");
            const char* items[] = { "Classic", "Light", "Dark" };
            static int item_current = 2;
            if (ImGui::Combo("Editor Theme", &item_current, items, IM_ARRAYSIZE(items)))
            {
                switch (item_current)
                {
                case 0:
                    ImGui::StyleColorsClassic();
                    break;
                case 1:
                    ImGui::StyleColorsLight();
                    break;
                case 2:
                    ImGui::StyleColorsDark();
                    break;
                default:
                    break;
                }
            }
            //ImGui::SameLine();

            ImGui::Separator();
            const char* GPU = (const char*)glGetString(GL_RENDERER);
            ImGui::Text("OpenGL Version: %s", (const char*)glGetString(GL_VERSION));
            ImGui::Text("GLSL Version: %s", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
            ImGui::Text("Vendor: %s", (const char*)glGetString(GL_VENDOR));
            ImGui::Text("GPU: %s", GPU);
            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
            ImGui::Text("%.2f ms", 1000.0f / ImGui::GetIO().Framerate);
            ImGui::Separator();
            ImGui::Text("Collision Objects: %d", bulletWorldController->dynamicsWorld->getNumCollisionObjects());
            ImGui::Text("Time: %.1f", animationTime);

            /*if (ImGui::ImageButton((void*)texturesImGui[0], ImVec2(100, 100)))
            {
                isEngine = false;
                isPc = true;
                isVr = false;

                int w, h;
                glfwGetWindowSize(window, &w, &h);
                glViewport(0, 0, w, h);

                debugDrawer->ToggleDebugFlag(btIDebugDraw::DBG_DrawWireframe);
            }*/
            ImGui::SameLine(120);
            if (ImGui::ImageButton((void*)texturesImGui[0], ImVec2(100, 100)))
            {
                if (ovr)
                {
                    glfwSetWindowSize(window, WIDTH_VR, HEIGHT_VR);
                    isEngine = false;
                    isPc = false;
                    isVr = true;
                }
                //debugDrawer->ToggleDebugFlag(btIDebugDraw::DBG_DrawWireframe);
            }
            ImGui::SameLine(240);

            ImGui::End();
        }

        void inspectorImGui(Model*& modelSelect)
        {
            ImGui::Begin("Inspector");
            if (modelSelect != NULL)
            {
                ImGui::Text("Index: %d", modelSelect->userIndex);
                ImGui::Separator();
                ImGui::Text("Transform");
                ImGui::DragFloat3("Position", glm::value_ptr(modelSelect->position), 0.1);
                ImGui::DragFloat3("Translate", glm::value_ptr(modelSelect->translate), 0.1f);
                ImGui::DragFloat3("Rotation", glm::value_ptr(modelSelect->rotation), 0.1);
                ImGui::DragFloat3("Scale", glm::value_ptr(modelSelect->scale), 0.1);

                ImGui::Text("Collider");

                const char* shapes[] = { "Box", "Sphere", "Capsule", "Cylinder", "Cone" };
                if (ImGui::Combo("Shape", &modelSelect->shape_current, shapes, IM_ARRAYSIZE(shapes)))
                {
                    modelSelect->changeBodyPhysics(bulletWorldController);
                }
                /*ImGui::DragFloat3("Center", rotation, 0.1);
                ImGui::DragFloat3("Size", scale, 0.1);*/

                if (ImGui::DragFloat3("Size", glm::value_ptr(modelSelect->shapeScalar), 0.1f))
                {
                    modelSelect->changeScalar();
                }
                if (ImGui::DragFloat("Margin", &modelSelect->margin, 0.1f))
                {
                    modelSelect->changeMargin();
                }



                ImGui::Text("Rigidbody");

                if (ImGui::DragFloat3("Angular Factor ", glm::value_ptr(modelSelect->angularFactor), 0.01f))
                {
                    modelSelect->changeAngularFactor();
                }
                if (ImGui::DragFloat3("Linear Velocity", glm::value_ptr(modelSelect->linearVelocity), 0.01f))
                {
                    modelSelect->changeLinearVelocity();
                }
                if (ImGui::DragFloat("Mass", &modelSelect->mass, 0.01f))
                {
                    modelSelect->changeMass();
                }
                if (ImGui::Button("Eliminar", ImVec2(50, 50)))
                {
                    //models->removeModel(modelSelect, bulletWorldController);
                }
            }

            ImGui::End();
        }

        void renderImGui()
        {
            ImGui::Begin("Render", NULL);
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            displayRender = ImGui::GetWindowSize();
            display = ImGui::GetCursorScreenPos();


            drawList->AddImage(
                (void*)textureEngine, display,
                ImVec2(display.x + displayRender.x, display.y + displayRender.y), ImVec2(0, 1), ImVec2(1, 0)
            );

            ImVec2 mousePosition = ImGui::GetMousePos();
            ImVec2 sceenPosition = ImGui::GetCursorScreenPos();
            ImVec2 scroll = ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY());

            this->mouse = ImVec2(mousePosition.x - sceenPosition.x - scroll.x, mousePosition.y - sceenPosition.y - scroll.y);

            ImGui::End();


        }

        void projectImGui()
        {
            ImGui::Begin("Project", NULL);


            if (ImGui::ImageButton((void*)textureObjects[0], ImVec2(100, 100)))
            {
                vector<Texture> textures;
                Texture t(Util::loadTexture("assets/objects/stallTexture.png"), TextureType::DIFFUSE);

                textures.push_back(t);
                //models->addModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "assets/objects/stall.obj", *util->myShaders[ShaderType::MODEL_STATIC], bulletWorldController, textures);
            }
            ImGui::End();
        }

        void skyboxImGui()
        {
            ImGui::Begin("Skybox", NULL);

            if (ImGui::ImageButton((void*)textureSkyboxs[0], ImVec2(100, 100)))
            {
                skybox->loadSkybox("nubes", "jpg");
            }

            ImGui::SameLine(125);

            if (ImGui::ImageButton((void*)textureSkyboxs[1], ImVec2(100, 100)))
            {
                skybox->loadSkybox("blue", "png");
            }

            ImGui::SameLine(250);

            if (ImGui::ImageButton((void*)textureSkyboxs[2], ImVec2(100, 100)))
            {
                skybox->loadSkybox("rainbow", "png");
            }

            if (ImGui::ImageButton((void*)textureSkyboxs[3], ImVec2(100, 100)))
            {
                skybox->loadSkybox("Lan", "jpg");
            }

            if (ImGui::ImageButton((void*)textureSkyboxs[4], ImVec2(100, 100)))
            {
                skybox->loadSkybox("bosque", "png");
            }



            ImGui::End();
        }

        void lightingImGui()
        {
            ImGui::Begin("Lighting", NULL);

            shaderModel->use();
            shaderModel->setVec3("viewPos", camera->Position);
            shaderModel->setFloat("material.shininess", 32.0f);

            if (isLightDirectional)
            {
                ImGui::Text("Luz Direccional");

                ImGui::DragFloat3("Direccion", glm::value_ptr(luz->luzDireccional.direction), 0.001f, 0.0f, 1.0f);
                ImGui::ColorEdit3("Ambiental", glm::value_ptr(luz->luzDireccional.ambient));
                ImGui::ColorEdit3("Difusa", glm::value_ptr(luz->luzDireccional.diffuse));
                ImGui::ColorEdit3("Especular", glm::value_ptr(luz->luzDireccional.specular));

                ImGui::Separator();

                shaderModel->setVec3("dirLight.direction", luz->luzDireccional.direction);
                shaderModel->setVec3("dirLight.ambient", luz->luzDireccional.ambient);
                shaderModel->setVec3("dirLight.diffuse", luz->luzDireccional.diffuse);
                shaderModel->setVec3("dirLight.specular", luz->luzDireccional.specular);
            }

            if (isLightPoint)
            {
                for (int i = 0; i < luz->luzPuntual.size(); i++)
                {
                    ImGui::Text("Luz Puntual %d", i + 1);

                    ImGui::DragFloat3((to_string(i + 1) + ".Position").c_str(), glm::value_ptr(luz->getSol(i)->position), 0.05f);
                    if (ImGui::ColorEdit3((to_string(i + 1) + ".Ambiental").c_str(), glm::value_ptr(luz->luzPuntual[i].ambient)))
                    {
                        //luz->updateSol(i, luz->luzPuntual[i].ambient);
                        shaderModel->use();
                    }
                    ImGui::ColorEdit3((to_string(i + 1) + ".Difusa").c_str(), glm::value_ptr(luz->luzPuntual[i].diffuse));
                    ImGui::ColorEdit3((to_string(i + 1) + ".Especular").c_str(), glm::value_ptr(luz->luzPuntual[i].specular));
                    ImGui::DragFloat((to_string(i + 1) + ".Linear").c_str(), &luz->luzPuntual[i].linear, 0.001f, 0.0f, 1.0f);
                    ImGui::DragFloat((to_string(i + 1) + ".Quadratic").c_str(), &luz->luzPuntual[i].quadratic, 0.001f, 0.0f, 1.0f);

                    if (ImGui::Button((to_string(i + 1) + ".Eliminar").c_str()))
                    {
                        luz->deleteSol(i);
                        shaderModel->setInt("sizePointLights", luz->sizeSol());
                        break;
                    }

                    shaderModel->setVec3("pointLights[" + to_string(i) + "].position", luz->getSol(i)->position);
                    shaderModel->setVec3("pointLights[" + to_string(i) + "].ambient", luz->luzPuntual[i].ambient);
                    shaderModel->setVec3("pointLights[" + to_string(i) + "].diffuse", luz->luzPuntual[i].diffuse);
                    shaderModel->setVec3("pointLights[" + to_string(i) + "].specular", luz->luzPuntual[i].specular);
                    shaderModel->setFloat("pointLights[" + to_string(i) + "].constant", luz->luzPuntual[i].constant);
                    shaderModel->setFloat("pointLights[" + to_string(i) + "].linear", luz->luzPuntual[i].linear);
                    shaderModel->setFloat("pointLights[" + to_string(i) + "].quadratic", luz->luzPuntual[i].quadratic);

                    ImGui::Separator();
                }
            }

            if (isLightSpot)
            {

                ImGui::Text("Luz Focal");

                ImGui::DragFloat3("Position F", glm::value_ptr(camera->Position), 0.05f);
                ImGui::DragFloat3("Direction F", glm::value_ptr(camera->Front), 0.05f);
                ImGui::ColorEdit3("Ambiental F", glm::value_ptr(luz->luzFocal.ambient));
                ImGui::ColorEdit3("Difusa F", glm::value_ptr(luz->luzFocal.diffuse));
                ImGui::ColorEdit3("Especular F", glm::value_ptr(luz->luzFocal.specular));
                ImGui::DragFloat("Constant F", &luz->luzFocal.constant, 0.001f, 0.0f, 256.0f);
                ImGui::DragFloat("Linear F", &luz->luzFocal.linear, 0.001f, 0.0f, 256.0f);
                ImGui::DragFloat("Quadratic F", &luz->luzFocal.quadratic, 0.001f, 0.0f, 256.0f);
                ImGui::DragFloat("OuterCutOff F", &luz->luzFocal.outerCutOff, 0.05f, 0.0f, 256.0f);
                ImGui::DragFloat("CutOff F", &luz->luzFocal.cutOff, 0.05f, 0.0f, luz->luzFocal.outerCutOff);

                shaderModel->setVec3("spotLight.position", camera->Position);
                shaderModel->setVec3("spotLight.direction", camera->Front);
                shaderModel->setVec3("spotLight.ambient", luz->luzFocal.ambient);
                shaderModel->setVec3("spotLight.diffuse", luz->luzFocal.diffuse);
                shaderModel->setVec3("spotLight.specular", luz->luzFocal.specular);
                shaderModel->setFloat("spotLight.constant", luz->luzFocal.constant);
                shaderModel->setFloat("spotLight.linear", luz->luzFocal.linear);
                shaderModel->setFloat("spotLight.quadratic", luz->luzFocal.quadratic);
                shaderModel->setFloat("spotLight.cutOff", glm::cos(glm::radians(luz->luzFocal.cutOff)));
                shaderModel->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(luz->luzFocal.outerCutOff)));
            }
            ImGui::End();
        }

        void animationImGui()
        {
            static bool v;
            ImGui::Begin("Animation", NULL);

            if (ImGui::Checkbox("Luz Direccional", &isLightDirectional))
            {
                shaderModel->setBool("isLightDirectional", isLightDirectional);
            }
            if (ImGui::Checkbox("Luz Puntual", &isLightPoint))
            {
                shaderModel->setBool("isLightPoint", isLightPoint);
            }
            if (ImGui::Checkbox("Luz Focal", &isLightSpot))
            {
                shaderModel->setBool("isLightSpot", isLightSpot);
            }

            if (ImGui::Checkbox("ANIM 1", &v))
            {              
                numAnim = 0;
                animationTime = 0;
            }

            if (ImGui::Checkbox("ANIM 2", &v))
            {
                numAnim = 1;
                animationTime = 0;
            }

            if (ImGui::Checkbox("ANIM 3", &v))
            {
                numAnim = 2;
                animationTime = 0;
            }

            if (isLightPoint)
            {
                if (ImGui::ImageButton((void*)texturesImGui[0], ImVec2(50, 50)))
                {
                    luz->addSol(glm::vec3(camera->Position.x, 10.0f, camera->Position.z), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
                    shaderModel->setInt("sizePointLights", luz->sizeSol());
                }
            }

            ImGui::Separator();

            ImGui::End();
        }

        void terrainImGui()
        {
            ImGui::Begin("Terrain", NULL);
            ImGui::Text("Configuration");
            static int repeat = 10;
            static vector<float> volumen = { 100.0f,100.0f };
            if (ImGui::DragInt("Repeat", &repeat, 0.1f, 1, 100))
            {
                terrain->repeat = repeat;
                terrain->configTerrain();
            }
            if (ImGui::DragFloat2("Volumen", volumen.data(), 0.1f, 1, 1000))
            {
                terrain->volumen = glm::vec3(volumen[0], -0.01f, volumen[1]);
                terrain->configTerrain();
            }

            ImGui::Separator();
            if (ImGui::ImageButton((void*)textureFloors[0], ImVec2(100, 100)))
            {
                terrain->floorTexture = textureFloors[0];
            }
            ImGui::Separator();

            ImGui::End();
        }

        void cameraImGui()
        {
            ImGui::Begin("Camera", NULL);

            ImGui::Text("Transform");

            ImGui::DragFloat3("Position", glm::value_ptr(camera->Position));
            ImGui::DragFloat3("Front", glm::value_ptr(camera->Front));
            ImGui::DragFloat3("Up", glm::value_ptr(camera->Up));

            ImGui::Text("Camera Configuration");

            if (ImGui::DragFloat("Near", &camera->NEAR))
            {
                camera->projection = glm::perspective(glm::radians(camera->FOV), (float)4.0f / (float)3.0f, camera->NEAR, camera->FAR);
            }

            if (ImGui::DragFloat("Far", &camera->FAR))
            {
                camera->projection = glm::perspective(glm::radians(camera->FOV), (float)4.0f / (float)3.0f, camera->NEAR, camera->FAR);
            }

            ImGui::DragFloat("Speed", &camera->speed, 0.01f);



            ImGui::End();
        }

        void DockSpace()
        {
            static bool opt_fullscreen = true;
            static bool opt_padding = false;
            static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

            ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
            if (opt_fullscreen)
            {
                ImGuiViewport* viewport = ImGui::GetMainViewport();
                ImGui::SetNextWindowPos(viewport->GetWorkPos());
                ImGui::SetNextWindowSize(viewport->GetWorkSize());
                ImGui::SetNextWindowViewport(viewport->ID);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
                window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
            }
            else
            {
                dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
            }

            if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
                window_flags |= ImGuiWindowFlags_NoBackground;

            if (!opt_padding)
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("DockSpace Demo", NULL, window_flags);
            if (!opt_padding)
                ImGui::PopStyleVar();

            if (opt_fullscreen)
                ImGui::PopStyleVar(2);

            // DockSpace
            ImGuiIO& io = ImGui::GetIO();
            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
            {
                ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
            }
            else
            {
                ImGuiIO& io = ImGui::GetIO();
                ImGui::Text("ERROR: Docking is not enabled! See Demo > Configuration.");
                ImGui::Text("Set io.ConfigFlags |= ImGuiConfigFlags_DockingEnable in your code, or ");
                ImGui::SameLine(0.0f, 0.0f);
                if (ImGui::SmallButton("click here"))
                    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
            }

            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Options"))
                {
                    ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
                    ImGui::MenuItem("Padding", NULL, &opt_padding);
                    //ImGui::Separator();



                    ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
            }
            ImGui::End();
        }

        void vincularFrambufferEngine()
        {
            glGenFramebuffers(1, &framebufferEngine);
            glBindFramebuffer(GL_FRAMEBUFFER, framebufferEngine);

            glGenTextures(1, &textureEngine);
            glBindTexture(GL_TEXTURE_2D, textureEngine);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureEngine, 0);

            unsigned int rbo;
            glGenRenderbuffers(1, &rbo);
            glBindRenderbuffer(GL_RENDERBUFFER, rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
            // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                //cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
                glBindFramebuffer(GL_FRAMEBUFFER, 0);

        }

        static ovrSession _initOVR()
        {
            ovrSession ovr;
            if (OVR_SUCCESS(ovr_Initialize(NULL)))
            {
                ovrGraphicsLuid luid;
                if (OVR_SUCCESS(ovr_Create(&ovr, &luid)))
                {
                    return ovr;
                }
                ovr_Shutdown();
            }
            return NULL;
        }

        static void _destroyOVR(ovrSession session)
        {
            if (session)
            {
                ovr_Destroy(session);
                ovr_Shutdown();
            }
        }

    };

}
#endif