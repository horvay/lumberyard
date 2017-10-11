#pragma once
#include "AzCore/Component/Component.h"
#include "AzCore/Component/TickBus.h"
#include "AzFramework/Input/Events/InputChannelEventListener.h"

namespace BirdEye
{
    class BECameraComponent
        : public AZ::Component,
          public AzFramework::InputChannelEventListener,
          public AZ::TickBus::Handler
    {
    public:
        AZ_COMPONENT(BECameraComponent, "{49a33c8d-4df0-4f27-9cd2-65c2ccd91355}")

        ~BECameraComponent() override{};

        static void Reflect(AZ::ReflectContext* reflection);

        void Init() override;
        void Activate() override;
        void Deactivate() override;

    protected:
        bool OnInputChannelEventFiltered(const AzFramework::InputChannel& inputChannel) override;
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

        void OnKeyboardEvent(const AzFramework::InputChannel& inputChannel);
        void OnMouseEvent(const AzFramework::InputChannel& inputChannel);

    private:
        bool movingUp = false;
        bool movingDown = false;
        bool movingLeft = false;
        bool movingRight = false;
       
        bool mouseEdgeUp = false;
        bool mouseEdgeDown = false;
        bool mouseEdgeLeft = false;
        bool mouseEdgeRight = false;

        float movementScale = 5.f;

        float rotationScale = 5.f;
        float currentRotation = 0.f;
        float currentPitch = 0.35f;
        bool rotateLeft = false;
        bool rotateRight = false;

        bool pitchUp = false;
        bool pitchDown = false;

        float zoomDelta = 0.f;
        float zoomScale = 1.f;
        float maxZoom = 1.f;
        float minZoom = 200.f;
    };
}