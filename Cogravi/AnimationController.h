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

        vector<Animation*> animations;

        AnimationController()
        {

        }

        void update()
        {
            for (Animation*& animation : animations)
                animation->update();
        }

        void render(Camera& camera, float animationTime)
        {
            /*for (Animation*& animation : animations)
                animation->render(camera, animationTime);*/
        }

        void render(Avatar& avatar, float animationTime)
        {
            /*for (Animation*& animation : animations)
                animation->render(avatar, animationTime);*/
        }

        void addAnimation(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, string const& path, vector<Texture>& textures, Shader shader, BulletWorldController* worldController)
        {
            /*Animation* animation = new Animation(position, rotation, scale, path, textures, shader);
            animation->addBodyPhysicsBox(-1 * (animations.size() + 2), worldController);
            animations.push_back(animation);*/
        }

        Animation* getAnimation(int index)
        {
            return animations[index];
        }


    };
}
#endif