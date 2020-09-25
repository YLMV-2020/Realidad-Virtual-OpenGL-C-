#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <btBulletDynamicsCommon.h>
#include "BulletWorldController.h"


enum class CameraModes {
    FIRST_PERSON,
    THIRD_PERSON
};

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};
//
//const float YAW = 90.0f;
//const float PITCH = 0.0f;
//const float SPEED = 9.5f;
//const float SENSITIVITY = 0.1f;
//const float ZOOM = 45.0f;

#define UP_FAR_RATIO 0.5f
#define M_PI 3.14159

//#include "Vehicle.h"

class Camera
{
public:

    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;

    float upOffset;
    float farOffset;
    float zoomSpeed;

    int viewLocation;
    int projLocation;
    int width;
    int height;
    float fov;
    float yaw;//angulo de giro en Z
    float pitch;//angulo de giro en Y


    bool isViewChanged;

    CameraModes mode;
    GLuint shader_programme;

    bool isProjChanged;
    glm::mat4 projection;
    glm::mat4 view;


    void getPitchFromQuat(const btQuaternion q1, float& pitch, float& attitude, float& bank) {
        float sqw = q1.w() * q1.w();
        float sqx = q1.x() * q1.x();
        float sqy = q1.y() * q1.y();
        float sqz = q1.z() * q1.z();
        float unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor
        float test = q1.x() * q1.y() + q1.z() * q1.w();
        if (test > 0.499 * unit) { // singularity at north pole
            pitch = 2 * atan2(q1.x(), q1.w());
            attitude = M_PI / 2;
            bank = 0;
            return;
        }
        if (test < -0.499 * unit) { // singularity at south pole
            pitch = -2 * atan2(q1.x(), q1.w());
            attitude = -M_PI / 2;
            bank = 0;
            return;
        }
        pitch = atan2(2 * q1.y() * q1.w() - 2 * q1.x() * q1.z(), sqx - sqy - sqz + sqw);
        attitude = asin(2 * test / unit);
        bank = atan2(2 * q1.x() * q1.w() - 2 * q1.y() * q1.z(), -sqx + sqy - sqz + sqw);

    }

    Camera()
        : isViewChanged(true),
        isProjChanged(false),
        upOffset(15.0f),
        Up(glm::vec3(0, 1, 0)),
        Front(glm::vec3(0.0f, 0.0f, -1.0f)),
        Position(glm::vec3(0.0f, 0.0f, 0.75f)),
        farOffset(30.0f),
        zoomSpeed(0.5f)
    {}


    ~Camera() {}

    void init(int width, int height, float fov, CameraModes m) {

        setWidth(width);
        setHeight(height);
        setFOV(fov);
        setMode(m);

        Position = glm::vec3(-10, 7, -56);
        Front = glm::vec3(579, 0, 5237);
        Up = glm::vec3(0, 1, 0);

      /*  this->shader_programme = shaderProg;
        this->viewLocation = glGetUniformLocation(shaderProg, "view");
        this->projLocation = glGetUniformLocation(shaderProg, "proj");*/

        //glUseProgram(shader_programme);
        projection = glm::perspective(glm::radians(fov), (float)4.0f / (float)3.0f, 0.1f, 10000.0f);
        //glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);


        view = glm::lookAt(Position, Position + Front, Up);
        //glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

    }


    void update(btDiscreteDynamicsWorld*& world, btTransform trans)
    {
        if (isProjChanged) {
            projection = glm::perspective(glm::radians(fov), (float)4.0f / (float)3.0f, 0.1f, 10000.0f);
            glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);
            isProjChanged = false;
        }
        float angle;
        float attitude;
        float bank;
        float camX;
        float camZ;
        //btTransform trans;
        glm::vec3 targetPos;
        glm::vec3 posicionCamara;
        btCollisionWorld::ClosestRayResultCallback* RayCallback;
        glm::vec3 diff;

        switch (mode) {
        case CameraModes::THIRD_PERSON:
            //cout << "3ERA PERSONA\n";
            //if (trans == nullptr) {
            //    printf("Error, target hasn't been set");
            //    return;
            //}

            //target->vehicleDrawableModels[0]->body->getMotionState()->getWorldTransform(*trans);

            getPitchFromQuat(trans.getRotation(), angle, attitude, bank);

            targetPos = glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
            camX = -sin(angle) * farOffset;
            camZ = -cos(angle) * farOffset;
            //AQUI
            posicionCamara = glm::vec3(camX * 2.f, upOffset * 1.5f, camZ * 2.f) + targetPos;

            cout << "x: " << targetPos.x << "\n";
            cout << "y: " << targetPos.y << "\n";
            cout << "z: " << targetPos.z << "\n";

            RayCallback = new btCollisionWorld::ClosestRayResultCallback(btVector3(targetPos.x, targetPos.y + 10, targetPos.z), btVector3(posicionCamara.x, posicionCamara.y, posicionCamara.z));

            world->rayTest(btVector3(targetPos.x, targetPos.y + 10, targetPos.z), btVector3(posicionCamara.x, posicionCamara.y, posicionCamara.z), *RayCallback);
            if (RayCallback->hasHit()) {
                posicionCamara = glm::vec3(RayCallback->m_hitPointWorld.x(), RayCallback->m_hitPointWorld.y(), RayCallback->m_hitPointWorld.z());

                btVector3 pos = RayCallback->m_hitPointWorld;

                cout << "x: " << pos.getX() << "\n";
                cout << "y: " << pos.getY() << "\n";
                cout << "z: " << pos.getZ() << "\n";

                diff = (targetPos - posicionCamara) * (float)1.f / 5.f;
                posicionCamara += diff;
            }

            view = glm::lookAt(posicionCamara, targetPos + glm::vec3(0, 10, 0), Up);
            //glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

            break;
        case CameraModes::FIRST_PERSON:
            view = glm::lookAt(Position, Position + Front, Up);
            //glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
            break;
        }
    }

    void zoomIn() {
        if (farOffset < 15) {
            return;
        }
        this->farOffset = this->farOffset - zoomSpeed;
        this->upOffset = UP_FAR_RATIO * farOffset;
    }
    void zoomOut() {
        if (farOffset > 50) {
            return;
        }
        this->farOffset = this->farOffset + zoomSpeed;
        this->upOffset = UP_FAR_RATIO * farOffset;
    }

    void setWidth(int width) {
        this->width = width;
        this->isProjChanged = true;
    }
    void setHeight(int height) {
        this->height = height;
        this->isProjChanged = true;
    }
    void setFOV(float fov) {
        this->fov = fov;
        this->isProjChanged = true;
    }
    void setMode(CameraModes m) {
        this->mode = m;
    }
    void setPosition(glm::vec3 pos) {
        this->Position = pos;
    }
    void setFront(glm::vec3 fron) {
        this->Front = fron;
    }
    void setUp(glm::vec3 up) {
        this->Up = up;
    }
    void setYaw(float y) {
        this->yaw = y;
    }
    void setPitch(float p) {
        this->pitch = p;
    }

    glm::vec3 getPosition() {
        return this->Position;
    }
    glm::vec3 getFront() {
        return this->Front;
    }
    glm::vec3 getUp() {
        return this->Up;
    }
    float getYaw() {
        return this->yaw;
    }
    float getPitch() {
        return this->pitch;
    }
    CameraModes getMode() {
        return this->mode;
    }





//
//    // Camera Attributes
//    //_-----------------------------------------------------------------------------------------
//  
//    glm::vec3 Right;
//    glm::vec3 WorldUp;
//
//    float distanceFromPlayer = 50.0f;
//    float angleAroundPlayer = 0.0f;
//
//    // Euler Angles
//    float Yaw;
//    float Pitch;
//    // Camera options
//    float MovementSpeed;
//    float MouseSensitivity;
//    float Zoom;
//
//    glm::vec3 positionPlayer;
//    glm::vec3 rotationPlayer;
//
//
//    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
//    {
//        Position = position;
//        WorldUp = up;
//        Yaw = yaw;
//        Pitch = pitch;
//        updateCameraVectors();
//    }
//
//    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
//    {
//        Position = glm::vec3(posX, posY, posZ);
//        WorldUp = glm::vec3(upX, upY, upZ);
//        Yaw = yaw;
//        Pitch = pitch;
//        updateCameraVectors();
//    }
//
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    glm::mat4 GetProjectionMatrix()
    {
        return glm::perspective(glm::radians(fov), (float)WIDTH / (float)HEIGHT, NEAR, FAR);
    }

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
//        //Position.y = 5.f;
//
//    }
//
//    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
//    {
//        xoffset *= MouseSensitivity;
//        yoffset *= MouseSensitivity;
//
//        Yaw += xoffset;
//        Pitch += yoffset;
//
//        if (constrainPitch)
//        {
//            if (Pitch > 89.0f)
//                Pitch = 89.0f;
//            if (Pitch < -89.0f)
//                Pitch = -89.0f;
//        }
//
//        updateCameraVectors();
//    }
//
//  
//    void ProcessMouseScroll(float yoffset)
//    {
//        if (Zoom >= 1.0f && Zoom <= 45.0f)
//            Zoom -= yoffset;
//        if (Zoom <= 1.0f)
//            Zoom = 1.0f;
//        if (Zoom >= 45.0f)
//            Zoom = 45.0f;
//    }
//
//private:
//    
//    void updateCameraVectors()
//    {
//        glm::vec3 front;
//        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
//        front.y = sin(glm::radians(Pitch));
//        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
//        Front = glm::normalize(front);
//       
//        Right = glm::normalize(glm::cross(Front, WorldUp));  
//        Up = glm::normalize(glm::cross(Right, Front));
//    }
};
#endif