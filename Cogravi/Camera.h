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
    float distance = 0.1f;
    float distanceUp = 1.0f;

    CameraType mode;

    glm::mat4 projection;
    glm::mat4 view;  

    Camera(btRigidBody* target)
    {
        this->Up = glm::vec3(0, 1, 0);
        this->Front = glm::vec3(0.0f, 0.0f, -1.0f);

        this->Position = glm::vec3(5.0f, 10.0f, 10.75f);

        this->upOffset = 2.30f;
        this->farOffset = 70.0f;
        this->zoomSpeed = 0.5f;

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

            posicionCamara = glm::vec3(camX * distance, upOffset * distanceUp, camZ * distance) + targetPos;
            RayCallback = new btCollisionWorld::ClosestRayResultCallback(btVector3(targetPos.x, targetPos.y + 10, targetPos.z), btVector3(posicionCamara.x, posicionCamara.y, posicionCamara.z));

            world->rayTest(btVector3(targetPos.x, targetPos.y + 10, targetPos.z), btVector3(posicionCamara.x, posicionCamara.y, posicionCamara.z), *RayCallback);
            if (RayCallback->hasHit()) 
            {
                posicionCamara = glm::vec3(RayCallback->m_hitPointWorld.x(), RayCallback->m_hitPointWorld.y(), RayCallback->m_hitPointWorld.z());
                diff = (targetPos - posicionCamara) * (float)1.f / 5.f;
                posicionCamara += diff;
            }
            view = glm::lookAt(posicionCamara, targetPos + glm::vec3(0, 1, 0), Up);
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

//#ifndef CAMERA_H
//#define CAMERA_H
//
//#include <GL/glew.h>
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//
//#include <vector>
//
//// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
//enum Camera_Movement {
//    FORWARD,
//    BACKWARD,
//    LEFT,
//    RIGHT
//};
//
//// Default camera values
//const float YAW = -90.0f;
//const float PITCH = 0.0f;
//const float SPEED = 2.5f;
//const float SENSITIVITY = 0.1f;
//const float ZOOM = 45.0f;
//
//
//// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
//class Camera
//{
//public:
//    // camera Attributes
//    glm::vec3 Position;
//    glm::vec3 Front;
//    glm::vec3 Up;
//    glm::vec3 Right;
//    glm::vec3 WorldUp;
//    // euler Angles
//    float Yaw;
//    float Pitch;
//    // camera options
//    float MovementSpeed;
//    float MouseSensitivity;
//    float Zoom;
//
//    // constructor with vectors
//    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
//    {
//        Position = position;
//        WorldUp = up;
//        Yaw = yaw;
//        Pitch = pitch;
//        updateCameraVectors();
//    }
//    // constructor with scalar values
//    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
//    {
//        Position = glm::vec3(posX, posY, posZ);
//        WorldUp = glm::vec3(upX, upY, upZ);
//        Yaw = yaw;
//        Pitch = pitch;
//        updateCameraVectors();
//    }
//
//    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
//    glm::mat4 GetViewMatrix()
//    {
//        return glm::lookAt(Position, Position + Front, Up);
//    }
//
//    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
//    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
//    {
//        float velocity = MovementSpeed * deltaTime;
//        if (direction == FORWARD)
//            Position += Front * velocity;
//        if (direction == BACKWARD)
//            Position -= Front * velocity;
//        if (direction == LEFT)
//            Position -= Right * velocity;
//        if (direction == RIGHT)
//            Position += Right * velocity;
//    }
//
//    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
//    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
//    {
//        xoffset *= MouseSensitivity;
//        yoffset *= MouseSensitivity;
//
//        Yaw += xoffset;
//        Pitch += yoffset;
//
//        // make sure that when pitch is out of bounds, screen doesn't get flipped
//        if (constrainPitch)
//        {
//            if (Pitch > 89.0f)
//                Pitch = 89.0f;
//            if (Pitch < -89.0f)
//                Pitch = -89.0f;
//        }
//
//        // update Front, Right and Up Vectors using the updated Euler angles
//        updateCameraVectors();
//    }
//
//    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
//    void ProcessMouseScroll(float yoffset)
//    {
//        Zoom -= (float)yoffset;
//        if (Zoom < 1.0f)
//            Zoom = 1.0f;
//        if (Zoom > 45.0f)
//            Zoom = 45.0f;
//    }
//
//private:
//    // calculates the front vector from the Camera's (updated) Euler Angles
//    void updateCameraVectors()
//    {
//        // calculate the new Front vector
//        glm::vec3 front;
//        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
//        front.y = sin(glm::radians(Pitch));
//        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
//        Front = glm::normalize(front);
//        // also re-calculate the Right and Up vector
//        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
//        Up = glm::normalize(glm::cross(Right, Front));
//    }
//};
//#endif