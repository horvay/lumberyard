#include "StdAfx.h"

#include "Components/TutCharacterComp.h"
#include "Buses/CameraRotationRequest.h"
#include "PhysicsWrapper/PhysicsWrapperBus.h"

#include "AzFramework/Input/Devices/Keyboard/InputDeviceKeyboard.h"
#include "AzFramework/Input/Devices/Mouse/InputDeviceMouse.h"
#include "AzCore/Component/ComponentApplicationBus.h"
#include "AzCore/EBus/EBus.h"
#include "AzCore/Math/Transform.h"
#include "AzCore/Serialization/EditContext.h"
#include "AzFramework/Entity/GameEntityContextBus.h"
#include "AzFramework/Input/Channels/InputChannel.h"
#include "LmbrCentral/Physics/CryCharacterPhysicsBus.h"

using namespace TutorialSeries;
using namespace AzFramework;
using namespace PhysicsWrapper;

void TutorialSeriesCharacterComponent::Reflect(AZ::ReflectContext *reflection)
{
    if (auto serializationContext = azrtti_cast<AZ::SerializeContext *>(reflection))
    {
        serializationContext->Class<TutorialSeriesCharacterComponent>()
            ->Version(1)
            ->Field("Movement scale", &TutorialSeriesCharacterComponent::MovementScale)
            ->Field("Rotation Speed", &TutorialSeriesCharacterComponent::RotationSpeed)
            ->Field("Slice To Spawn", &TutorialSeriesCharacterComponent::SliceToSpawn);

        if (auto editContext = serializationContext->GetEditContext())
        {
            editContext->Class<TutorialSeriesCharacterComponent>("TutorialSeriesCharacterComponent", "Main controller component")
                ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                ->Attribute(AZ::Edit::Attributes::Category, "TutorialSeries")
                ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game"))
                ->DataElement(nullptr, &TutorialSeriesCharacterComponent::MovementScale, "Movement scale", "How fast the character moves")
                ->DataElement(nullptr, &TutorialSeriesCharacterComponent::RotationSpeed, "Rotation Speed", "The speed multiplier to apply to mouse rotation")
                ->DataElement(nullptr, &TutorialSeriesCharacterComponent::SliceToSpawn, "Slice To Spawn", "This is the slice that will be spawned when clicking");
        }
    }
}

void TutorialSeriesCharacterComponent::Init()
{
}

void TutorialSeriesCharacterComponent::Activate()
{
    AZ::TickBus::Handler::BusConnect();
    InputChannelEventListener::Connect();

    AZ::TransformNotificationBus::Handler::BusConnect(GetEntityId());
}

void TutorialSeriesCharacterComponent::Deactivate()
{
    AZ::TickBus::Handler::BusDisconnect();
    InputChannelEventListener::Disconnect();
    AZ::TransformNotificationBus::Handler::BusDisconnect();
}

bool TutorialSeriesCharacterComponent::OnInputChannelEventFiltered(const AzFramework::InputChannel &inputChannel)
{
    auto device_id = inputChannel.GetInputDevice().GetInputDeviceId();
    if (device_id == InputDeviceMouse::Id)
    {
        OnMouseEvent(inputChannel);
    }

    if (device_id == InputDeviceKeyboard::Id)
    {
        OnKeyboardEvent(inputChannel);
    }

    return false;
}

void TutorialSeriesCharacterComponent::OnKeyboardEvent(const InputChannel &inputChannel)
{
    auto input_type = inputChannel.GetInputChannelId();
    if (input_type == InputDeviceKeyboard::Key::AlphanumericW)
    {
        movingForward = !!inputChannel.GetValue();
    }
    else if (input_type == InputDeviceKeyboard::Key::AlphanumericS)
    {
        movingBack = !!inputChannel.GetValue();
    }
    else if (input_type == InputDeviceKeyboard::Key::AlphanumericA)
    {
        strafingLeft = !!inputChannel.GetValue();
    }
    else if (input_type == InputDeviceKeyboard::Key::AlphanumericD)
    {
        strafingRight = !!inputChannel.GetValue();
    }
}

void TutorialSeriesCharacterComponent::OnTick(float deltaTime, AZ::ScriptTimePoint time)
{
    AZ::Transform entityTransform;
    EBUS_EVENT_ID_RESULT(entityTransform, GetEntityId(), AZ::TransformBus, GetWorldTM);

    auto x_rotation = GetCurrentXOrientation();
    auto z_rotation = GetCurrentZOrientation();

    entityTransform.SetRotationPartFromQuaternion(z_rotation);
    EBUS_EVENT_ID(GetEntityId(), AZ::TransformBus, SetWorldTM, entityTransform);

    EBUS_EVENT(CameraRotationRequestBus, SetCameraRotation, z_rotation * x_rotation);

    auto desiredVelocity = AZ::Vector3::CreateZero();
    if (movingForward || movingBack || strafingLeft || strafingRight)
    {
        HandleForwardBackwardMovement(desiredVelocity);
        HandleStrafing(desiredVelocity);

        desiredVelocity = z_rotation * desiredVelocity;
    }

    EBUS_EVENT_ID(GetEntityId(), LmbrCentral::CryCharacterPhysicsRequestBus, RequestVelocity, desiredVelocity, 0);
}

const AZ::Quaternion TutorialSeriesCharacterComponent::GetCurrentZOrientation()
{
    auto z_rotation = AZ::Quaternion::CreateRotationZ(m_mouseChangeAggregate.GetX() * RotationSpeed);
    return z_rotation;
}

const AZ::Quaternion TutorialSeriesCharacterComponent::GetCurrentXOrientation()
{
    auto x_rotation = AZ::Quaternion::CreateRotationX(m_mouseChangeAggregate.GetY() * RotationSpeed);
    return x_rotation;
}

void TutorialSeriesCharacterComponent::HandleForwardBackwardMovement(AZ::Vector3 &desiredVelocity)
{
    if (movingBack || movingForward)
    {
        float forward_back_vel = 0;
        if (movingForward)
        {
            forward_back_vel += MovementScale;
        }
        if (movingBack)
        {
            forward_back_vel -= (MovementScale / 2.f);
        }

        desiredVelocity.SetY(forward_back_vel);
    }
}

void TutorialSeriesCharacterComponent::HandleStrafing(AZ::Vector3 &desiredVelocity)
{
    if (strafingLeft || strafingRight)
    {
        float left_right_vel = 0;
        if (strafingRight)
        {
            left_right_vel += MovementScale * .75f;
        }
        if (strafingLeft)
        {
            left_right_vel -= MovementScale * .75f;
        }

        desiredVelocity.SetX(left_right_vel);
    }
}

void TutorialSeriesCharacterComponent::OnMouseEvent(const InputChannel &inputChannel)
{
    auto input_type = inputChannel.GetInputChannelId();
    if (input_type == InputDeviceMouse::SystemCursorPosition)
    {
        PerformRotation(inputChannel);
    }
    else if ((input_type == InputDeviceMouse::Button::Left || input_type == InputDeviceMouse::Button::Right) && !!inputChannel.GetValue())
    {
        PerformRaycast(inputChannel);
    }
}

void TutorialSeriesCharacterComponent::CenterCursorPosition()
{
    EBUS_EVENT(InputSystemCursorRequestBus, SetSystemCursorPositionNormalized, AZ::Vector2{.5f, .5f});
    m_lastMousePosition = AZ::Vector2{.5f, .5f};
}

void TutorialSeriesCharacterComponent::PerformRotation(const InputChannel &inputChannel)
{
    auto position_data = inputChannel.GetCustomData<InputChannel::PositionData2D>();
    TrackMouseMovement(position_data);
    CenterCursorPosition();
}

void TutorialSeriesCharacterComponent::TrackMouseMovement(const InputChannel::PositionData2D *position_data)
{
    auto deltaMousePosition = m_lastMousePosition - position_data->m_normalizedPosition;
    if (deltaMousePosition.GetX() > .5f || deltaMousePosition.GetX() < -.5f || deltaMousePosition.GetY() > .5f || deltaMousePosition.GetY() < -.5f)
        return;

    auto new_y = (m_mouseChangeAggregate + deltaMousePosition).GetY() * RotationSpeed;
    if (new_y > 1.8f || new_y < -1.8f)
    {
        m_mouseChangeAggregate.SetX(m_mouseChangeAggregate.GetX() + deltaMousePosition.GetX());
    }
    else
    {
        m_mouseChangeAggregate += deltaMousePosition;
    }
}

AZ::Vector3 TutorialSeriesCharacterComponent::GetPlacementPosition(PhysicsWrapper::Hit &hit)
{
    auto shift_amount = hit.normal * AZ::Vector3{1.f, 1.f, hit.normal.GetZ() < 0.f ? 2.f : 0.f};
    auto position = shift_amount + hit.position;
    return position;
}

void TutorialSeriesCharacterComponent::PlayerUseAction(PhysicsWrapper::Hit &hit)
{
    auto placement_pos = GetPlacementPosition(hit);

    auto transform = AZ::Transform::CreateTranslation(placement_pos);

    AzFramework::SliceInstantiationTicket ticket;
    EBUS_EVENT_RESULT(ticket, AzFramework::GameEntityContextRequestBus, InstantiateDynamicSlice, SliceToSpawn, transform, nullptr);
}

void TutorialSeriesCharacterComponent::PerformRaycast(const InputChannel &inputChannel)
{
    if (inputChannel.GetState() == InputChannel::State::Began)
    {
        PhysicsWrapper::Hit hit;
        EBUS_EVENT_RESULT(hit, PhysicsWrapper::PhysicsWrapperRequestBus, PerformRayCast);

        if (hit.hit)
        {
            AZ::Entity * entity;
            EBUS_EVENT_RESULT(entity, AZ::ComponentApplicationBus, FindEntity, hit.entity);

            auto input_type = inputChannel.GetInputChannelId();
            if (input_type == InputDeviceMouse::Button::Right)
            {
                PlayerUseAction(hit);
            }
        }
    }
}
