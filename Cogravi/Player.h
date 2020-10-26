#ifndef PLAYER_HEADER
#define PLAYER_HEADER

#include "Animation.h"

namespace Cogravi
{
	class Player :public Animation
	{
		
		float currentSpeed = 0.0f;
		float currentTurnSpeed = 0.0f;

	public:

		float RUN_SPEED = 10.0f;
		float TURN_SPEED = 160.0f;

		Player(Shader& shader, BulletWorldController *worldController):
			Animation(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.01f), "assets/animations/archer/Orc Idle.dae", shader)
		{
			addBodyPhysicsCapsule(111299, worldController);
			shape->setLocalScaling(btVector3(2.0f, 5.0f, 0.0f));

			addAnimation("Female Tough Walk.dae");
			addAnimation("Jump.dae");
			/*addAnimation("Hip Hop Dancing.dae");
			addAnimation("Zombie Walk.dae");
			addAnimation("Ninja Idle.dae");
			addAnimation("Hurricane Kick.dae");
			addAnimation("Great Sword Slash.dae");
			addAnimation("Great Sword Walk.dae");*/

		}

		void render(Camera& camera, Shader& shader, float animationTime) override
		{
			shader.use();
			glm::mat4 projection = camera.GetProjectionMatrix();
			glm::mat4 view = camera.GetViewMatrix();

			transform = glm::mat4(1.0f);

			transform = glm::translate(transform, glm::vec3(position.x, position.y, position.z));

			transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

			transform = glm::scale(transform, scale);

			shader.setMat4("projection", projection);
			shader.setMat4("view", view);
			shader.setMat4("model", transform);

			//glm::mat4 matr_normals_cube = glm::mat4(glm::transpose(glm::inverse(transform)));

			draw(shader, animationTime);
		}

		void update()
		{
			float height = body->getWorldTransform().getOrigin().getY();
			float heightTarget = height > position.y ? height : position.y + 1;

			//position.y = height * 2.0f;

			btTransform target;

			target.setIdentity();
			target.setOrigin(btVector3(position.x, height, position.z));

			btQuaternion quat;
			quat.setEulerZYX(btScalar(glm::radians(rotation.z)), btScalar(glm::radians(rotation.y)), btScalar(glm::radians(rotation.x)));
			target.setRotation(btQuaternion(quat));

			//body->setWorldTransform(target);
			body->setCenterOfMassTransform(target);
		}

		void input(GLFWwindow* window, float deltaTime)
		{

			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			{
				this->currentSpeed = RUN_SPEED;
				currentAnimation = 1;
			}
			else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			{
				currentAnimation = 1;
				this->currentSpeed = -RUN_SPEED;
			}
			else
			{
				currentAnimation = 0;
				this->currentSpeed = 0.0f;
			}

			if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			{
				currentAnimation = 1;
				this->currentTurnSpeed = TURN_SPEED;
			}
			else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			{
				currentAnimation = 1;
				this->currentTurnSpeed = -TURN_SPEED;
			}
			else
			{
				//currentAnimation = 0;
				this->currentTurnSpeed = 0.0f;
			}
			move(deltaTime);
		}

		void move(float deltaTime)
		{
			rotation += glm::vec3(0, currentTurnSpeed * deltaTime, 0);
			float distance = currentSpeed * deltaTime;

			float dx = distance * glm::sin(glm::radians(rotation.y));
			float dz = distance * glm::cos(glm::radians(rotation.y));

			position += glm::vec3(dx, 0, dz);
		}

	};
}
#endif
