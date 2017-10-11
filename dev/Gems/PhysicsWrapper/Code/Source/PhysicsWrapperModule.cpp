
#include "StdAfx.h"
#include <platform_impl.h>

#include <AzCore/Memory/SystemAllocator.h>

#include "PhysicsWrapperSystemComponent.h"

#include <IGem.h>

namespace PhysicsWrapper
{
    class PhysicsWrapperModule
        : public CryHooksModule
    {
    public:
        AZ_RTTI(PhysicsWrapperModule, "{7DC841EB-ABC5-4912-B830-FA83FE422585}", CryHooksModule);
        AZ_CLASS_ALLOCATOR(PhysicsWrapperModule, AZ::SystemAllocator, 0);

        PhysicsWrapperModule()
            : CryHooksModule()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            m_descriptors.insert(m_descriptors.end(), {
                PhysicsWrapperSystemComponent::CreateDescriptor(),
            });
        }
        
        /**
         * Add required SystemComponents to the SystemEntity.
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<PhysicsWrapperSystemComponent>(),
            };
        }
    };
}

// DO NOT MODIFY THIS LINE UNLESS YOU RENAME THE GEM
// The first parameter should be GemName_GemIdLower
// The second should be the fully qualified name of the class above
AZ_DECLARE_MODULE_CLASS(PhysicsWrapper_453b88cf44d041709924810814c70a70, PhysicsWrapper::PhysicsWrapperModule)
