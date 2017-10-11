#pragma once
#include "AzCore/Component/Component.h"

#include "Buses/CameraController.h"

namespace BirdEye
{
    class BECameraControllerComponent
        : public AZ::Component
        , protected CameraControllerBus::Handler
    {
    public:
        AZ_COMPONENT(BECameraControllerComponent, "{e95a671a-14e5-4137-9776-ef55dde86b03}")

        ~BECameraControllerComponent() override {};

        static void Reflect(AZ::ReflectContext* reflection);

        void Init() override;
        void Activate() override;
        void Deactivate() override;

    protected:
        const AZ::EntityId GetCameraEntityId() override;
    };
}