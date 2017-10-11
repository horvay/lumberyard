
#pragma once

#include <AzCore/Component/Component.h>

#include <TutorialSeries/TutorialSeriesBus.h>
#include <IEditorGame.h>
#include "System/GameStartup.h"

namespace TutorialSeries
{
    class TutorialSeriesSystemComponent
        : public AZ::Component
        , protected EditorGameRequestBus::Handler
        , protected TutorialSeriesRequestBus::Handler
    {
    public:
        AZ_COMPONENT(TutorialSeriesSystemComponent, "{AE1A6CD6-F1CC-4C8B-8564-830BBD067B80}");

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

    protected:
        ////////////////////////////////////////////////////////////////////////
        // TutorialSeriesRequestBus interface implementation

        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // EditorGameRequestBus interface implementation
        IGameStartup* CreateGameStartup() override;
        IEditorGame* CreateEditorGame() override;
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        ////////////////////////////////////////////////////////////////////////
    };
}
