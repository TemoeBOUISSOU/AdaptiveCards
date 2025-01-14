// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#pragma once

#include "TextRun.h"
#include "AdaptiveTextElement.h"

namespace AdaptiveCards::Rendering::Uwp
{
    class DECLSPEC_UUID("d37e5b66-2a5e-4a9e-b087-dbef5a1705b1") AdaptiveTextRun
        : public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::RuntimeClassType::WinRtClassicComMix>,
                                              ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveTextRun,
                                              ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveTextElement,
                                              ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveInline,
                                              Microsoft::WRL::CloakedIid<ITypePeek>,
                                              Microsoft::WRL::CloakedIid<AdaptiveCards::Rendering::Uwp::AdaptiveTextElement>>
    {
        AdaptiveRuntime(AdaptiveTextRun);

    public:
        HRESULT RuntimeClassInitialize() noexcept;
        HRESULT RuntimeClassInitialize(const std::shared_ptr<AdaptiveCards::TextRun>& sharedTextRun) noexcept;

        // IAdaptiveTextRun
        IFACEMETHODIMP get_Highlight(_Out_ boolean* highlight);
        IFACEMETHODIMP put_Highlight(boolean highlight);

        IFACEMETHODIMP get_SelectAction(_COM_Outptr_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveActionElement** action);
        IFACEMETHODIMP put_SelectAction(_In_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveActionElement* action);

        IFACEMETHODIMP get_Italic(_Out_ boolean* italic);
        IFACEMETHODIMP put_Italic(boolean italic);

        IFACEMETHODIMP get_Strikethrough(_Out_ boolean* strikethrough);
        IFACEMETHODIMP put_Strikethrough(boolean strikethrough);

        IFACEMETHODIMP get_Underline(_Out_ boolean* underline);
        IFACEMETHODIMP put_Underline(boolean underline);

        // IAdaptiveTextElement
        IFACEMETHODIMP get_Text(_Outptr_ HSTRING* text) { return AdaptiveTextElement::get_Text(text); }
        IFACEMETHODIMP put_Text(_In_ HSTRING text) { return AdaptiveTextElement::put_Text(text); }

        IFACEMETHODIMP get_Size(_Out_ ABI::AdaptiveCards::Rendering::Uwp::TextSize* textSize)
        {
            return AdaptiveTextElement::get_Size(textSize);
        }
        IFACEMETHODIMP put_Size(ABI::AdaptiveCards::Rendering::Uwp::TextSize textSize)
        {
            return AdaptiveTextElement::put_Size(textSize);
        }

        IFACEMETHODIMP get_Weight(_Out_ ABI::AdaptiveCards::Rendering::Uwp::TextWeight* textWeight)
        {
            return AdaptiveTextElement::get_Weight(textWeight);
        }
        IFACEMETHODIMP put_Weight(ABI::AdaptiveCards::Rendering::Uwp::TextWeight textWeight)
        {
            return AdaptiveTextElement::put_Weight(textWeight);
        }

        IFACEMETHODIMP get_Color(_Out_ ABI::AdaptiveCards::Rendering::Uwp::ForegroundColor* textColor)
        {
            return AdaptiveTextElement::get_Color(textColor);
        }
        IFACEMETHODIMP put_Color(ABI::AdaptiveCards::Rendering::Uwp::ForegroundColor textColor)
        {
            return AdaptiveTextElement::put_Color(textColor);
        }

        IFACEMETHODIMP get_IsSubtle(_Out_ boolean* isSubtle) { return AdaptiveTextElement::get_IsSubtle(isSubtle); }
        IFACEMETHODIMP put_IsSubtle(boolean isSubtle) { return AdaptiveTextElement::put_IsSubtle(isSubtle); }

        IFACEMETHODIMP get_Language(_Outptr_ HSTRING* language) { return AdaptiveTextElement::get_Language(language); }
        IFACEMETHODIMP put_Language(_In_ HSTRING language) { return AdaptiveTextElement::put_Language(language); }

        IFACEMETHODIMP get_FontType(_Out_ ABI::AdaptiveCards::Rendering::Uwp::FontType* type)
        {
            return AdaptiveTextElement::get_FontType(type);
        }
        IFACEMETHODIMP put_FontType(ABI::AdaptiveCards::Rendering::Uwp::FontType type)
        {
            return AdaptiveTextElement::put_FontType(type);
        }

        HRESULT GetSharedModel(std::shared_ptr<AdaptiveCards::TextRun>& sharedModel) noexcept;

        // ITypePeek method
        void* PeekAt(REFIID riid) override { return PeekHelper(riid, this); }

    private:
        Microsoft::WRL::ComPtr<ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveActionElement> m_selectAction;
        boolean m_highlight;
        boolean m_italic;
        boolean m_strikethrough;
        boolean m_underline;
    };

    ActivatableClass(AdaptiveTextRun);
}
