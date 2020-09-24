#ifndef AVATAR_HEADER
#define AVATAR_HEADER

#include "AvatarUtil.h"

#include <btBulletDynamicsCommon.h>

namespace Cogravi
{
	class Avatar
	{
	public:

		ovrPosef eyeRenderPose;

		glm::vec3 eyeWorld;
		glm::vec3 eyeForward;
		glm::vec3 eyeUp;
		glm::mat4 view;
		glm::mat4 proj;

		int userIndex = 0;
		float Yaw = 0.0f;

		Vector3f position;
		Quatf rotation;

		Vector3f leftHandPosition;
		Vector3f rightHandPosition;

		Quatf leftHandRotation;
		Quatf rightHandRotation;

		Avatar()
		{
			position = Vector3f(0.0f, 1.7f, 0.0f);
		}

		void Init() {

			// Compile the reference shaders
			char errorBuffer[512];

			
			_skinnedMeshProgram = Util::Instance()->myShaders[ShaderType::AVATAR]->ID;

			//_skinnedMeshProgram = _compileProgramFromFiles("avatar.vert", "avatar.frag", sizeof(errorBuffer), errorBuffer);

			//RDM: Note: don't use Platform for now
			//ovrPlatformInitializeWindows(MIRROR_SAMPLE_APP_ID);
			ovrAvatar_Initialize(MIRROR_SAMPLE_APP_ID);

			//RDM: Note: use hardcoded user id of 0
			//ovrID userID = ovr_GetLoggedInUserID();
			ovrAvatar_RequestAvatarSpecification(0);
		}

		void Prerender(ovrSession session) {

			// Pump avatar messages
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

			// If the avatar is initialized, update it
			if (_avatar)
			{
				// Convert the OVR inputs into Avatar SDK inputs
				ovrInputState touchState;
				ovr_GetInputState(session, ovrControllerType_Active, &touchState);
				ovrTrackingState trackingState = ovr_GetTrackingState(session, 0.0, false);

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

				if (rightStick.x > 0) Yaw -= 0.01f;
				if (rightStick.y > 0) Yaw += 0.01f;

				//RDM: Note: avoid having to manage unwanted params i.e. deltaSeconds, mic, playbackPacket, &playbackTime
				//_updateAvatar(_avatar, deltaSeconds, hmd, inputStateLeft, inputStateRight, mic, playbackPacket, &playbackTime);

				//RDM: Note: avoid updating body for now
				//ovrAvatarPose_UpdateBody(_avatar, hmd);
				ovrAvatarPose_UpdateHands(_avatar, inputStateLeft, inputStateRight);

				ovrAvatarPose_Finalize(_avatar, 0.0);
			}
		}

		void Render(ovrPosef eye, ovrMatrix4f proj) {

			bool renderJoints = false;

			//RDM: Note: make sure hand mesh is drawn the right side up
			glFrontFace(GL_CCW);

			this->eyeRenderPose = eye;

			// Get view and projection matrices
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

			// If we have the avatar and have finished loading assets, render it
			if (_avatar && !_loadingAssets)
			{
				_renderAvatar(_avatar, ovrAvatarVisibilityFlag_FirstPerson, view, this->proj, eyeWorld, renderJoints);
			}

			//RDM: Note: revert - make sure hand mesh is drawn the right side up
			glFrontFace(GL_CW);
		}


	private:

	};
}

#endif