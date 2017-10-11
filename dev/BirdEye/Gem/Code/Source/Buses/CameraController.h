#pragma once
#include <AzCore/EBus/EBus.h>

namespace BirdEye
{
    class CameraController
        : public AZ::EBusTraits
    {
    public:
        virtual const AZ::EntityId GetCameraEntityId() = 0;
    };

    using CameraControllerBus = AZ::EBus<CameraController>;
}
