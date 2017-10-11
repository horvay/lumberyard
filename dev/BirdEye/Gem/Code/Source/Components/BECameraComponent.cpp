#include "StdAfx.h"

#include "BECameraComponent.h"
#include "Buses/CameraController.h"

#include "AzCore/Component/TransformBus.h"
#include "AzCore/Math/Transform.h"
#include "AzCore/Serialization/EditContext.h"
#include "AzFramework/Input/Channels/InputChannel.h"
#include "AzFramework/Input/Devices/Keyboard/InputDeviceKeyboard.h"
#include "AzFramework/Input/Devices/Mouse/InputDeviceMouse.h"
#include "Cry_Math.h"

using namespace BirdEye;
using namespace AzFramework;

void BECameraComponent::Reflect(AZ::ReflectContext* reflection)
{
    if (auto serializationContext = azrtti_cast<AZ::SerializeContext*>(reflection))
    {
        serializationContext->Class<BECameraComponent>()
            ->Version(1)
            ->Field("Movement scale", &BECameraComponent::movementScale)
            ->Field("Rotation scale", &BECameraComponent::rotationScale)
            ->Field("Zoom scale", &BECameraComponent::zoomScale)
            ->Field("Min Zoom", &BECameraComponent::minZoom)
            ->Field("Max Zoom", &BECameraComponent::maxZoom);

        if (auto editContext = serializationContext->GetEditContext())
        {
            editContext->Class<BECameraComponent>("BECameraComponent", "Camera component from bird's eye view")
                ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                ->Attribute(AZ::Edit::Attributes::Category, "BirdEye")
                ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game"))
                ->DataElement(nullptr, &BECameraComponent::movementScale, "Movement scale", "How fast the camera moves")
                ->DataElement(nullptr, &BECameraComponent::rotationScale, "Rotation scale", "How fast the camera rotates")
                ->DataElement(nullptr, &BECameraComponent::zoomScale, "Zoom scale", "How fast the camera zooms")
                ->DataElement(nullptr, &BECameraComponent::minZoom, "Min Zoom", "The minimum zoom possible (how far back can it go?)")
                ->DataElement(nullptr, &BECameraComponent::maxZoom, "Max Zoom", "The maximum zoom possible (how far forward can it go?)");
        }
    }
}

void BECameraComponent::Init()
{
}

void BECameraComponent::Activate()
{
    AZ::TickBus::Handler::BusConnect();
    InputChannelEventListener::Connect();
}

void BECameraComponent::Deactivate()
{
    AZ::TickBus::Handler::BusDisconnect();
    InputChannelEventListener::Disconnect();
}

bool BECameraComponent::OnInputChannelEventFiltered(const AzFramework::InputChannel& inputChannel)
{
    OnMouseEvent(inputChannel);
    OnKeyboardEvent(inputChannel);

    return false;
}

void BECameraComponent::OnKeyboardEvent(const InputChannel& inputChannel)
{
    auto input_type = inputChannel.GetInputChannelId();
    if (input_type == InputDeviceKeyboard::Key::AlphanumericW)
    {
        movingUp = !!inputChannel.GetValue();
    }
    else if (input_type == InputDeviceKeyboard::Key::AlphanumericS)
    {
        movingDown = !!inputChannel.GetValue();
    }
    else if (input_type == InputDeviceKeyboard::Key::AlphanumericA)
    {
        movingLeft = !!inputChannel.GetValue();
    }
    else if (input_type == InputDeviceKeyboard::Key::AlphanumericD)
    {
        movingRight = !!inputChannel.GetValue();
    }
    else if (input_type == InputDeviceKeyboard::Key::AlphanumericQ)
    {
        rotateLeft = !!inputChannel.GetValue();
    }
    else if (input_type == InputDeviceKeyboard::Key::AlphanumericE)
    {
        rotateRight = !!inputChannel.GetValue();
    }
    else if (input_type == InputDeviceKeyboard::Key::NavigationPageUp)
    {
        pitchDown = !!inputChannel.GetValue();
    }
    else if (input_type == InputDeviceKeyboard::Key::NavigationPageDown)
    {
        pitchUp = !!inputChannel.GetValue();
    }
}

void BECameraComponent::OnMouseEvent(const InputChannel& inputChannel)
{
    auto input_type = inputChannel.GetInputChannelId();
    if (input_type == InputDeviceMouse::Button::Left || input_type == InputDeviceMouse::Button::Right)
    {
    }
    else if (input_type == InputDeviceMouse::Movement::Z)
    {
        zoomDelta += inputChannel.GetValue();
    }
    else if (input_type == InputDeviceMouse::SystemCursorPosition)
    {
        mouseEdgeDown = mouseEdgeLeft = mouseEdgeRight = mouseEdgeUp = false;

        if (auto position_data = inputChannel.GetCustomData<InputChannel::PositionData2D>())
        {
            auto position = position_data->m_normalizedPosition;
            auto x = position.GetX();
            auto y = position.GetY();

            mouseEdgeLeft = x <= .01;
            mouseEdgeRight = x >= .99;
            mouseEdgeUp = y <= .01;
            mouseEdgeDown = y >= .99;
        }
    }
}

void BECameraComponent::OnTick(float deltaTime, AZ::ScriptTimePoint time)
{
    AZ::Transform entityTransform;
    EBUS_EVENT_ID_RESULT(entityTransform, GetEntityId(), AZ::TransformBus, GetWorldTM);

    float x = 0.f;
    float y = 0.f;

    if (movingUp || mouseEdgeUp)
        y += movementScale * deltaTime;
    if (movingDown || mouseEdgeDown)
        y -= movementScale * deltaTime;
    if (movingRight || mouseEdgeRight)
        x += movementScale * deltaTime;
    if (movingLeft || mouseEdgeLeft)
        x -= movementScale * deltaTime;

    auto oldRotation = currentRotation;

    if (rotateLeft)
        currentRotation -= rotationScale * deltaTime;
    if (rotateRight)
        currentRotation += rotationScale * deltaTime;

    if (pitchUp)
        currentPitch -= rotationScale * deltaTime;
    if (pitchDown)
        currentPitch += rotationScale * deltaTime;

    if (currentPitch < .10f) currentPitch = .10f;
    if (currentPitch > 1.05f) currentPitch = 1.05f;

    if (rotateLeft || rotateRight || pitchUp || pitchDown)
    {
        AZ::Quaternion xRotation = AZ::Quaternion::CreateRotationX(currentPitch);
        AZ::Quaternion zRotation = AZ::Quaternion::CreateRotationZ(currentRotation);

        entityTransform.SetRotationPartFromQuaternion(zRotation * xRotation);
    }

    if (y != 0.f || x != 0.f)
    {
        auto new_position = entityTransform.GetPosition() + AZ::Quaternion::CreateRotationZ(currentRotation) * AZ::Vector3{x, y, 0.f};
        entityTransform.SetPosition(new_position);
    }

    EBUS_EVENT_ID(GetEntityId(), AZ::TransformBus, SetWorldTM, entityTransform);

    if (zoomDelta != 0)
    {
        AZ::EntityId camera_entity;
        EBUS_EVENT_RESULT(camera_entity, CameraControllerBus, GetCameraEntityId);

        AZ::Transform entityTransform;
        EBUS_EVENT_ID_RESULT(entityTransform, camera_entity, AZ::TransformBus, GetLocalTM);

        const auto& old_position = entityTransform.GetPosition();

        auto newZ = old_position.GetZ() + zoomDelta * zoomScale * deltaTime;
        auto new_position = AZ::Vector3{0.f, 0.f, newZ.GetClamp(maxZoom, minZoom)};

        entityTransform.SetPosition(new_position);
        EBUS_EVENT_ID(camera_entity, AZ::TransformBus, SetLocalTM, entityTransform);

        zoomDelta = 0.f;
    }
}
