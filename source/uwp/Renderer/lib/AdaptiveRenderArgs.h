// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#pragma once

#include "AdaptiveCards.Rendering.Uwp.h"

namespace AdaptiveCards::Rendering::Uwp
{
    class AdaptiveRenderArgs
        : public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::RuntimeClassType::WinRtClassicComMix>,
                                              ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveRenderArgs>
    {
        AdaptiveRuntime(AdaptiveRenderArgs);

    public:
        AdaptiveRenderArgs() :
            m_isInShowCard(false), m_allowAboveTitleIconPlacement(false), m_ancestorHasFallback(false)
        {
        }

        HRESULT RuntimeClassInitialize() noexcept;

        HRESULT RuntimeClassInitialize(ABI::AdaptiveCards::Rendering::Uwp::ContainerStyle containerStyle,
                                       _In_opt_ IInspectable* parentElement,
                                       _In_opt_ IAdaptiveRenderArgs* renderArgs) noexcept;

        HRESULT RuntimeClassInitialize(ABI::AdaptiveCards::Rendering::Uwp::ContainerStyle containerStyle,
                                       _In_opt_ IInspectable* parentElement,
                                       _In_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveCard* parentCard,
                                       _In_opt_ IAdaptiveRenderArgs* renderArgs) noexcept;

        IFACEMETHODIMP get_ContainerStyle(_Out_ ABI::AdaptiveCards::Rendering::Uwp::ContainerStyle* value) override;
        IFACEMETHODIMP put_ContainerStyle(ABI::AdaptiveCards::Rendering::Uwp::ContainerStyle value) override;

        IFACEMETHODIMP get_ParentElement(_COM_Outptr_ IInspectable** value) override;
        IFACEMETHODIMP put_ParentElement(_In_ IInspectable* value) override;

        IFACEMETHODIMP get_IsInShowCard(_Out_ boolean* isInShowCard) override;
        IFACEMETHODIMP put_IsInShowCard(boolean isInShowCard) override;

        IFACEMETHODIMP get_AllowAboveTitleIconPlacement(_Out_ boolean* value) override;
        IFACEMETHODIMP put_AllowAboveTitleIconPlacement(boolean value) override;

        IFACEMETHODIMP get_AncestorHasFallback(_Out_ boolean* hasFallback);
        IFACEMETHODIMP put_AncestorHasFallback(boolean hasFallback);

        IFACEMETHODIMP get_ParentCard(_COM_Outptr_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveCard** value) override;
        IFACEMETHODIMP put_ParentCard(_In_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveCard* value) override;

    private:
        ABI::AdaptiveCards::Rendering::Uwp::ContainerStyle m_containerStyle;
        Microsoft::WRL::ComPtr<IInspectable> m_parentElement;
        boolean m_isInShowCard;
        boolean m_allowAboveTitleIconPlacement;
        boolean m_ancestorHasFallback;
        Microsoft::WRL::ComPtr<ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveCard> m_parentCard;
    };

    ActivatableClass(AdaptiveRenderArgs);
}
