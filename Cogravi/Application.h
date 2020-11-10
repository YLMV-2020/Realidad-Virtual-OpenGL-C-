#ifndef APPLICATION_HEADER
#define APPLICATION_HEADER

#include "ApplicationAttributes.h"

namespace Cogravi
{
    class Application :public ApplicationAttributes
    {
    public:

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
                if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
                {
                    vector<Texture> g;
                    Texture sd = Texture(Util::loadTexture("assets/textures//terrain//azulX.jpg"), TextureType::DIFFUSE);
                    g.push_back(sd);
                    modelController->addModel(camera->Position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(5.0f, 5.0f, 5.0f), "assets/objects/cubo.obj", ColliderType::SPHERE, bulletWorldController, glm::vec3(1.1f), glm::vec3(0.0f), g);
                    //modelController->getModelPhysics(0)->body->applyCentralImpulse(btVector3(0, 20, 0));
                    float fuerza = 20.0f;
                    glm::vec3 look = camera->Front * fuerza;
                    modelController->modelsPhysics.back()->body->setRestitution(0.5f);
                    //modelController->modelsPhysics.back()->body->setGravity(btVector3)
                    //modelController->modelsPhysics.back()->body->setmas
                    modelController->modelsPhysics.back()->body->setFriction(0.3);
                    modelController->modelsPhysics.back()->body->setRollingFriction(0.3);
                    modelController->modelsPhysics.back()->body->setAngularFactor(btVector3(1, 1, 1));
                    modelController->modelsPhysics.back()->body->applyCentralImpulse(btVector3(look.x, look.y, look.z));
                    //modelController->modelsPhysics.back()->body->setLinearVelocity(btVector3(look.x, look.y, look.z));
                    //modelController->modelsPhysics.back()->body->setCollisionFlags(modelController->modelsPhysics.back()->body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
                }

                if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                {
                    isEngine = true;
                    isVr = false;
                    isPc = false;
                    glViewport(0, 0, WIDTH, HEIGHT);
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
            };
        }

        void addModels()
        {
            vector<Texture> texturesLoad;
            modelController->addModel(glm::vec3(0.0f, 0.00f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10.0f, 10.0f, 10.0f), "tunel/tunel.obj");     
            modelController->addModel(glm::vec3(-10.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.1f, 1.1f, 1.1f), "Birthday Scene/Birthday Area.obj");          
            modelController->addModel(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.f, 1.f, 1.f), "rex/allo.obj", ColliderType::BOX, bulletWorldController, glm::vec3(1.48f, 2.47f, 5.57f), glm::vec3(0, -2.5, 0.39));
            modelController->addModel(glm::vec3(10.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.f, 1.f, 1.f), "stove/stove.obj");
            modelController->addModel(glm::vec3(-10.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.f, 1.f, 1.f), "PIZZA/pizza1.obj");
            //modelController->addModel(glm::vec3(20.0f, 0.0f, 20.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.f, 1.f, 1.f), "tablet/tablet.obj", ColliderType::BOX, bulletWorldController, glm::vec3(0.63f, 0.94f, 0.09f), glm::vec3(0.0f, -0.95f, 0.0f));
            //modelController->addModel(glm::vec3(110.0f, 0.0f, 110.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.f, 1.f, 1.f), "start/start.obj");
            //modelController->addModel(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.f, 1.f, 1.f), "Pro-Bending Arena/probending Arena.obj");


        }

        void addAnimations()
        {
            animationController->addAnimation(glm::vec3(-30.0f, 0.00f, 40.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f), "doctora/doctora.dae", *shaderAnimation);
        }

        void loadTexturesImGui()
        {
            texturesImGui.push_back(Util::loadTexture("assets\\textures\\imgui\\VR.png"));
        }

        void loadTextureObjects()
        {
            textureObjects.push_back(Util::loadTexture("assets\\textures\\objects\\tierra.png"));
            textureObjects.push_back(Util::loadTexture("assets\\textures\\objects\\mercurio.png"));
            textureObjects.push_back(Util::loadTexture("assets\\textures\\objects\\rex.png"));
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
            textureFloors.push_back(Util::loadTexture("assets\\textures\\terrain\\marble.jpg"));
            textureFloors.push_back(Util::loadTexture("assets\\textures\\terrain\\wall.jpg"));
            textureFloors.push_back(Util::loadTexture("assets\\textures\\terrain\\azulX.jpg"));
            textureFloors.push_back(Util::loadTexture("assets\\textures\\terrain\\tileable.jpg"));
            textureFloors.push_back(Util::loadTexture("assets\\textures\\terrain\\grassyCliff.png"));
            textureFloors.push_back(Util::loadTexture("assets\\textures\\terrain\\TiledConcrete.png"));
            textureFloors.push_back(Util::loadTexture("assets\\textures\\terrain\\wood.png"));
        }

        void inicializarScene()
        {
            util = Util::Instance();
            bulletWorldController = BulletWorldController::Instance();

            //avatar = new Avatar(bulletWorldController);

            terrain = new Terrain("marble.jpg", glm::vec3(100, -0.01f, 100), 50.0f, bulletWorldController);
            skybox = new Skybox("bosque", "png");

            debugDrawer = new DebugDrawer();
            debugDrawer->setDebugMode(0);

            bulletWorldController->dynamicsWorld->setDebugDrawer(debugDrawer);
            debugDrawer->ToggleDebugFlag(btIDebugDraw::DBG_DrawWireframe);

            modelController = ModelController::Instance();
            animationController = AnimationController::Instance();

            luz = Lighting::Instance();
            text = new Text("Arial.ttf");

            tablet = Tablet::Instance();

            loadTextureFloors();
            loadTextureObjects();
            loadTexturesImGui();
            loadTextureSkyboxs();

            shaderModel = util->myShaders[ShaderType::MODEL_STATIC];
            shaderAnimation = util->myShaders[ShaderType::MODEL_DYNAMIC];
            shaderInstance = util->myShaders[ShaderType::INSTANCE_STATIC];
            shaderInstanceDynamic = util->myShaders[ShaderType::INSTANCE_DYNAMIC];
            shaderSol = util->myShaders[ShaderType::SOL];

            player = new Player(*shaderAnimation);
            camera = new Camera(player->body);
            input = new InputProcessor(window, camera);

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
            io.ConfigFlags |= ImGuiWindowFlags_AlwaysHorizontalScrollbar;


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
            glDepthMask(GL_TRUE);
            glEnable(GL_DEPTH_TEST);
            inicializarImgui();
            inicializarScene();
            /*inicializarVR();
            avatar->Init();*/
            util->vincularFrambuffer(framebufferEngine, textureEngine);
            util->vincularFrambuffer(framebufferTablet, textureTablet);

            tablet->updateTexture(textureTablet);
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
             /*modelController->render(*camera);
             animations->render(*camera, animationTime);*/

            debugDrawer->SetMatrices(ViewMatrix, ProjectionMatrix);
            bulletWorldController->dynamicsWorld->debugDrawWorld();
            //debugDrawer->color = glm::vec3(0, 1, 0);

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
                avatar->Prerender(ovr, deltaTime);
                bulletWorldController->physics_step(60.f);

                avatar->Update();
                //animations->update();
                modelController->update();

                Model* referencia = modelController->getModelPhysics(0);

                referencia->isCollision = true;
                avatar->isCollision[0] = true;

                btTransform transform;
                transform.setIdentity();
                transform.setOrigin(btVector3(avatar->leftHandPosition.x, avatar->leftHandPosition.y, avatar->leftHandPosition.z));

                float x, y, z;
                avatar->leftHandRotation.GetYawPitchRoll(&x, &y, &z);

                btQuaternion quat;
                quat.setEulerZYX(btScalar(z), btScalar(y), btScalar(x));
                transform.setRotation(btQuaternion(quat));

                referencia->body->setWorldTransform(transform);

                /* static bool colision = false;
                 if (bulletWorldController->check_collisions(referencia->userIndex, avatar->userIndex[0]))
                 {
                     colision = true;

                 }
                 if (avatar->isCollision && colision)
                 {
                     referencia->isCollision = true;

                     btTransform transform;
                     transform.setIdentity();
                     transform.setOrigin(btVector3(avatar->leftHandPosition.x, avatar->leftHandPosition.y, avatar->leftHandPosition.z));
                     float x, y, z;
                     avatar->leftHandRotation.GetYawPitchRoll(&x, &y, &z);
                     btQuaternion quat;
                     quat.setEulerZYX(btScalar(z), btScalar(y), btScalar(x));
                     transform.setRotation(btQuaternion(quat));
                     referencia->body->setWorldTransform(transform);
                 }
                 else
                 {
                     referencia->isCollision = false;
                     colision = false;
                 }*/



                 /* if (bulletWorldController->check_collisions(modelController->getModelPhysics(0)->userIndex, avatar->userIndex[1]))
                  {
                      if (avatar->isCollision)
                      {
                          referencia->isCollision = true;
                          referencia->position.x = avatar->position.x;
                          referencia->position.y = avatar->position.y;
                          referencia->position.z = avatar->position.z;
                      }
                      else
                      {
                          referencia->isCollision = false;
                      }
                      cout << "Colisiaona mango 1\n";
                  }*/


                  /*player->input(window, deltaTime);
                  player->update();*/
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
                    skybox->render(*avatar, luz->isLightDirectional ? luz->luzDireccional.ambient : glm::vec3(1));
                    terrain->render(*avatar, luz->isLightDirectional ? luz->luzDireccional.ambient : glm::vec3(1));

                    modelController->render(*avatar, *shaderModel);

                    debugDrawer->SetMatrices(avatar->view, avatar->proj);
                    bulletWorldController->dynamicsWorld->debugDrawWorld();

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

        void renderEngine()
        {
            static bool presionado = false;
            static int index = 0;
            static Model* modelSelect = NULL;

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            glm::mat4 ProjectionMatrix = camera->GetProjectionMatrix();
            glm::mat4 ViewMatrix = camera->GetViewMatrix();

            //bulletWorldController->physics_step(ImGui::GetIO().Framerate);
            bulletWorldController->physics_step(60.f);

            camera->update(bulletWorldController->dynamicsWorld);
            modelController->update();
            player->input(window, deltaTime);
            player->update();
            player->updateTime(lastFrame);

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
                            if (modelSelect->description.font != lastFont)
                            {
                                text->loadType(modelSelect->description.fontPath);
                                lastFont = modelSelect->description.font;
                            }
                        }
                        presionado = true;
                    }

                    if (modelSelect != NULL && index >= 0) 
                    {
                        

                        btVector3 pos = RayCallback.m_hitPointWorld;
                        btTransform t;
                        t.setIdentity();
                        t.setOrigin(btVector3(pos.getX(), modelSelect->position.y, pos.getZ()));

                        btQuaternion quat;
                        quat.setEulerZYX(glm::radians(modelSelect->rotate.z), glm::radians(modelSelect->rotate.y), glm::radians(modelSelect->rotate.x));

                        t.setRotation(quat);
                        modelSelect->body->setWorldTransform(t);
                    }
                }
            }
            else
            {
                index = -1;
                presionado = false;
            }

            DockSpace();

            glBindFramebuffer(GL_FRAMEBUFFER, framebufferEngine);
            {
                glClearColor(0, 0, 0, 0);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                skybox->render(*camera, luz->isLightDirectional ? luz->luzDireccional.ambient : glm::vec3(1));
                terrain->render(*camera, luz->isLightDirectional ? luz->luzDireccional.ambient : glm::vec3(1));
                //tablet->render(*camera, *shaderModel);
                modelController->render(*camera, *shaderModel);
                animationController->render(*camera, *shaderAnimation, lastFrame);
                player->render(*camera, *shaderAnimation);

                debugDrawer->SetMatrices(ViewMatrix, ProjectionMatrix);
                bulletWorldController->dynamicsWorld->debugDrawWorld();

                lightingImGui();

            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glBindFramebuffer(GL_FRAMEBUFFER, framebufferTablet);
            {
                glClearColor(0, 0, 0, 0);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                if (modelSelect != NULL)
                {
                    //AGREGAR INTEFRZAF PARA TIIPO DE LETRA, COLOR, TAMAÑAO,
                    text->RenderText(modelSelect->description.bloc, 100.0f, 750.0f, 1.0f, modelSelect->description.color);
                    text->RenderText(modelSelect->description.bloc, 100.0f, 700.0f, 1.0f, modelSelect->description.color);
                    text->RenderText(modelSelect->description.bloc, 100.0f, 650.0f, 1.0f, modelSelect->description.color);
                    text->RenderText(modelSelect->description.bloc, 100.0f, 600.0f, 1.0f, modelSelect->description.color);
                    text->RenderText(modelSelect->description.bloc, 100.0f, 550.0f, 1.0f, modelSelect->description.color);

                }
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            settingsImGui();
            renderImGui();
            tabletImGui();

            terrainImGui();
            cameraImGui();
            skyboxImGui();
            projectImGui();
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

            while (!glfwWindowShouldClose(window))
            {
                float currentFrame;
                currentFrame = glfwGetTime();

                deltaTime = currentFrame - lastFrame;
                lastFrame = currentFrame;

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
            return modelController->getModelPhysics(index);
            //return NULL;
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
            ImGui::Text("Time: %.1f", ImGui::GetTime());

            ImGui::Separator();
            if (ImGui::Checkbox("Draw Wireframe", &isDrawWireframe))
                debugDrawer->ToggleDebugFlag(btIDebugDraw::DBG_DrawWireframe);

            if (ImGui::Checkbox("Draw AABB", &isDrawAABB))
                debugDrawer->ToggleDebugFlag(btIDebugDraw::DBG_DrawAabb);

            if (ImGui::Checkbox("Model Wireframe", &isWireframe))
            {
                if (isWireframe)
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                else
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            ImGui::ColorEdit3("Draw Color", glm::value_ptr(debugDrawer->color));


            ImGui::Separator();
            if (ImGui::ImageButton((void*)texturesImGui[0], ImVec2(100, 100)))
            {
                if (ovr)
                {
                    glfwSetWindowSize(window, WIDTH_VR, HEIGHT_VR);
                    isEngine = false;
                    isPc = false;
                    isVr = true;
                }
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
                ImGui::DragFloat3("Position", glm::value_ptr(modelSelect->position), 0.01f);
                ImGui::DragFloat3("Translate", glm::value_ptr(modelSelect->translate), 0.01f);
                if(ImGui::DragFloat3("Rotation", glm::value_ptr(modelSelect->rotate), 0.1f))
                {
                    modelSelect->rotation = modelSelect->rotate;
                    btTransform t;
                    t.setIdentity();
                    t.setOrigin(btVector3(modelSelect->position.x, modelSelect->position.y, modelSelect->position.z));

                    btQuaternion quat;
                    quat.setEulerZYX(glm::radians(modelSelect->rotation.z), glm::radians(modelSelect->rotation.y), glm::radians(modelSelect->rotation.x));

                    t.setRotation(quat);
                    modelSelect->body->setWorldTransform(t);

                    t.getRotation().getEulerZYX(modelSelect->rotation.z, modelSelect->rotation.y, modelSelect->rotation.x);
                   
                }
                ImGui::DragFloat3("Scale", glm::value_ptr(modelSelect->scale), 0.01f, 0.01f, 10.0f);

                ImGui::Text("Collider");

                static const char* shapes[] = { "Box", "Sphere", "Capsule", "Cylinder", "Cone","Mesh" };

                if (ImGui::Combo("Shape", &modelSelect->shape_current, shapes, IM_ARRAYSIZE(shapes)))
                {
                    modelSelect->shapeScalar = glm::vec3(1.0f);
                    modelSelect->changeBodyPhysics(bulletWorldController);
                }

                if (modelSelect->shape_current == 1)
                {
                    if (ImGui::DragFloat("Radius", glm::value_ptr(modelSelect->shapeScalar), 0.1f))
                    {
                        modelSelect->changeScalar();
                    }
                }
                else
                {
                    if (ImGui::DragFloat3("Size", glm::value_ptr(modelSelect->shapeScalar), 0.01f, 0.01f, 100.0f))
                    {
                        modelSelect->changeScalar();
                    }
                }
                if (ImGui::DragFloat("Margin", &modelSelect->margin, 0.1f))
                {
                    modelSelect->changeMargin();
                }

                ImGui::Text("Rigidbody");

                if (ImGui::DragFloat("Mass", &modelSelect->mass, 0.01f))
                {
                    modelSelect->changeMass();
                }

                ImGui::Separator();
                ImGui::Text("Font");
                static const char* fonts[] = { "Arial", "Retro Computer"};
                static const string format[] = { ".ttf", ".otf" };

                if (ImGui::Combo("Type", &modelSelect->description.font, fonts, IM_ARRAYSIZE(fonts)))
                {
                    lastFont = modelSelect->description.font;
                    modelSelect->description.fontPath = string(fonts[lastFont]) + format[lastFont];
                    text->loadType(modelSelect->description.fontPath);
                }
                ImGui::ColorEdit3("Color", glm::value_ptr(modelSelect->description.color));
                ImGui::Text("Description");

                ImGui::InputTextMultiline(" ", modelSelect->description.bloc, sizeof(modelSelect->description));

                //// Basic columns
                //if (ImGui::TreeNode("Basic"))
                //{
                //    

                //    ImGui::Text("With border:");
                //    ImGui::Columns(4, "mycolumns"); // 4-ways, with border
                //    ImGui::Separator();
                //    ImGui::Text("ID"); ImGui::NextColumn();
                //    ImGui::Text("Name"); ImGui::NextColumn();
                //    ImGui::Text("Path"); ImGui::NextColumn();
                //    ImGui::Text("Hovered"); ImGui::NextColumn();
                //    ImGui::Separator();
                //    const char* names[3] = { "One", "Two", "Three" };
                //    const char* paths[3] = { "/path/one", "/path/two", "/path/three" };
                //    static int selected = -1;
                //    for (int i = 0; i < 3; i++)
                //    {
                //        char label[32];
                //        sprintf(label, "%04d", i);
                //        if (ImGui::Selectable(label, selected == i, ImGuiSelectableFlags_SpanAllColumns))
                //            selected = i;
                //        bool hovered = ImGui::IsItemHovered();
                //        ImGui::NextColumn();
                //        ImGui::Text(names[i]); ImGui::NextColumn();
                //        ImGui::Text(paths[i]); ImGui::NextColumn();
                //        ImGui::Text("%d", hovered); ImGui::NextColumn();
                //    }
                //    ImGui::Columns(1);
                //    ImGui::Separator();
                //    ImGui::TreePop();
                //}
                ImGui::Separator();
                if (ImGui::Button("Eliminar", ImVec2(100, 50)))
                {
                    modelController->removeModel(modelSelect, bulletWorldController);
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

        void tabletImGui()
        {
            ImGui::Begin("Tablet", NULL);

            

            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImVec2 displayRender = ImGui::GetWindowSize();
            ImVec2 display = ImGui::GetCursorScreenPos();

            drawList->AddImage(
                (void*)textureTablet, display,
                ImVec2(display.x + displayRender.x, display.y + displayRender.y), ImVec2(0, 1), ImVec2(1, 0)
            );

            ImGui::End();
        }

        void projectImGui()
        {
            ImGui::Begin("Project", NULL);

            if (ImGui::ImageButton((void*)textureObjects[0], ImVec2(100, 100)))
            {
                modelController->addModel(glm::vec3(camera->Position.x, 3.50f, camera->Position.z), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.05f, 0.05f, 0.05f), "tierra/tierra.3ds", ColliderType::SPHERE, bulletWorldController, glm::vec3(2.9f));
            }
            ImGui::SameLine();
            if (ImGui::ImageButton((void*)textureObjects[1], ImVec2(100, 100)))
            {
                vector<Texture> textures;
                Texture txt(Util::loadTexture("assets/objects/mercurio/2k_mercury.jpg"), TextureType::DIFFUSE);
                textures.push_back(txt);
                modelController->addModel(camera->Front + camera->Position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.001f, 0.001f, 0.001f), "mercurio/mercurio.obj", ColliderType::SPHERE, bulletWorldController, glm::vec3(2.1f, 0.0f, 0.0f), glm::vec3(0.0f), textures);
            }

            ImGui::SameLine();

            if (ImGui::ImageButton((void*)textureObjects[2], ImVec2(100, 100)))
            {
                modelController->addModel(camera->Front + camera->Position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.f, 1.f, 1.f), "rex/allo.obj", ColliderType::BOX, bulletWorldController, glm::vec3(1.48f, 2.47f, 5.57f), glm::vec3(0, -2.5, 0.39));
               
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

            ImGui::SameLine();

            if (ImGui::ImageButton((void*)textureSkyboxs[1], ImVec2(100, 100)))
            {
                skybox->loadSkybox("blue", "png");
            }

            ImGui::SameLine();

            if (ImGui::ImageButton((void*)textureSkyboxs[2], ImVec2(100, 100)))
            {
                skybox->loadSkybox("rainbow", "png");
            }
            ImGui::SameLine();
            if (ImGui::ImageButton((void*)textureSkyboxs[3], ImVec2(100, 100)))
            {
                skybox->loadSkybox("Lan", "jpg");
            }
            ImGui::SameLine();
            if (ImGui::ImageButton((void*)textureSkyboxs[4], ImVec2(100, 100)))
            {
                skybox->loadSkybox("bosque", "png");
            }



            ImGui::End();
        }

        void lightingImGui()
        {
            ImGui::Begin("Lighting", NULL);

            ImGui::Checkbox("Luz Direccional", &luz->isLightDirectional);
            ImGui::Checkbox("Luz Puntual", &luz->isLightPoint);
            ImGui::Checkbox("Luz Focal", &luz->isLightSpot);

            if (luz->isLightDirectional)
            {
                ImGui::Text("Luz Direccional");

                ImGui::DragFloat3("Direccion", glm::value_ptr(luz->luzDireccional.direction), 0.001f, 0.0f, 0.01f);
                ImGui::ColorEdit3("Ambiental", glm::value_ptr(luz->luzDireccional.ambient));
                ImGui::ColorEdit3("Difusa", glm::value_ptr(luz->luzDireccional.diffuse));
                ImGui::ColorEdit3("Especular", glm::value_ptr(luz->luzDireccional.specular));

                ImGui::Separator();
            }

            if (luz->isLightPoint)
            {
                for (int i = 0; i < luz->sizeSol(); i++)
                {
                    ImGui::Text("Luz Puntual %d", i + 1);

                    ImGui::DragFloat3((to_string(i + 1) + ".Position").c_str(), glm::value_ptr(luz->getSol(i)->position), 0.05f);
                    ImGui::ColorEdit3((to_string(i + 1) + ".Ambiental").c_str(), glm::value_ptr(luz->luzPuntual[i].ambient));
                    ImGui::ColorEdit3((to_string(i + 1) + ".Difusa").c_str(), glm::value_ptr(luz->luzPuntual[i].diffuse));
                    ImGui::ColorEdit3((to_string(i + 1) + ".Especular").c_str(), glm::value_ptr(luz->luzPuntual[i].specular));
                    ImGui::DragFloat((to_string(i + 1) + ".Constant").c_str(), &luz->luzPuntual[i].constant, 0.001f, 0.0f, 1.0f);
                    ImGui::DragFloat((to_string(i + 1) + ".Linear").c_str(), &luz->luzPuntual[i].linear, 0.001f, 0.0f, 1.0f);
                    ImGui::DragFloat((to_string(i + 1) + ".Quadratic").c_str(), &luz->luzPuntual[i].quadratic, 0.001f, 0.0f, 1.0f);

                    if (ImGui::Button((to_string(i + 1) + ".Eliminar").c_str()))
                    {
                        luz->deleteSol(i);
                        break;
                    }

                    ImGui::Separator();
                }
                if (ImGui::ImageButton((void*)texturesImGui[0], ImVec2(50, 50)))
                {
                    luz->addSol(camera->Position + camera->Front, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(3.50f, 3.50f, 3.50f));
                }
            }

            if (luz->isLightSpot)
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


            }

            luz->loadShader(*camera, *shaderAnimation, *shaderSol);
            luz->loadShader(*camera, *shaderModel, *shaderSol);
            luz->loadShader(*camera, *shaderInstance, *shaderSol);
            //luz->loadShader(*camera, *shaderInstanceDynamic);


            ImGui::End();
        }

        void terrainImGui()
        {

            ImGui::Begin("Terrain", NULL);
            ImGui::Text("Configuration");
            static int repeat = 500;
            static vector<float> volumen = { 1000.0f,1000.0f };
            if (ImGui::DragInt("Repeat", &repeat, 0.1f, 1, 1000))
            {
                terrain->repeat = repeat;
                terrain->configTerrain();
            }
            if (ImGui::DragFloat2("Volumen", volumen.data(), 0.1f, 1, 10000))
            {
                terrain->volumen = glm::vec3(volumen[0], -0.01f, volumen[1]);
                terrain->configTerrain();
            }

            ImGui::Separator();
            for (int i = 0; i < textureFloors.size(); i++)
            {
                if (ImGui::ImageButton((void*)textureFloors[i], ImVec2(100, 100)))
                {
                    terrain->floorTexture = textureFloors[i];
                }
                /* if ((i + 1) % 6 != 0)*/
                ImGui::SameLine();
            }
            ImGui::Separator();

            ImGui::End();
        }

        void cameraImGui()
        {
            ImGui::Begin("Camera", NULL);

            static int modeCamera = 0;
            if (ImGui::RadioButton("First Person", &modeCamera, 0))
                camera->setMode(CameraType::FIRST_PERSON);
            ImGui::SameLine();
            if (ImGui::RadioButton("Third Person", &modeCamera, 1))
                camera->setMode(CameraType::THIRD_PERSON);

            ImGui::Text("Transform");

            ImGui::DragFloat3("Position", glm::value_ptr(camera->Position));
            ImGui::DragFloat3("Front", glm::value_ptr(camera->Front));
            ImGui::DragFloat3("Up", glm::value_ptr(camera->Up));

            ImGui::Text("Configuration");

            if (ImGui::DragFloat("Near", &camera->NEAR))
            {
                camera->projection = glm::perspective(glm::radians(camera->FOV), (float)WIDTH / (float)HEIGHT, camera->NEAR, camera->FAR);
            }

            if (ImGui::DragFloat("Far", &camera->FAR))
            {
                camera->projection = glm::perspective(glm::radians(camera->FOV), (float)WIDTH / (float)HEIGHT, camera->NEAR, camera->FAR);
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
                    //ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
                    ImGui::MenuItem("Padding", NULL, &opt_padding);
                    //ImGui::Separator();
                    ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
            }
            ImGui::End();
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