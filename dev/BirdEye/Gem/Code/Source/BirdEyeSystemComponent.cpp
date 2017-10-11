
#include "StdAfx.h"

#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>

#include "BirdEyeSystemComponent.h"
#include "System/GameStartup.h"
#include <Core/EditorGame.h>

#include "AzFramework/Input/Devices/Mouse/InputDeviceMouse.h"

namespace BirdEye
{
    void BirdEyeSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<BirdEyeSystemComponent, AZ::Component>()
                ->Version(0)
                ->SerializerForEmptyClass();

            if (AZ::EditContext* ec = serialize->GetEditContext())
            {
                ec->Class<BirdEyeSystemComponent>("BirdEye", "[Description of functionality provided by this System Component]")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    // ->Attribute(AZ::Edit::Attributes::Category, "") Set a category
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("System"))
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true);
            }
        }
    }

    void BirdEyeSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC("EditorGameService"));
    }

    void BirdEyeSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC("EditorGameService"));
    }

    void BirdEyeSystemComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC("CryLegacyService"));
    }

    void BirdEyeSystemComponent::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        (void)dependent;
    }

    IGameStartup* BirdEyeSystemComponent::CreateGameStartup()
    {
        static char buffer[sizeof(GameStartup)];
        return static_cast<IGameStartup*>(new (buffer) GameStartup());
    }

    IEditorGame* BirdEyeSystemComponent::CreateEditorGame()
    {
        GetIEditor()->RegisterNotifyListener(this);
        return new EditorGame();
    }

    const char* BirdEyeSystemComponent::GetGameName() const
    {
        return "BirdEye";
    }

    void BirdEyeSystemComponent::Init()
    {
    }

    void BirdEyeSystemComponent::Activate()
    {
        BirdEyeRequestBus::Handler::BusConnect();
        EditorGameRequestBus::Handler::BusConnect();
        AzFramework::GameEntityContextEventBus::Handler::BusConnect();
    }

    void BirdEyeSystemComponent::Deactivate()
    {
        EditorGameRequestBus::Handler::BusConnect();
        BirdEyeRequestBus::Handler::BusDisconnect();
        AzFramework::GameEntityContextEventBus::Handler::BusDisconnect();
    }

    void BirdEyeSystemComponent::OnGameEntitiesStarted()
    {
        EBUS_EVENT(AzFramework::InputSystemCursorRequestBus, SetSystemCursorState, AzFramework::SystemCursorState::ConstrainedAndVisible);
    }

    void BirdEyeSystemComponent::OnEditorNotifyEvent(EEditorNotifyEvent event)
    {
        if (event == EEditorNotifyEvent::eNotify_OnBeginSimulationMode)
        {
            EBUS_EVENT(AzFramework::InputSystemCursorRequestBus, SetSystemCursorState, AzFramework::SystemCursorState::ConstrainedAndVisible);
        }
    }
}

#ifdef AZ_MONOLITHIC_BUILD
extern "C" {
IGameStartup* CreateGameStartup()
{
    IGameStartup* pGameStartup = nullptr;
    EditorGameRequestBus::BroadcastResult(pGameStartup, &EditorGameRequestBus::Events::CreateGameStartup);
    return pGameStartup;
}
}
#endif
