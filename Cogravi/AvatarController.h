#ifndef AVATAR_CONTROLLER_HEADER
#define AVATAR_CONTROLLER_HEADER


namespace Cogravi {

    class AvatarController
    {

    public:

        Avatar* avatar;
        float bulletTime;

        static AvatarController* Instance()
        {
            static AvatarController instance;
            return &instance;
        }

        AvatarController()
        {
            avatar = Avatar::Instance();
        }
       
        void input(ovrSession session)
        {
            if (OVR_SUCCESS(ovr_GetInputState(session, ovrControllerType_Touch, &avatar->touchState)))
            {
                if (avatar->touchState.Buttons == ovrTouch_A)
                {
                    addPokebola(20.0f);
                }
            }
        }

        void addPokebola(float force)
        {
            PokebolaController* pokebolaController = PokebolaController::Instance();
            pokebolaController->addPokebola(_glmFromOvrVector(avatar->leftHandPosition) + avatar->eyeForward, avatar->eyeForward * force);
        }

    };
}
#endif