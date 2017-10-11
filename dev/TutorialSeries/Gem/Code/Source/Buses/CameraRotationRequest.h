#pragma once
#include "AzCore/EBus/EBus.h"
#include "AzCore/Math/Quaternion.h"

namespace TutorialSeries
{
    class CameraRotationRequest
        : public AZ::EBusTraits
    {
    public:
        virtual void SetCameraRotation(AZ::Quaternion) = 0;
    };

    using CameraRotationRequestBus = AZ::EBus<CameraRotationRequest>;
}
