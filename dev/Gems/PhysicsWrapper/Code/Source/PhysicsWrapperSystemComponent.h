
#pragma once

#include <AzCore/Component/Component.h>

#include <PhysicsWrapper/PhysicsWrapperBus.h>

namespace PhysicsWrapper
{
    class PhysicsWrapperSystemComponent
        : public AZ::Component
        , protected PhysicsWrapperRequestBus::Handler
    {
    public:
        AZ_COMPONENT(PhysicsWrapperSystemComponent, "{85082C5C-C2D7-49C4-8C6A-6CD00AFF0760}");

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

    protected:
        ////////////////////////////////////////////////////////////////////////
        // PhysicsWrapperRequestBus interface implementation
        Hit PerformRayCastWithParam(float distance, int colisionFlags);
        Hit PerformRayCast();
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        ////////////////////////////////////////////////////////////////////////
    };
}
