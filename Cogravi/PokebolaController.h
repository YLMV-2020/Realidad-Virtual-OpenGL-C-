#ifndef POKEBOLA_CONTROLLER_HEADER
#define POKEBOLA_CONTROLLER_HEADER

#include "Pokebola.h"

namespace Cogravi 
{

    class PokebolaController
    {

    public:

        vector<Pokebola*>pokebolas;

        ModelController* modelController;
        BulletWorldController* bulletWorldController;

        int index;

        static PokebolaController* Instance()
        {
            static PokebolaController instance;
            return &instance;
        }

        PokebolaController()
        {
            modelController = ModelController::Instance();
            bulletWorldController = BulletWorldController::Instance();
        }

        bool update(Model*& modelSelect, float currentFrame)
        {
            bool isCollision = false;
            for (Pokebola*& pokebola : pokebolas)
            {
                pokebola->update();
                pokebola->updateTime(currentFrame);

                for (Model*& model : modelController->modelsPhysics)
                {  
                    if (bulletWorldController->check_collisions(model->userIndex, pokebola->userIndex))
                    {
                        this->index = model->userIndex;
                        modelSelect = modelController->getModelPhysics(index);
                        isCollision = true;
                    }
                }

                if (pokebola->time >= 6.0f)
                {
                    removePokebola(pokebola);
                }
            }  
            return isCollision;
        }

        void render(Camera& camera, Shader& shader)
        {
            for (Pokebola*& pokebola : pokebolas)
                pokebola->render(camera, shader);        
        }

        void render(Avatar& avatar, Shader& shader)
        {
            for (Pokebola*& pokebola : pokebolas)
                pokebola->render(avatar, shader);
        }

        void addPokebola(glm::vec3 position, glm::vec3 impulse)
        {
            Pokebola* pokebola = new Pokebola(position, pokebolas.size() + 100);

            pokebola->body->setRestitution(0.5f);
            pokebola->body->setFriction(0.3f);
            pokebola->body->setRollingFriction(0.3f);
            pokebola->body->setAngularFactor(btVector3(1.0f, 1.0f, 1.0f));
            pokebola->body->applyCentralImpulse(btVector3(impulse.x, impulse.y, impulse.z));

            pokebolas.push_back(pokebola);
        }

        void removePokebola(Pokebola*& pokebola)
        {
            if (!pokebolas.empty())
            {
                pokebola->destroy();
                pokebolas.erase(pokebolas.begin() + (pokebola->userIndex - 100));

                for (int i = 0; i < pokebolas.size(); i++)
                {
                    pokebolas[i]->userIndex = i + 100;
                    pokebolas[i]->body->setUserIndex(i + 100);
                }
            }
        }


    };
}
#endif