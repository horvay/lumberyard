
#pragma once

#include <AzCore/Component/Component.h>

#include <TutorialUI/TutorialUIBus.h>
#include "AzFramework/Entity/GameEntityContextBus.h"

namespace TutorialUI
{
    class TutorialUISystemComponent
        : public AZ::Component
        , protected TutorialUIRequestBus::Handler
    {
    public:
        AZ_COMPONENT(TutorialUISystemComponent, "{4AC24C6A-FA1E-4E19-8848-61064914664C}");

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

    protected:
        ////////////////////////////////////////////////////////////////////////
        // TutorialUIRequestBus interface implementation

        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        ////////////////////////////////////////////////////////////////////////
    };
}
