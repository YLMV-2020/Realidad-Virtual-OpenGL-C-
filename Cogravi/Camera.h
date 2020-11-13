#ifndef CAMERA_H
#define CAMERA_H

namespace Cogravi
{
#define UP_FAR_RATIO 0.5f
#define M_PI 3.14159f

    class Camera
    {
    public:

        btRigidBody* target;

        glm::vec3 Position;
        glm::vec3 Front;
        glm::vec3 Up;

        float FOV = 45.0f;
        float NEAR = 0.1f;
        float FAR = 1000.0f;

        float yaw; //angulo de giro en Z
        float pitch; //angulo de giro en Y

        float speed = 1.0f;
        float distanceFromPlayer = 7.0f;
        float distanceUp = 2.30f;

        CameraType mode;

        glm::mat4 projection;
        glm::mat4 view;

        Camera(btRigidBody* target)
        {
            this->Up = glm::vec3(0, 1, 0);
            this->Front = glm::vec3(0.0f, 0.0f, -1.0f);

            this->Position = glm::vec3(5.0f, 10.0f, 10.75f);

            this->projection = glm::perspective(glm::radians(FOV), (float)WIDTH / (float)HEIGHT, NEAR, FAR);
            this->view = glm::lookAt(Position, Position + Front, Up);

            this->mode = CameraType::FIRST_PERSON;
            this->target = target;
        }

        ~Camera() {}

        void getPitchFromQuat(const btQuaternion q1, float& pitch, float& attitude, float& bank)
        {
            float sqw = q1.w() * q1.w();
            float sqx = q1.x() * q1.x();
            float sqy = q1.y() * q1.y();
            float sqz = q1.z() * q1.z();
            float unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor
            float test = q1.x() * q1.y() + q1.z() * q1.w();

            if (test > 0.499 * unit) // singularity at north pole
            {
                pitch = 2 * atan2(q1.x(), q1.w());
                attitude = M_PI / 2;
                bank = 0;
                return;
            }

            if (test < -0.499 * unit) // singularity at south pole
            {
                pitch = -2 * atan2(q1.x(), q1.w());
                attitude = -M_PI / 2;
                bank = 0;
                return;
            }
            pitch = atan2(2 * q1.y() * q1.w() - 2 * q1.x() * q1.z(), sqx - sqy - sqz + sqw);
            attitude = asin(2 * test / unit);
            bank = atan2(2 * q1.x() * q1.w() - 2 * q1.y() * q1.z(), -sqx + sqy - sqz + sqw);

        }

        void update()
        {
            float angle;
            float attitude;
            float bank;
            float camX;
            float camZ;
            btTransform trans;
            glm::vec3 targetPos;
            glm::vec3 posicionCamara;

            switch (mode)
            {

            case CameraType::THIRD_PERSON:

                if (target == nullptr)
                    return;

                target->getMotionState()->getWorldTransform(trans);
                getPitchFromQuat(trans.getRotation(), angle, attitude, bank);
                targetPos = glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());

                camX = -sin(angle) * distanceFromPlayer;
                camZ = -cos(angle) * distanceFromPlayer;

                posicionCamara = glm::vec3(camX , distanceUp, camZ) + targetPos;
                view = glm::lookAt(posicionCamara, targetPos + Front * 3.0f , Up);
                break;

            case CameraType::FIRST_PERSON:

                view = glm::lookAt(Position, Position + Front, Up);
                break;
            }
        }

        void distancePlayer(float yoffset)
        {
            distanceFromPlayer -= yoffset;
        }

        void distanceUpPlayer(float yoffset)
        {
            distanceUp -= yoffset;
        }

        glm::mat4 GetViewMatrix()
        {
            return view;
        }

        glm::mat4 GetProjectionMatrix()
        {
            return projection;
        }

    };
}
#endif
