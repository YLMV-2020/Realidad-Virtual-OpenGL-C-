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
        vector<Model*> bullets;
        vector<GameObject*> models;

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

            for (Model*& bullet : bullets)
                bullet->update();
        }

        void render(Camera& camera, Shader& shader)
        {
            for (unsigned int i = 1; i < modelsDynamicSize; i++)
                modelsPhysics[i]->render(camera, shader);
            
            for (GameObject*& model : models)
                model->render(camera, shader);

            for (Model*& bullet : bullets)
                bullet->render(camera, shader);
        }

        void render(Avatar& avatar, Shader& shader)
        {
            for (unsigned int i = 1; i < modelsDynamicSize; i++)
                modelsPhysics[i]->render(avatar, shader);

            for (GameObject*& model : models)
                model->render(avatar, shader);

            for (Model*& bullet : bullets)
                bullet->render(avatar, shader);
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

        void addBullet(glm::vec3 position, glm::vec3 impulse)
        {
            Model* bullet = new Model(position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f), "assets/objects/pokebola/pokebola.obj");
            bullet->translate = glm::vec3(0.0f, 0.0f, 0.03f);
            bullet->shapeScalar = glm::vec3(0.06f);
            bullet->shape_current = 0;
            //bullet->mass = 10.0f;
            bullet->addBodyPhysicsSphere(bullets.size() + 100);

            bullet->body->setRestitution(0.5f);
            bullet->body->setFriction(0.3f);
            bullet->body->setRollingFriction(0.3f);
            bullet->body->setAngularFactor(btVector3(1.0f, 1.0f, 1.0f));
            bullet->body->applyCentralImpulse(btVector3(impulse.x, impulse.y, impulse.z));

            bullets.push_back(bullet);

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