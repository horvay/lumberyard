
#include "StdAfx.h"
#include <platform_impl.h>

#include <AzCore/Memory/SystemAllocator.h>

#include "TutorialUISystemComponent.h"

#include <IGem.h>

namespace TutorialUI
{
    class TutorialUIModule
        : public CryHooksModule
    {
    public:
        AZ_RTTI(TutorialUIModule, "{B28965B2-F11B-4D97-B918-50F080B5865C}", CryHooksModule);
        AZ_CLASS_ALLOCATOR(TutorialUIModule, AZ::SystemAllocator, 0);

        TutorialUIModule()
            : CryHooksModule()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            m_descriptors.insert(m_descriptors.end(), {
                TutorialUISystemComponent::CreateDescriptor(),
            });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<TutorialUISystemComponent>(),
            };
        }
    };
}

// DO NOT MODIFY THIS LINE UNLESS YOU RENAME THE GEM
// The first parameter should be GemName_GemIdLower
// The second should be the fully qualified name of the class above
AZ_DECLARE_MODULE_CLASS(TutorialUI_114abe624aea4de6a6b011bdbfdea1bd, TutorialUI::TutorialUIModule)
