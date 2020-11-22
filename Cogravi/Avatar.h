#ifndef AVATAR_HEADER
#define AVATAR_HEADER

#include "AvatarUtil.h"

namespace Cogravi
{
	class Avatar
	{

	private:

		glm::mat4 proj;
		glm::mat4 view;

	public:

		ovrPosef eyeRenderPose;

		glm::vec3 eyeWorld;
		glm::vec3 eyeForward;
		glm::vec3 eyeUp;
		
		const int ID_R = 2020;
		const int ID_L = 2021;

		float Yaw = 0.0f;

		Vector3f position;
		Quatf rotation;

		Vector3f leftHandPosition;
		Vector3f rightHandPosition;

		Quatf leftHandRotation;
		Quatf rightHandRotation;

		ovrTrackingState trackingState;
		ovrInputState touchState;

		btRigidBody* body[2];
		btCollisionShape* shape[2];

		float currentSpeed = 0.0f;
		float currentTurnSpeed = 0.0f;

		float RUN_SPEED = 10.0f;
		float TURN_SPEED = 160.0f;

		bool isCollision[2] = { false };

		static Avatar* Instance()
		{
			static Avatar instance;
			return &instance;
		}

		Avatar()
		{
			position = Vector3f(0.0f, 0.0f, 0.0f);
			addBodyPhysicsBox();
		}

		void init() 
		{
			_skinnedMeshProgram = Util::Instance()->myShaders[ShaderType::AVATAR]->ID;
			ovrAvatar_Initialize(MIRROR_SAMPLE_APP_ID);
			ovrAvatar_RequestAvatarSpecification(0);
		}

		void prerender(ovrSession session, float deltaTime) 
		{

			while (ovrAvatarMessage* message = ovrAvatarMessage_Pop())
			{
				switch (ovrAvatarMessage_GetType(message))
				{
				case ovrAvatarMessageType_AvatarSpecification:
					_handleAvatarSpecification(ovrAvatarMessage_GetAvatarSpecification(message));
					break;
				case ovrAvatarMessageType_AssetLoaded:
					_handleAssetLoaded(ovrAvatarMessage_GetAssetLoaded(message));
					break;
				}
				ovrAvatarMessage_Free(message);
			}


			if (_avatar)
			{

				ovr_GetInputState(session, ovrControllerType_Active, &touchState);
				trackingState = ovr_GetTrackingState(session, 0.0, false);

				ovrPosef leftHand = trackingState.HandPoses[ovrHand_Left].ThePose;
				ovrPosef rightHand = trackingState.HandPoses[ovrHand_Right].ThePose;

				Matrix4f rollPitchYaw = Matrix4f::RotationY(Yaw);

				leftHandPosition = position + rollPitchYaw.Transform(leftHand.Position);
				rightHandPosition = position + rollPitchYaw.Transform(rightHand.Position);

				leftHandRotation = Quatf(rollPitchYaw * Matrix4f(leftHand.Orientation));
				rightHandRotation = Quatf(rollPitchYaw * Matrix4f(rightHand.Orientation));

				glm::vec3 hmdP = _glmFromOvrVector(trackingState.HeadPose.ThePose.Position);
				glm::quat hmdQ = _glmFromOvrQuat(trackingState.HeadPose.ThePose.Orientation);

				glm::vec3 leftP =  _glmFromOvrVector(leftHandPosition);
				glm::quat leftQ = _glmFromQuatf(leftHandRotation);

				glm::vec3 rightP =  _glmFromOvrVector(rightHandPosition);
				glm::quat rightQ = _glmFromOvrQuat(rightHandRotation);

				ovrAvatarTransform hmd;
				_ovrAvatarTransformFromGlm(hmdP, hmdQ, glm::vec3(1.0f), &hmd);

				ovrAvatarTransform left;
				_ovrAvatarTransformFromGlm(leftP, leftQ, glm::vec3(1.0f), &left);

				ovrAvatarTransform right;
				_ovrAvatarTransformFromGlm(rightP, rightQ, glm::vec3(1.0f), &right);

				ovrAvatarHandInputState inputStateLeft;
				_ovrAvatarHandInputStateFromOvr(left, touchState, ovrHand_Left, &inputStateLeft);

				ovrAvatarHandInputState inputStateRight;
				_ovrAvatarHandInputStateFromOvr(right, touchState, ovrHand_Right, &inputStateRight);

				Vector2f leftStick = touchState.Thumbstick[ovrHand_Left];
				Vector2f rightStick = touchState.Thumbstick[ovrHand_Right];


				position += Matrix4f::RotationY(Yaw).Transform(
					Vector3f(leftStick.x * leftStick.x * (leftStick.x > 0 ? 0.1f : -0.1f), 0, leftStick.y * leftStick.y * (leftStick.y > 0 ? -0.1f : 0.1f)));

				//input(session);

				if (rightStick.x > 0) Yaw -= 0.01f;
				if (rightStick.y > 0) Yaw += 0.01f;

				ovrAvatarPose_UpdateHands(_avatar, inputStateLeft, inputStateRight);

				ovrAvatarPose_Finalize(_avatar, 0.0);
			}
		}

		
		void input(ovrSession session)
		{
			if (OVR_SUCCESS(ovr_GetInputState(session, ovrControllerType_Touch, &touchState)))
			{
				if (touchState.Buttons == ovrTouch_A)
				{
					

				}
				if (touchState.Buttons == ovrTouch_LButtonMask)
				{
					//isCollision = !isCollision;
					cout << "L button Mask\n";
				}
				/*else
					isCollision = false;*/

				if (touchState.HandTrigger[ovrHand_Left] > 0.5f) {
					//leftHandTriggerPressed = true;
					//cout << "Presionando p\n";
				}
				//cout << "Presionando pdddd\n";
			}
		}

		void render(ovrPosef eye, ovrMatrix4f proj) {

			bool renderJoints = false;

			glFrontFace(GL_CCW);

			this->eyeRenderPose = eye;

			Matrix4f rollPitchYaw = Matrix4f::RotationY(Yaw);
			Matrix4f finalRollPitchYaw = rollPitchYaw * Matrix4f(eye.Orientation);
			Vector3f finalUp = finalRollPitchYaw.Transform(Vector3f(0, 1, 0));
			Vector3f finalForward = finalRollPitchYaw.Transform(Vector3f(0, 0, -1));
			Vector3f shiftedEyePos = position + rollPitchYaw.Transform(eye.Position);

			eyeForward = glm::vec3(finalForward.x, finalForward.y, finalForward.z);
			eyeUp = glm::vec3(finalUp.x, finalUp.y, finalUp.z);
			eyeWorld = glm::vec3(shiftedEyePos.x, shiftedEyePos.y, shiftedEyePos.z);

			view = glm::lookAt(eyeWorld, eyeWorld + eyeForward, eyeUp);

			glm::mat4 aProj(
				proj.M[0][0], proj.M[1][0], proj.M[2][0], proj.M[3][0],
				proj.M[0][1], proj.M[1][1], proj.M[2][1], proj.M[3][1],
				proj.M[0][2], proj.M[1][2], proj.M[2][2], proj.M[3][2],
				proj.M[0][3], proj.M[1][3], proj.M[2][3], proj.M[3][3]
			);

			this->proj = aProj;

			if (_avatar && !_loadingAssets)
			{
				_renderAvatar(_avatar, ovrAvatarVisibilityFlag_FirstPerson, view, this->proj, eyeWorld, renderJoints);
			}

			glFrontFace(GL_CW);
		}

		void update()
		{
			btTransform transform;
			transform.setIdentity();
			transform.setOrigin(btVector3(leftHandPosition.x, leftHandPosition.y, leftHandPosition.z));

			float x, y, z;

			leftHandRotation.GetYawPitchRoll(&x, &y, &z);

			btQuaternion quat;
			quat.setEulerZYX(btScalar(z), btScalar(-y), btScalar(x));
			transform.setRotation(btQuaternion(quat));

			body[0]->setWorldTransform(transform);

			//btTransform transform;
			transform.setIdentity();
			transform.setOrigin(btVector3(rightHandPosition.x, rightHandPosition.y, rightHandPosition.z));


			rightHandRotation.GetYawPitchRoll(&x, &y, &z);

			//btQuaternion quat;
			quat.setEulerZYX(btScalar(z), btScalar(-y), btScalar(x));
			transform.setRotation(btQuaternion(quat));

			body[1]->setWorldTransform(transform);
		}

		void addBodyPhysicsBox()
		{
			this->shape[0] = new btBoxShape(btVector3(0.05f, 0.05f, 0.05f));
			this->shape[1] = new btBoxShape(btVector3(0.05f, 0.05f, 0.05f));
			bodyPhysicsConfiguration();
		}

		void bodyPhysicsConfiguration()
		{
			btTransform transform;
			transform.setIdentity();
			transform.setOrigin(btVector3(0, 0, 0));

			btQuaternion quat;
			quat.setEulerZYX(btScalar(glm::radians(0.0f)), btScalar(glm::radians(0.0f)), btScalar(glm::radians(0.0f)));
			transform.setRotation(btQuaternion(quat));

			btVector3 inertia(0, 0, 0);

			for (int i = 0; i < 2; i++)
			{
				float mass = 1.f;

				shape[i]->calculateLocalInertia(mass, inertia);

				btDefaultMotionState* state = new btDefaultMotionState(transform);
				btRigidBody::btRigidBodyConstructionInfo info(mass, state, shape[i], inertia);

				body[i] = new btRigidBody(info);
				body[i]->setRestitution(1.0f);
				body[i]->setUserIndex(2020 + i);
				body[i]->setLinearFactor(btVector3(1.0f, 1.0f, 1.0f));

				body[i]->setActivationState(DISABLE_DEACTIVATION);

				body[i]->setAngularFactor(btVector3(0, 0, 0));
				body[i]->setGravity(btVector3(0, 1, 0));

				BulletWorldController::Instance()->dynamicsWorld->addRigidBody(body[i]);
			}
		}

		glm::mat4& GetProjectionMatrix()
		{
			return proj;
		}

		glm::mat4& GetViewMatrix()
		{
			return view;
		}
	};
}

#endif