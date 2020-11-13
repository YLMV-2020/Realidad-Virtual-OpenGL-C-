#ifndef INPUT_PROCESSOR_HEADER
#define INPUT_PROCESSOR_HEADER

namespace Cogravi
{
	class InputProcessor
	{
	public:

        GLFWwindow* window;
        Camera* camera;

        int lastX;
        int lastY;

        bool firstMouse = false;
        bool isWireframe = false;
        bool mouseCursorDisabled = false;

        //static InputProcessor* Instance()
        //{
        //    static InputProcessor instance;
        //    return &instance;
        //}

        InputProcessor(GLFWwindow* window, Camera* camera) 
        {
            this->window = window;
            this->camera = camera;

            glfwGetWindowSize(window, &lastX, &lastY);
            lastX = lastX / 2;
            lastY = lastY / 2;
        }

        void processMouse(double xpos, double ypos)
        {
            /*if (abs(xpos - lastX) < 2.f && abs(ypos - lastY) < 2.f)
            {
                return;
            }*/

            if (firstMouse)
            {
                lastX = xpos;
                lastY = ypos;
                firstMouse = false;
            }

            float xoffset = xpos - lastX;
            float yoffset = lastY - ypos; 
            lastX = xpos;
            lastY = ypos;

            if (!mouseCursorDisabled)
                processMouseMovement(xoffset, yoffset);
        }

        void processMouseMovement(float xoffset, float yoffset)
        {
            float sensitivity = 0.1f; 
            xoffset *= sensitivity;
            yoffset *= sensitivity;

            camera->yaw += xoffset;
            camera->pitch += yoffset;

            if (camera->pitch > 89.0f)
                camera->pitch = 89.0f;
            if (camera->pitch < -89.0f)
                camera->pitch = -89.0f;

            glm::vec3 front;
            front.x = cos(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
            front.y = sin(glm::radians(camera->pitch));
            front.z = sin(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
            front = glm::normalize(front);

            camera->Front = front;
        }

        void processInput(float deltaTime)
        {
            float cameraSpeed = camera->speed * deltaTime * 20.0f;
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                camera->Position = camera->Position + cameraSpeed * camera->Front;
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                camera->Position = camera->Position - cameraSpeed * camera->Front;
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                camera->Position = camera->Position - glm::normalize(glm::cross(camera->Front, camera->Up)) * cameraSpeed;
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                camera->Position = camera->Position + glm::normalize(glm::cross(camera->Front, camera->Up)) * cameraSpeed;
        }

	};
}
#endif