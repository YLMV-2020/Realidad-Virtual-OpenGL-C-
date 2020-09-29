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

        bool firstMouse = true;
        bool isWireframe = false;
        bool isReleased = false;
        bool mouseCursorDisabled = true;

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

            if (abs(xpos - lastX) < 2.f && abs(ypos - lastY) < 2.f)
            {
                return;
            }

            if (firstMouse)
            {
                lastX = xpos;
                lastY = ypos;
                firstMouse = false;
            }

            float xoffset = xpos - lastX;
            float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
            lastX = xpos;
            lastY = ypos;

            if (!mouseCursorDisabled)
            {
                processMouseMovement(xoffset, yoffset);
            }
        }

        void processMouseMovement(float xoffset, float yoffset)
        {
            float sensitivity = 0.1f; // change this value to your liking
            xoffset *= sensitivity;
            yoffset *= sensitivity;

            camera->setYaw(camera->getYaw() + xoffset);
            camera->setPitch(camera->getPitch() + yoffset);
            // make sure that when pitch is out of bounds, screen doesn't get flipped
            if (camera->getPitch() > 89.0f)
                camera->setPitch(89.0f);
            if (camera->getPitch() < -89.0f)
                camera->setPitch(-89.0f);

            glm::vec3 front;
            front.x = cos(glm::radians(camera->getYaw())) * cos(glm::radians(camera->getPitch()));
            front.y = sin(glm::radians(camera->getPitch()));
            front.z = sin(glm::radians(camera->getYaw())) * cos(glm::radians(camera->getPitch()));
            front = glm::normalize(front);

            camera->setFront(front);
        }

        void processInput()
        {

            float cameraSpeed = 1.f;
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                camera->setPosition(camera->getPosition() + cameraSpeed * camera->getFront());
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                camera->setPosition(camera->getPosition() - cameraSpeed * camera->getFront());
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                camera->setPosition(camera->getPosition() - glm::normalize(glm::cross(camera->getFront(), camera->getUp())) * cameraSpeed);
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                camera->setPosition(camera->getPosition() + glm::normalize(glm::cross(camera->getFront(), camera->getUp())) * cameraSpeed);
            if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
            {
                switch (camera->getMode())
                {
                case CameraType::FIRST_PERSON:
                    camera->setMode(CameraType::THIRD_PERSON);
                    break;
                case CameraType::THIRD_PERSON:
                    camera->setMode(CameraType::FIRST_PERSON);
                }
            }
            if (isReleased)
            {
                if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
                {
                    if (!isWireframe)
                    {
                        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                        isWireframe = true;
                    }
                    else
                    {
                        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 
                        isWireframe = false;
                    }
                    isReleased = false;
                }
            }
            if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE)
            {
                isReleased = true;
            }

        }
		

	};
}
#endif