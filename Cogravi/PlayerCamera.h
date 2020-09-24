//#ifndef PLAYER_CAMERA_H
//#define PLAYER_CAMERA_H
//
//#include <GL/glew.h>
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//
//#include <vector>
//
//#include "PlayerController.h"
//
//// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
//class PlayerCamera
//{
//public:
//
//    float distanceFromPlayer = 50.0f;
//    float angleAroundPlayer = 0.0f;
//
//    // Camera Attributes
//    glm::vec3 Position = glm::vec3(0, 0, 0);
//    // Euler Angles
//    float Yaw = 0.0f;
//    float Pitch = 20.0f;
//    float Roll;
//    // Camera options
//    float MovementSpeed;
//    float MouseSensitivity;
//    float Zoom;
//
//    Cogravi::PlayerController* player;
//
//    // Constructor with vectors
//    PlayerCamera(Cogravi::PlayerController *player)
//    {
//        this->player = player;
//    }
//
//    void move(float yoffset, GLFWwindow* window)
//    {
//        calculateZoom(yoffset);
//        calculatePitch(window);
//        calculateAngleAroundPlayer(window);
//        float horizontalDistance = calculateHorizontalDistance();
//        float verticalDistance = calculateVerticallDistance();
//        calculateCameraPosition(horizontalDistance, verticalDistance);
//        //this->Yaw = 180.0f - (player->rotation.y + angleAroundPlayer);
//    }
//
//    void calculateCameraPosition(float horizDistance, float verticDistance)
//    {
//        float theta = player->rotation.y + angleAroundPlayer;
//        float offsetX = horizDistance * glm::sin(glm::radians(theta));
//        float offsetZ = horizDistance * glm::cos(glm::radians(theta));
//        Position.x = player->position.x - offsetX;
//        Position.z = player->position.z - offsetZ;
//        Position.y = player->position.y + verticDistance;
//    }
//
//    float calculateHorizontalDistance()
//    {
//        return distanceFromPlayer * glm::cos(glm::radians(Pitch));
//    }
//    
//    float calculateVerticallDistance()
//    {
//        return distanceFromPlayer * glm::sin(glm::radians(Pitch));
//    }
//
//    void calculateZoom(float yoffset)
//    {
//        float zoomLevel = yoffset * 0.1f;
//        distanceFromPlayer -= zoomLevel;
//    }
//
//    void calculatePitch(GLFWwindow* window)
//    {
//        double x, y;
//        glfwGetCursorPos(window, &x, &y);
//
//        float pitchChange = y * 0.1f;
//        Pitch -= pitchChange;
//    }
//
//    void calculateAngleAroundPlayer(GLFWwindow *window)
//    {
//        double x, y;
//        glfwGetCursorPos(window, &x, &y);
//
//        float angleChange = x * 0.3f;
//        angleAroundPlayer -= angleChange;
//    }
//};
//#endif