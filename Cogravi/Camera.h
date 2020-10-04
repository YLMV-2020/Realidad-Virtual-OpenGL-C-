#ifndef CAMERA_H
#define CAMERA_H

enum class CameraType 
{
    FIRST_PERSON,
    THIRD_PERSON
};

#define UP_FAR_RATIO 0.5f
#define M_PI 3.14159

class Camera
{
public:

    btRigidBody* target;

    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;

    float upOffset;
    float farOffset;
    float zoomSpeed;

    float FOV = 45.0f;
    float NEAR = 0.1f;
    float FAR = 1000.0f;

    float yaw; //angulo de giro en Z
    float pitch; //angulo de giro en Y

    float speed = 0.10f;

    CameraType mode;

    glm::mat4 projection;
    glm::mat4 view;  

    Camera(btRigidBody* target)
    {
        this->Up = glm::vec3(0, 1, 0);
        this->Front = glm::vec3(0.0f, 0.0f, -1.0f);

        this->Position = glm::vec3(5.0f, 10.0f, 10.75f);

        this->upOffset = 15.0f;
        this->farOffset = 30.0f;
        this->zoomSpeed = 0.5f;

        this->projection = glm::perspective(glm::radians(FOV), (float)4.0f / (float)3.0f, NEAR, FAR);
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

    void update(btDiscreteDynamicsWorld*& world)
    {
        float angle;
        float attitude;
        float bank;
        float camX;
        float camZ;
        btTransform trans;
        glm::vec3 targetPos;
        glm::vec3 posicionCamara;
        btCollisionWorld::ClosestRayResultCallback* RayCallback;
        glm::vec3 diff;

        switch (mode) 
        {

        case CameraType::THIRD_PERSON:

            if (target == nullptr) 
                return;
        
            target->getMotionState()->getWorldTransform(trans);
            getPitchFromQuat(trans.getRotation(), angle, attitude, bank);
            targetPos = glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
            
            camX = -sin(angle) * farOffset;
            camZ = -cos(angle) * farOffset;

            posicionCamara = glm::vec3(camX * 2.0f, upOffset * 1.5f, camZ * 2.0f) + targetPos;
            RayCallback = new btCollisionWorld::ClosestRayResultCallback(btVector3(targetPos.x, targetPos.y + 10, targetPos.z), btVector3(posicionCamara.x, posicionCamara.y, posicionCamara.z));

            world->rayTest(btVector3(targetPos.x, targetPos.y + 10, targetPos.z), btVector3(posicionCamara.x, posicionCamara.y, posicionCamara.z), *RayCallback);
            if (RayCallback->hasHit()) 
            {
                posicionCamara = glm::vec3(RayCallback->m_hitPointWorld.x(), RayCallback->m_hitPointWorld.y(), RayCallback->m_hitPointWorld.z());
                diff = (targetPos - posicionCamara) * (float)1.f / 5.f;
                posicionCamara += diff;
            }
            view = glm::lookAt(posicionCamara, targetPos + glm::vec3(0, 10, 0), Up);
            break;
        case CameraType::FIRST_PERSON:
            view = glm::lookAt(Position, Position + Front, Up);
            break;
        }
    }

    void zoomIn() 
    {
        if (farOffset < 15) 
        {
            return;
        }
        this->farOffset = this->farOffset - zoomSpeed;
        this->upOffset = UP_FAR_RATIO * farOffset;
    }

    void zoomOut() 
    {
        if (farOffset > 50) 
        {
            return;
        }
        this->farOffset = this->farOffset + zoomSpeed;
        this->upOffset = UP_FAR_RATIO * farOffset;
    }  

    void setMode(CameraType mode) 
    {
        this->mode = mode;
    }

    void setPosition(glm::vec3 pos) 
    {
        this->Position = pos;
    }

    void setFront(glm::vec3 fron) 
    {
        this->Front = fron;
    }

    void setUp(glm::vec3 up) 
    {
        this->Up = up;
    }

    void setYaw(float y) 
    {
        this->yaw = y;
    }

    void setPitch(float p) 
    {
        this->pitch = p;
    }

    void setTarget(btRigidBody* target)
    {
        this->target = target;
    }

    glm::vec3 getPosition() 
    {
        return this->Position;
    }

    glm::vec3 getFront() 
    {
        return this->Front;
    }

    glm::vec3 getUp() 
    {
        return this->Up;
    }

    float getYaw() 
    {
        return this->yaw;
    }

    float getPitch() 
    {
        return this->pitch;
    }

    CameraType getMode() 
    {
        return this->mode;
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
#endif