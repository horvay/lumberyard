
#pragma once
#include "AzCore/Component/TransformBus.h"
#include "AzCore/Component/Component.h"
#include "AzFramework/Input/Events/InputChannelEventListener.h"
#include "AzCore/Component/TickBus.h"
#include "AzFramework/Entity/EntityContext.h"

namespace PhysicsWrapper
{
    struct Hit;
}

namespace TutorialSeries
{
    struct Vector3;

    class TutorialSeriesCharacterComponent
        : public AZ::Component,
          public AzFramework::InputChannelEventListener,
          public AZ::TickBus::Handler,
          public AZ::TransformNotificationBus::Handler
    {
    public:
        AZ_COMPONENT(TutorialSeriesCharacterComponent, "{F52E6197-C72B-4BEF-99CB-FE41C36CF882}");

        ~TutorialSeriesCharacterComponent() override = default;

        static void Reflect(AZ::ReflectContext* reflection);

        void Init() override;
        void Activate() override;
        void Deactivate() override;

        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

        bool OnInputChannelEventFiltered(const AzFramework::InputChannel& inputChannel) override;

    private:
        void OnKeyboardEvent(const AzFramework::InputChannel& inputChannel);
        void OnMouseEvent(const AzFramework::InputChannel& inputChannel);

        void HandleForwardBackwardMovement(AZ::Vector3& desiredVelocity);
        void HandleStrafing(AZ::Vector3& desiredVelocity);

        void PerformRotation(const AzFramework::InputChannel& inputChannel);
        void TrackMouseMovement(const AzFramework::InputChannel::PositionData2D* position_data);

        void CenterCursorPosition();

        // new
        void PlayerUseAction(PhysicsWrapper::Hit &hit);
        void PerformRaycast(const AzFramework::InputChannel &inputChannel);

        AZ::Vector3 GetPlacementPosition(PhysicsWrapper::Hit &hit);

        AZ::Data::Asset<AZ::DynamicPrefabAsset> SliceToSpawn;
        // end new

        const AZ::Quaternion GetCurrentZOrientation();
        const AZ::Quaternion GetCurrentXOrientation();

        AZ::Vector2 m_lastMousePosition{.5f, .5f};
        AZ::Vector2 m_mouseChangeAggregate{0, 0};
        float RotationSpeed = 5.f;

        float MovementScale = 5.0f;

        bool movingForward = false;
        bool movingBack = false;
        bool strafingLeft = false;
        bool strafingRight = false;

        AZ::Entity linkedEntity;
    };
}

