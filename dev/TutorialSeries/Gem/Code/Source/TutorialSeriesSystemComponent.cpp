
#include "StdAfx.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>

#include "TutorialSeriesSystemComponent.h"
#include <Core/EditorGame.h>
#include "System/GameStartup.h"

namespace TutorialSeries
{
    void TutorialSeriesSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<TutorialSeriesSystemComponent, AZ::Component>()
                ->Version(0)
                ->SerializerForEmptyClass();

            if (AZ::EditContext* ec = serialize->GetEditContext())
            {
                ec->Class<TutorialSeriesSystemComponent>("TutorialSeries", "[Description of functionality provided by this System Component]")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        // ->Attribute(AZ::Edit::Attributes::Category, "") Set a category
                        ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("System"))
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ;
            }
        }
    }

    void TutorialSeriesSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC("EditorGameService"));
    }

    void TutorialSeriesSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC("EditorGameService"));
    }

    void TutorialSeriesSystemComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC("CryLegacyService"));
    }

    void TutorialSeriesSystemComponent::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        (void)dependent;
    }

    IGameStartup* TutorialSeriesSystemComponent::CreateGameStartup()
    {
        static char buffer[sizeof(GameStartup)];
        return static_cast<IGameStartup*>(new(buffer)GameStartup());
    }

    IEditorGame* TutorialSeriesSystemComponent::CreateEditorGame()
    {
        return new EditorGame();
    }
    
    void TutorialSeriesSystemComponent::Init()
    {
    }

    void TutorialSeriesSystemComponent::Activate()
    {
        TutorialSeriesRequestBus::Handler::BusConnect();
        EditorGameRequestBus::Handler::BusConnect();
    }

    void TutorialSeriesSystemComponent::Deactivate()
    {
        EditorGameRequestBus::Handler::BusConnect();
        TutorialSeriesRequestBus::Handler::BusDisconnect();
    }
}

#ifdef AZ_MONOLITHIC_BUILD
extern "C"
{
    IGameStartup* CreateGameStartup()
    {
        IGameStartup* pGameStartup = nullptr;
        EditorGameRequestBus::BroadcastResult(pGameStartup, &EditorGameRequestBus::Events::CreateGameStartup);
        return pGameStartup;
    }
}
#endif
