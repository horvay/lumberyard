
#include "StdAfx.h"
#include "platform_impl.h"

#include "AzCore/Memory/SystemAllocator.h"

#include "TutorialSeriesSystemComponent.h"
#include "Components/TutCharacterComp.h"
#include "Components/TutCameraComp.h"

#include "IGem.h"

namespace TutorialSeries
{
    class TutorialSeriesModule
        : public CryHooksModule
    {
    public:
        AZ_RTTI(TutorialSeriesModule, "{33A3448B-6DF2-421F-A106-26FEADC7CBE2}", CryHooksModule);
        AZ_CLASS_ALLOCATOR(TutorialSeriesModule, AZ::SystemAllocator, 0);

        TutorialSeriesModule()
            : CryHooksModule()
        {
            m_descriptors.insert(m_descriptors.end(), {
                TutorialSeriesSystemComponent::CreateDescriptor(),
                TutorialSeriesCharacterComponent::CreateDescriptor(),
                TutorialSeriesCameraComponent::CreateDescriptor()
            });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<TutorialSeriesSystemComponent>(),
            };
        }
    };
}

// DO NOT MODIFY THIS LINE UNLESS YOU RENAME THE GEM
// The first parameter should be GemName_GemIdLower
// The second should be the fully qualified name of the class above
AZ_DECLARE_MODULE_CLASS(TutorialSeries_7a17c44ce76744e1b646e6d8556372c8, TutorialSeries::TutorialSeriesModule)
