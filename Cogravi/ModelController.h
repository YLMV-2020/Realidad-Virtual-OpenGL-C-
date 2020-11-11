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
        Model* tablet;

        GLuint modelsDynamicSize = 0;
        GLuint modelStaticSize = 0;

        ModelController()
        {

        }
        
        void update()
        {
            for (Model*& model : modelsPhysics)
                model->update();

            for (GameObject*& model : models)
                model->update();
        }

        void render(Camera& camera, Shader& shader)
        {
            for (unsigned int i = 1; i < modelsDynamicSize; i++)
                modelsPhysics[i]->render(camera, shader);
            
            for (GameObject*& model : models)
                model->render(camera, shader);
        }

        void render(Avatar& avatar, Shader& shader)
        {
            for (Model*& model : modelsPhysics)
                model->render(avatar, shader);

            for (GameObject*& model : models)
                model->render(avatar, shader);
        }

        void addModel(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, string const& path, ColliderType type, BulletWorldController* worldController, glm::vec3 colliderSize = glm::vec3(1.0f), glm::vec3 translateCollider  = glm::vec3(0.0f), vector<Texture> textures = {})
        {
            Model* model = new Model(position, rotation, scale, "assets/objects/" + path, textures);
            model->translate = translateCollider;
            model->shapeScalar = colliderSize;

            switch (type)
            {
            
            case ColliderType::BOX:
                model->shape_current = 0;
                model->addBodyPhysicsBox(modelsPhysics.size(), worldController);
                break;
            case ColliderType::SPHERE:
                model->shape_current = 1;
                model->addBodyPhysicsSphere(modelsPhysics.size(), worldController);
                break;
            case ColliderType::CAPSULE:
                model->shape_current = 2;
                model->addBodyPhysicsCapsule(modelsPhysics.size(), worldController);
                break;
            case ColliderType::CYLINDER:
                model->shape_current = 3;
                model->addBodyPhysicsCylinder(modelsPhysics.size(), worldController);
                break;
            case ColliderType::CONE:
                model->shape_current = 4;
                model->addBodyPhysicsCone(modelsPhysics.size(), worldController);
                break;
            case ColliderType::MESH:
                model->shape_current = 5;
                model->addBodyPhysicsMesh(modelsPhysics.size(), worldController);
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

        void removeModel(Model*& model, BulletWorldController* worldController)
        {
            if (!modelsPhysics.empty())
            {
                model->destroy(worldController);
                modelsPhysics.erase(modelsPhysics.begin() + model->userIndex);

                for (int i = 0; i < modelsPhysics.size(); i++)
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