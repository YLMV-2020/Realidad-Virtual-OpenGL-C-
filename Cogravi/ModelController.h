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

        vector<Model*> models;

        ModelController()
        {

        }
        
        void update()
        {
            for (Model*& model : models)
                model->update();
        }

        void render(Camera& camera)
        {
            for (Model*& model : models)
                model->render(camera);
        }

        void render(Avatar& avatar)
        {
            for (Model*& model : models)
                model->render(avatar);
        }

        void addModel(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, string const& path, Shader shader, BulletWorldController* worldController, vector<Texture> textures = {})
        {
            Model* model = new Model(position, rotation, scale, path, shader, textures);
            model->addBodyPhysicsBox(models.size(), worldController);
            models.push_back(model);
        }

        void removeModel(Model*& model, BulletWorldController* worldController)
        {
            if (!models.empty())
            {
                model->destroy(worldController);
                models.erase(models.begin() + model->userIndex);

                for (int i = 0; i < models.size(); i++)
                {
                    models[i]->userIndex = i;
                    models[i]->body->setUserIndex(i);
                }
                model = NULL;
            }
        }

        Model* getModel(int index)
        {
            return models[index];
        }

    };
}
#endif