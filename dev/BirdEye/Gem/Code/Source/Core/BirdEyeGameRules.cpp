
#include "StdAfx.h"
#include "BirdEyeGameRules.h"
#include "IActorSystem.h"

namespace BirdEye
{
    DECLARE_DEFAULT_COMPONENT_FACTORY(BirdEyeGameRules, IGameRules)

    bool BirdEyeGameRules::Init(IGameObject* pGameObject)
    {
        SetGameObject(pGameObject);
        if (!pGameObject->BindToNetwork())
        {
            return false;
        }

        if (gEnv == nullptr ||
            gEnv->pGame == nullptr ||
            gEnv->pGame->GetIGameFramework() == nullptr ||
            gEnv->pGame->GetIGameFramework()->GetIGameRulesSystem() == nullptr)
        {
            return false;
        }
        gEnv->pGame->GetIGameFramework()->GetIGameRulesSystem()->SetCurrentGameRules(this);

        return true;
    }

    void BirdEyeGameRules::PostInit(IGameObject* pGameObject)
    {
        if (pGameObject != nullptr)
        {
            pGameObject->EnableUpdateSlot(this, 0);
            pGameObject->EnablePostUpdates(this);
        }
    }

    BirdEyeGameRules::~BirdEyeGameRules()
    {
        if (gEnv->pGame->GetIGameFramework()->GetIGameRulesSystem())
        {
            gEnv->pGame->GetIGameFramework()->GetIGameRulesSystem()->SetCurrentGameRules(nullptr);
        }
    }

    bool BirdEyeGameRules::OnClientConnect(ChannelId channelId, bool isReset)
    {
        // This is where the actor for a player is created on starting / connecting to the game.
        Vec3 position(0, -5, 34); // Set position because rendering doesn't start until the camera leaves 0,0,0.
        auto pPlayerActor = gEnv->pGame->GetIGameFramework()->GetIActorSystem()->CreateActor(channelId, "Character", "Actor", position, Quat(Ang3(0, 0, 0)), Vec3(1, 1, 1));

        return true;
    }
} // namespace BirdEye
