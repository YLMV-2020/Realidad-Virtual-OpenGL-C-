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
            for (Model*& model : modelsPhysics)
                model->render(camera, shader);

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

        void addModel(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, string const& path, ColliderType type, BulletWorldController* worldController, vector<Texture> textures = {})
        {
            Model* model = new Model(position, rotation, scale, path, textures);
            //model->addBodyPhysicsBox(models.size(), worldController);

            switch (type)
            {
            case ColliderType::MESH:
                model->addBodyPhysicsMesh(modelsPhysics.size(), worldController);
                break;
            case ColliderType::BOX:
                model->addBodyPhysicsBox(modelsPhysics.size(), worldController);
                break;
            case ColliderType::SPHERE:
                model->addBodyPhysicsSphere(modelsPhysics.size(), worldController);
                break;
            case ColliderType::CAPSULE:
                model->addBodyPhysicsCapsule(modelsPhysics.size(), worldController);
                break;
            case ColliderType::CYLINDER:
                model->addBodyPhysicsCylinder(modelsPhysics.size(), worldController);
                break;
            case ColliderType::CONE:
                model->addBodyPhysicsCone(modelsPhysics.size(), worldController);
                break;

            default:
                break;
            }


            modelsPhysics.push_back(model);
        }

        void addModel(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, string const& path, vector<Texture> textures = {})
        {
            GameObject* model = new GameObject(position, rotation, scale, path, textures);
            models.push_back(model);
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
            }
        }

        Model* getModelPhysics(int index)
        {
            return modelsPhysics[index];
        }

    };
}
#endif