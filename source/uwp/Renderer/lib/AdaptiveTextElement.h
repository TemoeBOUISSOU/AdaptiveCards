// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#pragma once

namespace AdaptiveCards::Rendering::Uwp
{
    class DECLSPEC_UUID("4BC6640A-8FBE-4DE0-81FD-119BC10877F1") AdaptiveTextElement : public IUnknown
    {
    public:
        IFACEMETHODIMP get_Text(_Outptr_ HSTRING* text);
        IFACEMETHODIMP put_Text(_In_ HSTRING text);

        IFACEMETHODIMP get_Size(_Out_ ABI::AdaptiveCards::Rendering::Uwp::TextSize* textSize);
        IFACEMETHODIMP put_Size(ABI::AdaptiveCards::Rendering::Uwp::TextSize textSize);

        IFACEMETHODIMP get_Weight(_Out_ ABI::AdaptiveCards::Rendering::Uwp::TextWeight* textWeight);
        IFACEMETHODIMP put_Weight(ABI::AdaptiveCards::Rendering::Uwp::TextWeight textWeight);

        IFACEMETHODIMP get_Color(_Out_ ABI::AdaptiveCards::Rendering::Uwp::ForegroundColor* textColor);
        IFACEMETHODIMP put_Color(ABI::AdaptiveCards::Rendering::Uwp::ForegroundColor textColor);

        IFACEMETHODIMP get_IsSubtle(_Out_ boolean* isSubtle);
        IFACEMETHODIMP put_IsSubtle(boolean isSubtle);

        IFACEMETHODIMP get_Language(_Outptr_ HSTRING* language);
        IFACEMETHODIMP put_Language(_In_ HSTRING language);

        IFACEMETHODIMP get_FontType(_Out_ ABI::AdaptiveCards::Rendering::Uwp::FontType* fontType);
        IFACEMETHODIMP put_FontType(ABI::AdaptiveCards::Rendering::Uwp::FontType fontType);

        IFACEMETHODIMP get_Underline(_Out_ boolean* underline);
        IFACEMETHODIMP put_Underline(_In_ boolean underline);

    protected:
        template<typename T> HRESULT InitializeTextElement(const std::shared_ptr<T>& sharedModel)
        {
            m_subtle = sharedModel->GetIsSubtle();
            m_fontType = static_cast<ABI::AdaptiveCards::Rendering::Uwp::FontType>(sharedModel->GetFontType());
            m_textSize = static_cast<ABI::AdaptiveCards::Rendering::Uwp::TextSize>(sharedModel->GetTextSize());
            m_textWeight = static_cast<ABI::AdaptiveCards::Rendering::Uwp::TextWeight>(sharedModel->GetTextWeight());
            m_foregroundColor = static_cast<ABI::AdaptiveCards::Rendering::Uwp::ForegroundColor>(sharedModel->GetTextColor());

            RETURN_IF_FAILED(UTF8ToHString(sharedModel->GetText(), m_text.GetAddressOf()));
            RETURN_IF_FAILED(UTF8ToHString(sharedModel->GetLanguage(), m_language.GetAddressOf()));
            return S_OK;
        }

        template<typename T> HRESULT CopyTextElementProperties(T& sharedCardElement)
        {
            sharedCardElement.SetIsSubtle(m_subtle);
            sharedCardElement.SetFontType(static_cast<AdaptiveCards::FontType>(m_fontType));
            sharedCardElement.SetTextSize(static_cast<AdaptiveCards::TextSize>(m_textSize));
            sharedCardElement.SetTextWeight(static_cast<AdaptiveCards::TextWeight>(m_textWeight));
            sharedCardElement.SetTextColor(static_cast<AdaptiveCards::ForegroundColor>(m_foregroundColor));

            std::string text;
            RETURN_IF_FAILED(HStringToUTF8(m_text.Get(), text));
            sharedCardElement.SetText(text);

            if (!(WindowsIsStringEmpty(m_language.Get())))
            {
                std::string language;
                RETURN_IF_FAILED(HStringToUTF8(m_language.Get(), language));
                sharedCardElement.SetLanguage(language);
            }
            return S_OK;
        }

    private:
        boolean m_subtle;
        Microsoft::WRL::Wrappers::HString m_text;
        Microsoft::WRL::Wrappers::HString m_language;
        ABI::AdaptiveCards::Rendering::Uwp::FontType m_fontType;
        ABI::AdaptiveCards::Rendering::Uwp::TextSize m_textSize;
        ABI::AdaptiveCards::Rendering::Uwp::TextWeight m_textWeight;
        ABI::AdaptiveCards::Rendering::Uwp::ForegroundColor m_foregroundColor;
    };
}
