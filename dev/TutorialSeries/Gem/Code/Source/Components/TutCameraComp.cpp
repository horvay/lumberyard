#include "StdAfx.h"
#include "TutCameraComp.h"
#include "AzCore/Serialization/EditContext.h"
#include "AzCore/Math/Transform.h"
#include "AzCore/Component/TransformBus.h"

using namespace TutorialSeries;

void TutorialSeriesCameraComponent::Reflect(AZ::ReflectContext* reflection)
{
    if (auto serializationContext = azrtti_cast<AZ::SerializeContext*>(reflection))
    {
        serializationContext->Class<TutorialSeriesCameraComponent>()
            ->Version(1);

        if (auto editContext = serializationContext->GetEditContext())
        {
            editContext->Class<TutorialSeriesCameraComponent>("TutorialSeriesCameraComponent", "Camera Component to handle Camera stuffs")
                ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                ->Attribute(AZ::Edit::Attributes::Category, "TutorialSeries")
                ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game"));
        }
    }
}

void TutorialSeriesCameraComponent::Init()
{
}

void TutorialSeriesCameraComponent::Activate()
{
    CameraRotationRequestBus::Handler::BusConnect();
}

void TutorialSeriesCameraComponent::Deactivate()
{
    CameraRotationRequestBus::Handler::BusDisconnect();
}

void TutorialSeriesCameraComponent::SetCameraRotation(AZ::Quaternion rotation)
{
    AZ::Transform entityTransform;
    EBUS_EVENT_ID_RESULT(entityTransform, GetEntityId(), AZ::TransformBus, GetWorldTM);

    entityTransform.SetRotationPartFromQuaternion(rotation);
    EBUS_EVENT_ID(GetEntityId(), AZ::TransformBus, SetWorldTM, entityTransform);
}
