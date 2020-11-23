#ifndef ANIMATION_CONTROLLER_HEADER
#define ANIMATION_CONTROLLER_HEADER

#include "Animation.h"

namespace Cogravi {

    class AnimationController
    {

    public:

        static AnimationController* Instance()
        {
            static AnimationController instance;
            return &instance;
        }

        vector<Animation*> animationsPhysics;
        vector<DynamicGameObject*> animations;
        vector<DynamicGameObject*> animationsInstance;

        AnimationController()
        {

        }

        void update()
        {
            for (Animation*& animation : animationsPhysics)
                animation->update();
        }

        void renderInstance(Camera& camera, Shader& shader, float currentFrame)
        {
            for (DynamicGameObject*& animation : animationsInstance)
            {
                animation->updateTime(currentFrame);
                animation->renderInstance(camera, shader);
            }
        }

        void renderInstance(Avatar& avatar, Shader& shader, float currentFrame)
        {
            for (DynamicGameObject*& animation : animationsInstance)
            {
                animation->updateTime(currentFrame);
                animation->renderInstance(avatar, shader);
            }
        }

        void render(Camera& camera, Shader& shader, float currentFrame)
        {
            for (Animation*& animation : animationsPhysics)
            {
                animation->updateTime(currentFrame);
                animation->render(camera, shader);
            }

            for (DynamicGameObject*& animation : animations)
            {
                animation->updateTime(currentFrame);
                animation->render(camera, shader);
            }
        }

        void render(Avatar& avatar, Shader& shader, float currentFrame)
        {
            for (Animation*& animation : animationsPhysics)
            {
                animation->updateTime(currentFrame);
                animation->render(avatar, shader);
            }

            for (DynamicGameObject*& animation : animations)
            {
                animation->updateTime(currentFrame);
                animation->render(avatar, shader);
            }
        }

        void addAnimation(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, string const& path, ColliderType type, BulletWorldController* worldController, Shader &shader, vector<Texture> textures = {}, glm::vec3 colliderSize = glm::vec3(1.0f))
        {
            Animation* animation = new Animation(position, rotation, scale, "assets/animations/" + path, shader, textures);
            animation->shapeScalar = colliderSize;

            switch (type)
            {

            case ColliderType::BOX:
                animation->shape_current = 0;
                animation->addBodyPhysicsBox(-1 * (animationsPhysics.size() + 2), worldController);
                break;
            case ColliderType::SPHERE:
                animation->shape_current = 1;
                animation->addBodyPhysicsSphere(-1 * (animationsPhysics.size() + 2), worldController);
                break;
            case ColliderType::CAPSULE:
                animation->shape_current = 2;
                animation->addBodyPhysicsCapsule(-1 * (animationsPhysics.size() + 2), worldController);
                break;
            case ColliderType::CYLINDER:
                animation->shape_current = 3;
                animation->addBodyPhysicsCylinder(-1 * (animationsPhysics.size() + 2), worldController);
                break;
            case ColliderType::CONE:
                animation->shape_current = 4;
                animation->addBodyPhysicsCone(-1 * (animationsPhysics.size() + 2), worldController);
                break;
            default:
                break;
            }


            animationsPhysics.push_back(animation);
        }

        void addAnimation(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, string const& path, Shader& shader, vector<Texture> textures = {})
        {
            DynamicGameObject* animation = new DynamicGameObject(position, rotation, scale, "assets/animations/" + path, shader, textures);
            animations.push_back(animation);
        }

        void addAnimationInstance(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, string const& path, Shader& shader, int cantidad, glm::mat4* modelMatrices, vector<Texture> textures = {})
        {
            DynamicGameObject* animation = new DynamicGameObject(position, rotation, scale, "assets/animations/" + path, shader, textures, cantidad, modelMatrices);
            animationsInstance.push_back(animation);
        }

        void loadAnimationInstance(const string& path)
        {
            animationsInstance.back()->addAnimation(path);
        }

        void loadAnimation(const string& path)
        {
            animations.back()->addAnimation(path);
        }

        void changeAnimationInstance(int index, int numAnimation)
        {
            int tam = animationsInstance.size();
            if (tam <= index && index < 0) return;
            animationsInstance[index]->currentAnimation = numAnimation;
        }

        void changeAnimation(int index, int numAnimation)
        {
            int tam = animations.size();
            if (tam <= index && index < 0) return;
            animations[index]->currentAnimation = numAnimation;
        }

        void removeModel(Animation*& animation, BulletWorldController* worldController)
        {
            if (!animationsPhysics.empty())
            {
                animation->destroy(worldController);
                animationsPhysics.erase(animationsPhysics.begin() + animation->userIndex);

                for (int i = 0; i < animationsPhysics.size(); i++)
                {
                    animationsPhysics[i]->userIndex = i;
                    animationsPhysics[i]->body->setUserIndex(i);
                }
                animation = NULL;
                delete animation;
            }
        }
    };
}
#endif