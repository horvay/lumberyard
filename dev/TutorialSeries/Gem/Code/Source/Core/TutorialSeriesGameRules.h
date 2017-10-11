
#pragma once

#include <IGameRulesSystem.h>

namespace TutorialSeries
{
    class TutorialSeriesGameRules
        : public CGameObjectExtensionHelper < TutorialSeriesGameRules, IGameRules >
    {
    public:
        TutorialSeriesGameRules() {}
        virtual ~TutorialSeriesGameRules();

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
} // namespace TutorialSeries