#pragma once
#include "AzCore/Component/Component.h"
#include "Buses/CameraRotationRequest.h"

namespace TutorialSeries
{
    class TutorialSeriesCameraComponent
        : public AZ::Component,
        public CameraRotationRequestBus::Handler
    {
    public:
        AZ_COMPONENT(TutorialSeriesCameraComponent, "{9d5a7b4c-ee6a-4237-99ad-cdb86bc6a598}");

        ~TutorialSeriesCameraComponent() override {};

        static void Reflect(AZ::ReflectContext* reflection);

        void Init() override;
        void Activate() override;
        void Deactivate() override;

        void SetCameraRotation(AZ::Quaternion rotation) override;
    };
}