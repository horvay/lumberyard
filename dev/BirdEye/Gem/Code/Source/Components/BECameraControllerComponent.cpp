#include "StdAfx.h"

#include "BECameraControllerComponent.h"

#include "AzCore/Component/TransformBus.h"
#include "AzCore/Math/Transform.h"
#include "AzCore/Serialization/EditContext.h"

using namespace BirdEye;

void BECameraControllerComponent::Reflect(AZ::ReflectContext* reflection)
{
    if (auto serializationContext = azrtti_cast<AZ::SerializeContext*>(reflection))
    {
        serializationContext->Class<BECameraControllerComponent>()
            ->Version(1);
            
        if (auto editContext = serializationContext->GetEditContext())
        {
            editContext->Class<BECameraControllerComponent>("BECameraControllerComponent", "Controller for the camera. This goes on the actual camera.")
                ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                ->Attribute(AZ::Edit::Attributes::Category, "BirdEye")
                ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game"));
        }
    }
}

void BECameraControllerComponent::Init()
{
}

void BECameraControllerComponent::Activate()
{
    CameraControllerBus::Handler::BusConnect();
}

void BECameraControllerComponent::Deactivate()
{
    CameraControllerBus::Handler::BusDisconnect();
}

const AZ::EntityId BECameraControllerComponent::GetCameraEntityId()
{
    return GetEntityId();
}