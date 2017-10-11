
#include "StdAfx.h"
#include <platform_impl.h>

#include <AzCore/Memory/SystemAllocator.h>

#include "BirdEyeSystemComponent.h"
#include "Components/BECameraComponent.h"
#include "Components/BECameraControllerComponent.h"

#include <IGem.h>

namespace BirdEye
{
    class BirdEyeModule
        : public CryHooksModule
    {
    public:
        AZ_RTTI(BirdEyeModule, "{77C7295C-5B36-4973-B815-25FA82B81DCE}", CryHooksModule);
        AZ_CLASS_ALLOCATOR(BirdEyeModule, AZ::SystemAllocator, 0);

        BirdEyeModule()
            : CryHooksModule()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            m_descriptors.insert(m_descriptors.end(), {
                BirdEyeSystemComponent::CreateDescriptor(),
                BECameraComponent::CreateDescriptor(),
                BECameraControllerComponent::CreateDescriptor(),
            });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<BirdEyeSystemComponent>(),
            };
        }
    };
}

// DO NOT MODIFY THIS LINE UNLESS YOU RENAME THE GEM
// The first parameter should be GemName_GemIdLower
// The second should be the fully qualified name of the class above
AZ_DECLARE_MODULE_CLASS(BirdEye_d2287a4ca11b4571af3e1681d030643d, BirdEye::BirdEyeModule)
