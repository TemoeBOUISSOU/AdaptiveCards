// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#pragma once

namespace AdaptiveCards::Rendering::Uwp
{
    class AdaptiveHostConfig
        : public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::RuntimeClassType::WinRtClassicComMix>,
                                              ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveHostConfig>
    {
        AdaptiveRuntime(AdaptiveHostConfig);

    public:
        HRESULT RuntimeClassInitialize() noexcept;
        HRESULT RuntimeClassInitialize(const HostConfig& config);

        // IAdaptiveHostConfig
        IFACEMETHODIMP get_FontFamily(_Outptr_ HSTRING* text);
        IFACEMETHODIMP put_FontFamily(_In_ HSTRING text);

        IFACEMETHODIMP get_FontSizes(_COM_Outptr_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveFontSizesConfig** fontSize);
        IFACEMETHODIMP put_FontSizes(_In_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveFontSizesConfig* fontSize);

        IFACEMETHODIMP get_FontWeights(_COM_Outptr_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveFontWeightsConfig** fontWeight);
        IFACEMETHODIMP put_FontWeights(_In_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveFontWeightsConfig* fontWeight);

        IFACEMETHODIMP get_FontTypes(_COM_Outptr_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveFontTypesDefinition** value);
        IFACEMETHODIMP put_FontTypes(_In_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveFontTypesDefinition* value);

        IFACEMETHODIMP get_SupportsInteractivity(_Out_ boolean* supportsInteractivity);
        IFACEMETHODIMP put_SupportsInteractivity(boolean supportsInteractivity);

        IFACEMETHODIMP get_ImageBaseUrl(_Outptr_ HSTRING* imageBaseUrl);
        IFACEMETHODIMP put_ImageBaseUrl(_In_ HSTRING imageBaseUrl);

        IFACEMETHODIMP get_ContainerStyles(_COM_Outptr_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveContainerStylesDefinition** value);
        IFACEMETHODIMP put_ContainerStyles(_In_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveContainerStylesDefinition* value);

        IFACEMETHODIMP get_ImageSizes(_COM_Outptr_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveImageSizesConfig** imageConfig);
        IFACEMETHODIMP put_ImageSizes(_In_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveImageSizesConfig* imageConfig);

        IFACEMETHODIMP get_Spacing(_COM_Outptr_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveSpacingConfig** spacingConfigConfig);
        IFACEMETHODIMP put_Spacing(_In_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveSpacingConfig* spacingConfigConfig);

        IFACEMETHODIMP get_Separator(_COM_Outptr_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveSeparatorConfig** separatorConfigConfig);
        IFACEMETHODIMP put_Separator(_In_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveSeparatorConfig* separatorConfigConfig);

        IFACEMETHODIMP get_AdaptiveCard(_COM_Outptr_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveCardConfig** adaptiveCardConfigConfig);
        IFACEMETHODIMP put_AdaptiveCard(_In_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveCardConfig* adaptiveCardConfigConfig);

        IFACEMETHODIMP get_ImageSet(_COM_Outptr_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveImageSetConfig** imageSetConfigConfig);
        IFACEMETHODIMP put_ImageSet(_In_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveImageSetConfig* imageSetConfigConfig);

        IFACEMETHODIMP get_Image(_COM_Outptr_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveImageConfig** imageConfigConfig);
        IFACEMETHODIMP put_Image(_In_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveImageConfig* imageConfigConfig);

        IFACEMETHODIMP get_FactSet(_COM_Outptr_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveFactSetConfig** factSetConfigConfig);
        IFACEMETHODIMP put_FactSet(_In_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveFactSetConfig* factSetConfigConfig);

        IFACEMETHODIMP get_Actions(_COM_Outptr_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveActionsConfig** actionsOptionConfig);
        IFACEMETHODIMP put_Actions(_In_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveActionsConfig* actionsOptionConfig);

        IFACEMETHODIMP get_Media(_COM_Outptr_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveMediaConfig** mediaConfig);
        IFACEMETHODIMP put_Media(_In_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveMediaConfig* mediaConfig);

        IFACEMETHODIMP get_Inputs(_COM_Outptr_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveInputsConfig** inputsConfig);
        IFACEMETHODIMP put_Inputs(_In_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveInputsConfig* inputsConfig);

        IFACEMETHODIMP get_Headings(_COM_Outptr_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveHeadingsConfig** headingsConfig);
        IFACEMETHODIMP put_Headings(_In_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveHeadingsConfig* headingsConfig);

    private:
        Microsoft::WRL::Wrappers::HString m_fontFamily;
        boolean m_supportsInteractivity;
        Microsoft::WRL::Wrappers::HString m_imageBaseUrl;

        Microsoft::WRL::ComPtr<ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveFontSizesConfig> m_fontSizes;
        Microsoft::WRL::ComPtr<ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveFontWeightsConfig> m_fontWeights;
        Microsoft::WRL::ComPtr<ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveFontTypesDefinition> m_fontTypes;
        Microsoft::WRL::ComPtr<ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveContainerStylesDefinition> m_containerStyles;
        Microsoft::WRL::ComPtr<ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveImageSizesConfig> m_imageSizes;
        Microsoft::WRL::ComPtr<ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveSpacingConfig> m_spacing;
        Microsoft::WRL::ComPtr<ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveSeparatorConfig> m_separator;
        Microsoft::WRL::ComPtr<ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveCardConfig> m_adaptiveCard;
        Microsoft::WRL::ComPtr<ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveImageSetConfig> m_imageSet;
        Microsoft::WRL::ComPtr<ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveImageConfig> m_image;
        Microsoft::WRL::ComPtr<ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveFactSetConfig> m_factSet;
        Microsoft::WRL::ComPtr<ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveActionsConfig> m_actions;
        Microsoft::WRL::ComPtr<ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveMediaConfig> m_media;
        Microsoft::WRL::ComPtr<ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveInputsConfig> m_inputs;
        Microsoft::WRL::ComPtr<ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveHeadingsConfig> m_headings;
    };

    class AdaptiveHostConfigStaticsImpl WrlFinal
        : public Microsoft::WRL::AgileActivationFactory<ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveHostConfigStatics>
    {
        AdaptiveRuntimeStatic(AdaptiveHostConfig);

    public:
        IFACEMETHOD(ActivateInstance)(_COM_Outptr_ IInspectable** ppvObject) noexcept override
        {
            return Microsoft::WRL::Details::MakeAndInitialize<AdaptiveHostConfig>(ppvObject);
        }

        // IAdaptiveHostConfigStatics
        IFACEMETHODIMP FromJsonString(_In_ HSTRING hostConfigJson,
                                      _COM_Outptr_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveHostConfigParseResult** config) noexcept;
        IFACEMETHODIMP FromJson(_In_ ABI::Windows::Data::Json::IJsonObject* hostConfigJson,
                                _COM_Outptr_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveHostConfigParseResult** config) noexcept;

    private:
        HRESULT _FromJsonString(const std::string& jsonString,
                                _COM_Outptr_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveHostConfigParseResult** config);
    };

    ActivatableClassWithFactory(AdaptiveHostConfig, AdaptiveHostConfigStaticsImpl);
}
