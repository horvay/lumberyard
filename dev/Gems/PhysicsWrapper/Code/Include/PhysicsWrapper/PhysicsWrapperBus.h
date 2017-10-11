
#pragma once

#include <AzCore/EBus/EBus.h>
#include "AzCore/Math/Vector3.h"

namespace PhysicsWrapper
{
    struct Hit
    {
        AZ::EntityId entity;
        bool hit = false;
        AZ::Vector3 position{ 0, 0, 0 };
        AZ::Vector3 normal{ 0, 0,0 };
    };

    class PhysicsWrapperRequests
        : public AZ::EBusTraits
    {
    public:
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;

        virtual Hit PerformRayCastWithParam(float distance, int colisionFlags) = 0;
        virtual Hit PerformRayCast() = 0;

        // Public functions
    };

    using PhysicsWrapperRequestBus = AZ::EBus<PhysicsWrapperRequests>;
} // namespace PhysicsWrapper
