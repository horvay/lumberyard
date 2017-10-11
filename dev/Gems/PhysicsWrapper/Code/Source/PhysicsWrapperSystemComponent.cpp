#include "StdAfx.h"

#include "AzCore/Serialization/SerializeContext.h"
#include "AzCore/Serialization/EditContext.h"
#include "AzCore/std/smart_ptr/unique_ptr.h"
#include "physinterface.h"
#include "IPhysics.h"
#include "MathConversion.h"
#include "CryAction.h"
#include "Cry_Camera.h"

#include "PhysicsWrapperSystemComponent.h"
#include "LmbrCentral/Physics/PhysicsSystemComponentBus.h"

namespace PhysicsWrapper
{
void PhysicsWrapperSystemComponent::Reflect(AZ::ReflectContext *context)
{
    if (AZ::SerializeContext *serialize = azrtti_cast<AZ::SerializeContext *>(context))
    {
        serialize->Class<PhysicsWrapperSystemComponent, AZ::Component>()
            ->Version(0)
            ->SerializerForEmptyClass();

        if (AZ::EditContext *ec = serialize->GetEditContext())
        {
            ec->Class<PhysicsWrapperSystemComponent>("PhysicsWrapper", "[Description of functionality provided by this System Component]")
                ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                ->Attribute(AZ::Edit::Attributes::Category, "PhysicsWrapper")
                ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("System"))
                ->Attribute(AZ::Edit::Attributes::AutoExpand, true);
        }
    }
}

Hit PhysicsWrapperSystemComponent::PerformRayCastWithParam(float distance, int colisionFlags)
{
    auto hit = AZStd::make_unique<ray_hit>();

    auto &cam = GetISystem()->GetViewCamera();

    auto direction = cam.GetViewdir();
    auto start = cam.GetPosition() + direction;

    LmbrCentral::PhysicsSystemRequests::RayCastConfiguration config;
    config.m_origin = LYVec3ToAZVec3(start);
    config.m_direction = LYVec3ToAZVec3(direction);
    config.m_maxHits = 1;
    config.m_maxDistance = distance;
    config.m_piercesSurfacesGreaterThan = colisionFlags;

    LmbrCentral::PhysicsSystemRequests::RayCastResult result;
    EBUS_EVENT_RESULT(result, LmbrCentral::PhysicsSystemRequestBus, RayCast, config);

    Hit phit;

    if (result.GetHitCount() > 0)
    {
        auto hit = result.GetHit(0);
        phit.position = hit->m_position;
        phit.normal = hit->m_normal;

        if (hit->IsValid())
        {
            phit.hit = true;
            phit.entity = hit->m_entityId;
        }
    }

#if !defined(_RELEASE)
    if (auto *pPersistentDebug = gEnv->pGame->GetIGameFramework()->GetIPersistentDebug())
    {
        const ColorF green(0.000f, 1.000f, 0.000f);
        const ColorF red(1.000f, 0.000f, 0.000f);

        pPersistentDebug->Begin("FG_Line", true);

        auto end = start + direction * distance;

        if (!!phit.hit && phit.entity.IsValid())
        {
            pPersistentDebug->AddLine(start, end, green, 500);
        }
        else
        {
            pPersistentDebug->AddLine(start, end, red, 500);
        }
    }
#endif

    return phit;
}

Hit PhysicsWrapperSystemComponent::PerformRayCast()
{
    return PerformRayCastWithParam(100.f, rwi_stop_at_pierceable | rwi_colltype_any);
}

void PhysicsWrapperSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType &provided)
{
    provided.push_back(AZ_CRC("PhysicsWrapperService"));
}

void PhysicsWrapperSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType &incompatible)
{
    incompatible.push_back(AZ_CRC("PhysicsWrapperService"));
}

void PhysicsWrapperSystemComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType &required)
{
    (void)required;
}

void PhysicsWrapperSystemComponent::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType &dependent)
{
    (void)dependent;
}

void PhysicsWrapperSystemComponent::Init()
{
}

void PhysicsWrapperSystemComponent::Activate()
{
    PhysicsWrapperRequestBus::Handler::BusConnect();
}

void PhysicsWrapperSystemComponent::Deactivate()
{
    PhysicsWrapperRequestBus::Handler::BusDisconnect();
}
}
