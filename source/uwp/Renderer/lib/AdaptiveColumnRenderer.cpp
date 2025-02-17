// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#include "pch.h"

#include "AdaptiveColumnRenderer.h"

#include "ActionHelpers.h"
#include "AdaptiveColumn.h"
#include "AdaptiveElementParserRegistration.h"
#include "AdaptiveRenderArgs.h"

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace ABI::AdaptiveCards::Rendering::Uwp;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Foundation::Collections;
using namespace ABI::Windows::UI::Xaml;
using namespace ABI::Windows::UI::Xaml::Controls;
using namespace ABI::Windows;

namespace AdaptiveCards::Rendering::Uwp
{
    HRESULT AdaptiveColumnRenderer::RuntimeClassInitialize() noexcept { return S_OK; }

    HRESULT AdaptiveColumnRenderer::Render(_In_ IAdaptiveCardElement* adaptiveCardElement,
                                           _In_ IAdaptiveRenderContext* renderContext,
                                           _In_ IAdaptiveRenderArgs* renderArgs,
                                           _COM_Outptr_ IUIElement** ColumnControl) noexcept
    try
    {
        ComPtr<IAdaptiveCardElement> cardElement(adaptiveCardElement);
        ComPtr<IAdaptiveColumn> adaptiveColumn;
        RETURN_IF_FAILED(cardElement.As(&adaptiveColumn));

        ComPtr<IBorder> columnBorder =
            XamlHelpers::CreateXamlClass<IBorder>(HStringReference(RuntimeClass_Windows_UI_Xaml_Controls_Border));

        ComPtr<WholeItemsPanel> columnPanel;
        RETURN_IF_FAILED(MakeAndInitialize<WholeItemsPanel>(&columnPanel));

        ComPtr<IUIElement> columnPanelAsUIElement;
        RETURN_IF_FAILED(columnPanel.As(&columnPanelAsUIElement));

        RETURN_IF_FAILED(columnBorder->put_Child(columnPanelAsUIElement.Get()));

        ComPtr<IFrameworkElement> columnPanelAsFrameworkElement;
        RETURN_IF_FAILED(columnPanel.As(&columnPanelAsFrameworkElement));

        ComPtr<IReference<bool>> previousContextRtl;
        RETURN_IF_FAILED(renderContext->get_Rtl(&previousContextRtl));
        ComPtr<IReference<bool>> currentRtl = previousContextRtl;

        ComPtr<IReference<bool>> containerRtl;
        RETURN_IF_FAILED(adaptiveColumn->get_Rtl(&containerRtl));

        bool updatedRtl = false;
        if (containerRtl != nullptr)
        {
            currentRtl = containerRtl;
            RETURN_IF_FAILED(renderContext->put_Rtl(currentRtl.Get()));
            updatedRtl = true;
        }

        if (currentRtl)
        {
            boolean rtlValue;
            RETURN_IF_FAILED(currentRtl->get_Value(&rtlValue));

            RETURN_IF_FAILED(columnPanelAsFrameworkElement->put_FlowDirection(rtlValue ? FlowDirection_RightToLeft :
                                                                                         FlowDirection_LeftToRight));
        }

        ComPtr<IAdaptiveContainerBase> columnAsContainerBase;
        RETURN_IF_FAILED(adaptiveColumn.As(&columnAsContainerBase));

        ABI::AdaptiveCards::Rendering::Uwp::ContainerStyle containerStyle;
        RETURN_IF_FAILED(
            XamlHelpers::HandleStylingAndPadding(columnAsContainerBase.Get(), columnBorder.Get(), renderContext, renderArgs, &containerStyle));

        ComPtr<IFrameworkElement> parentElement;
        RETURN_IF_FAILED(renderArgs->get_ParentElement(&parentElement));
        ComPtr<IAdaptiveRenderArgs> newRenderArgs;
        RETURN_IF_FAILED(MakeAndInitialize<AdaptiveRenderArgs>(&newRenderArgs, containerStyle, parentElement.Get(), renderArgs));

        ComPtr<IPanel> columnAsPanel;
        THROW_IF_FAILED(columnPanel.As(&columnAsPanel));

        ComPtr<IVector<IAdaptiveCardElement*>> childItems;
        RETURN_IF_FAILED(adaptiveColumn->get_Items(&childItems));
        RETURN_IF_FAILED(XamlBuilder::BuildPanelChildren(
            childItems.Get(), columnAsPanel.Get(), renderContext, newRenderArgs.Get(), [](IUIElement*) {}));

        // If we changed the context's rtl setting, set it back after rendering the children
        if (updatedRtl)
        {
            RETURN_IF_FAILED(renderContext->put_Rtl(previousContextRtl.Get()));
        }

        ABI::AdaptiveCards::Rendering::Uwp::VerticalContentAlignment verticalContentAlignment;
        RETURN_IF_FAILED(adaptiveColumn->get_VerticalContentAlignment(&verticalContentAlignment));

        XamlHelpers::SetVerticalContentAlignmentToChildren(columnPanel.Get(), verticalContentAlignment);

        // Assign vertical alignment to the top so that on fixed height cards, the content
        // still renders at the top even if the content is shorter than the full card
        RETURN_IF_FAILED(columnPanelAsFrameworkElement->put_VerticalAlignment(VerticalAlignment_Stretch));

        RETURN_IF_FAILED(
            XamlHelpers::SetStyleFromResourceDictionary(renderContext, L"Adaptive.Column", columnPanelAsFrameworkElement.Get()));

        UINT32 columnMinHeight{};
        RETURN_IF_FAILED(columnAsContainerBase->get_MinHeight(&columnMinHeight));
        if (columnMinHeight > 0)
        {
            RETURN_IF_FAILED(columnPanelAsFrameworkElement->put_MinHeight(columnMinHeight));
        }

        ComPtr<IAdaptiveActionElement> selectAction;
        RETURN_IF_FAILED(columnAsContainerBase->get_SelectAction(&selectAction));

        // Define columnAsUIElement based on the existence of a backgroundImage
        ComPtr<IUIElement> columnAsUIElement;
        ComPtr<IAdaptiveBackgroundImage> backgroundImage;
        BOOL backgroundImageIsValid;
        RETURN_IF_FAILED(adaptiveColumn->get_BackgroundImage(&backgroundImage));
        RETURN_IF_FAILED(IsBackgroundImageValid(backgroundImage.Get(), &backgroundImageIsValid));
        if (backgroundImageIsValid)
        {
            ComPtr<IGrid> rootElement =
                XamlHelpers::CreateXamlClass<IGrid>(HStringReference(RuntimeClass_Windows_UI_Xaml_Controls_Grid));
            ComPtr<IPanel> rootAsPanel;
            RETURN_IF_FAILED(rootElement.As(&rootAsPanel));

            XamlHelpers::ApplyBackgroundToRoot(rootAsPanel.Get(), backgroundImage.Get(), renderContext, newRenderArgs.Get());

            // get HeightType for column
            ABI::AdaptiveCards::Rendering::Uwp::HeightType columnHeightType{};
            RETURN_IF_FAILED(cardElement->get_Height(&columnHeightType));

            // Add columnBorder to rootElement
            ComPtr<IFrameworkElement> columnBorderAsFElement;
            RETURN_IF_FAILED(columnBorder.As(&columnBorderAsFElement));
            XamlHelpers::AppendXamlElementToPanel(columnBorderAsFElement.Get(), rootAsPanel.Get(), columnHeightType);

            RETURN_IF_FAILED(rootElement.As(&columnAsUIElement));
        }
        else
        {
            RETURN_IF_FAILED(columnBorder.As(&columnAsUIElement));
        }

        ComPtr<IAdaptiveHostConfig> hostConfig;
        RETURN_IF_FAILED(renderContext->get_HostConfig(&hostConfig));

        ActionHelpers::HandleSelectAction(adaptiveCardElement,
                                          selectAction.Get(),
                                          renderContext,
                                          columnAsUIElement.Get(),
                                          XamlHelpers::SupportsInteractivity(hostConfig.Get()),
                                          false,
                                          ColumnControl);
        return S_OK;
    }
    CATCH_RETURN;

    HRESULT AdaptiveColumnRenderer::FromJson(
        _In_ ABI::Windows::Data::Json::IJsonObject* jsonObject,
        _In_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveElementParserRegistration* elementParserRegistration,
        _In_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveActionParserRegistration* actionParserRegistration,
        _In_ ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::Rendering::Uwp::AdaptiveWarning*>* adaptiveWarnings,
        _COM_Outptr_ ABI::AdaptiveCards::Rendering::Uwp::IAdaptiveCardElement** element) noexcept
    try
    {
        return AdaptiveCards::Rendering::Uwp::FromJson<AdaptiveCards::Rendering::Uwp::AdaptiveColumn, AdaptiveCards::Column, AdaptiveCards::ColumnParser>(
            jsonObject, elementParserRegistration, actionParserRegistration, adaptiveWarnings, element);
    }
    CATCH_RETURN;
}
