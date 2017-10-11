
#pragma once

#include <AzCore/Component/Component.h>

#include <BirdEye/BirdEyeBus.h>
#include <IEditorGame.h>
#include "System/GameStartup.h"
#include "IEditor.h"
#include "AzFramework/Entity/GameEntityContextBus.h"

namespace BirdEye
{
    class BirdEyeSystemComponent
        : public AZ::Component
        , protected EditorGameRequestBus::Handler
        , protected BirdEyeRequestBus::Handler
        , protected AzFramework::GameEntityContextEventBus::Handler
        , public IEditorNotifyListener
    {
    public:
        AZ_COMPONENT(BirdEyeSystemComponent, "{9E6DBDD2-EE68-4213-BF4A-D2B5FBA5B50D}");

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

    protected:

        void OnGameEntitiesStarted() override;

        ////////////////////////////////////////////////////////////////////////
        // BirdEyeRequestBus interface implementation

        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // EditorGameRequestBus interface implementation
        IGameStartup* CreateGameStartup() override;
        IEditorGame* CreateEditorGame() override;
        const char* GetGameName() const override;
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        ////////////////////////////////////////////////////////////////////////

        void OnEditorNotifyEvent(EEditorNotifyEvent) override;
    };
}
