#ifndef MODEL_CONTROLLER_HEADER
#define MODEL_CONTROLLER_HEADER

#include "Model.h"

namespace Cogravi {

    class ModelController
    {

    public:

        static ModelController* Instance()
        {
            static ModelController instance;
            return &instance;
        }

        vector<Model*> modelsPhysics;
        vector<GameObject*> models;
        vector<GameObject*> modelsInstance;

        GLuint modelsDynamicSize = 0;
        GLuint modelStaticSize = 0;
        GLuint modelInstanceSize = 0;

        ModelController()
        {

        }
        
        void update()
        {
            for (Model*& model : modelsPhysics)
                model->update();
        }

        void render(Camera& camera, Shader& shader)
        {
            for (unsigned int i = 1; i < modelsDynamicSize; i++)
                modelsPhysics[i]->render(camera, shader);
            
            for (GameObject*& model : models)
                model->render(camera, shader);

        }

        void renderInstance(Camera& camera, Shader& shader)
        {
            for (GameObject*& model : modelsInstance)
                model->renderInstance(camera, shader);
        }

        void render(Avatar& avatar, Shader& shader)
        {
            for (unsigned int i = 1; i < modelsDynamicSize; i++)
                modelsPhysics[i]->render(avatar, shader);

            for (GameObject*& model : models)
                model->render(avatar, shader);
        }

        void addModel(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, string const& path, ColliderType type, glm::vec3 colliderSize = glm::vec3(1.0f), glm::vec3 translateCollider  = glm::vec3(0.0f), vector<Texture> textures = {})
        {
            Model* model = new Model(position, rotation, scale, "assets/objects/" + path, textures);
            model->translate = translateCollider;
            model->shapeScalar = colliderSize;

            switch (type)
            {
            
            case ColliderType::BOX:
                model->shape_current = 0;
                model->addBodyPhysicsBox(modelsPhysics.size());
                break;
            case ColliderType::SPHERE:
                model->shape_current = 1;
                model->addBodyPhysicsSphere(modelsPhysics.size());
                break;
            case ColliderType::CAPSULE:
                model->shape_current = 2;
                model->addBodyPhysicsCapsule(modelsPhysics.size());
                break;
            case ColliderType::CYLINDER:
                model->shape_current = 3;
                model->addBodyPhysicsCylinder(modelsPhysics.size());
                break;
            case ColliderType::CONE:
                model->shape_current = 4;
                model->addBodyPhysicsCone(modelsPhysics.size());
                break;
            case ColliderType::MESH:
                model->shape_current = 5;
                model->addBodyPhysicsMesh(modelsPhysics.size());
                break;
            default:
                break;
            }

            model->shapeScalar = glm::vec3(1.0f);
            modelsPhysics.push_back(model);
            modelsDynamicSize++;
        }

        void addModel(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, string const& path, vector<Texture> textures = {})
        {
            GameObject* model = new GameObject(position, rotation, scale, "assets/objects/" + path, textures);
            models.push_back(model);
            modelStaticSize++;
        }

        void addModelInstance(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, string const& path, int cantidad, glm::mat4* modelMatrices, vector<Texture> textures = {})
        {
            GameObject* model = new GameObject(position, rotation, scale, "assets/objects/" + path, textures, cantidad, modelMatrices);
            modelsInstance.push_back(model);
            modelInstanceSize++;
        }     

        void removeModel(Model*& model)
        {
            if (!modelsPhysics.empty())
            {
                model->destroy();
                modelsPhysics.erase(modelsPhysics.begin() + model->userIndex);
                modelsDynamicSize--;

                for (int i = 0; i < modelsDynamicSize; i++)
                {
                    modelsPhysics[i]->userIndex = i;
                    modelsPhysics[i]->body->setUserIndex(i);
                }
                model = NULL;
                delete model;
            }
        }

        Model*& getModelPhysics(int index)
        {
            return modelsPhysics[index];
        }

    };
}
#endif