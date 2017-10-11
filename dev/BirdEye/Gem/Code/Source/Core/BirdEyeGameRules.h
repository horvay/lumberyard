
#pragma once

#include <IGameRulesSystem.h>

namespace BirdEye
{
    class BirdEyeGameRules
        : public CGameObjectExtensionHelper < BirdEyeGameRules, IGameRules >
    {
    public:
        BirdEyeGameRules() {}
        virtual ~BirdEyeGameRules();

        //////////////////////////////////////////////////////////////////////////
        //! IGameObjectExtension
        bool Init(IGameObject* pGameObject) override;
        void PostInit(IGameObject* pGameObject) override;
        void ProcessEvent(SEntityEvent& event) override { }
        //////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////////
        // IGameRules
        bool OnClientConnect(ChannelId channelId, bool isReset) override;
        //////////////////////////////////////////////////////////////////////////
    };
} // namespace BirdEye