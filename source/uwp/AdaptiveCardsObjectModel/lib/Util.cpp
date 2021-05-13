// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#include "pch.h"

#include <regex>

#include "AdaptiveActionParserRegistration.h"
#include "AdaptiveActionSet.h"
#include "AdaptiveColumn.h"
#include "AdaptiveColumnSet.h"
#include "AdaptiveContainer.h"
#include "AdaptiveChoiceInput.h"
#include "AdaptiveChoiceSetInput.h"
#include "AdaptiveDateInput.h"
#include "AdaptiveElementParserRegistration.h"
#include "AdaptiveExecuteAction.h"
#include "AdaptiveFact.h"
#include "AdaptiveFactSet.h"
#include "AdaptiveFeatureRegistration.h"
#include "AdaptiveImage.h"
#include "AdaptiveImageSet.h"
#include "AdaptiveMedia.h"
#include "AdaptiveMediaSource.h"
#include "AdaptiveNumberInput.h"
#include "AdaptiveOpenUrlAction.h"
#include "AdaptiveRequirement.h"
#include "AdaptiveRichTextBlock.h"
#include "AdaptiveShowCardAction.h"
#include "AdaptiveSubmitAction.h"
#include "AdaptiveTextBlock.h"
#include "AdaptiveTextInput.h"
#include "AdaptiveTextRun.h"
#include "AdaptiveTimeInput.h"
#include "AdaptiveToggleInput.h"
#include "AdaptiveToggleVisibilityTarget.h"
#include "AdaptiveToggleVisibilityAction.h"
#include "AdaptiveUnsupportedAction.h"
#include "AdaptiveUnsupportedElement.h"
#include "AdaptiveWarning.h"
#include "CustomActionWrapper.h"
#include "CustomElementWrapper.h"

using namespace AdaptiveCards;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace ABI::Windows::Data::Json;
using namespace ABI::Windows::UI;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace AdaptiveCards::ObjectModel::Uwp;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Foundation::Collections;

HRESULT WStringToHString(std::wstring_view in, _Outptr_ HSTRING* out) noexcept
try
{
    if (out == nullptr)
    {
        return E_INVALIDARG;
    }
    else if (in.empty())
    {
        return WindowsCreateString(L"", 0, out);
    }
    else
    {
        return WindowsCreateString(in.data(), static_cast<UINT32>(in.length()), out);
    }
}
CATCH_RETURN;

std::string WStringToString(std::wstring_view in)
{
    const int length_in = static_cast<int>(in.length());

    if (length_in > 0)
    {
        const int length_out = ::WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, in.data(), length_in, NULL, 0, NULL, NULL);

        if (length_out > 0)
        {
            std::string out(length_out, '\0');

            const int length_written =
                ::WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, in.data(), length_in, out.data(), length_out, NULL, NULL);

            if (length_written == length_out)
            {
                return out;
            }
        }

        throw bad_string_conversion();
    }

    return {};
}

std::wstring StringToWString(std::string_view in)
{
    const int length_in = static_cast<int>(in.length());

    if (length_in > 0)
    {
        const int length_out = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, in.data(), length_in, NULL, 0);

        if (length_out > 0)
        {
            std::wstring out(length_out, L'\0');

            const int length_written =
                ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, in.data(), length_in, out.data(), length_out);

            if (length_written == length_out)
            {
                return out;
            }
        }

        throw bad_string_conversion();
    }

    return {};
}

HRESULT UTF8ToHString(std::string_view in, _Outptr_ HSTRING* out) noexcept
try
{
    if (out == nullptr)
    {
        return E_INVALIDARG;
    }
    else
    {
        std::wstring wide = StringToWString(in);
        return WindowsCreateString(wide.c_str(), static_cast<UINT32>(wide.length()), out);
    }
}
CATCH_RETURN;

HRESULT HStringToUTF8(HSTRING in, std::string& out) noexcept
try
{
    UINT32 length = 0U;
    const auto* ptr_wide = WindowsGetStringRawBuffer(in, &length);
    out = WStringToString(std::wstring_view(ptr_wide, length));

    return S_OK;
}
CATCH_RETURN;

std::string HStringToUTF8(HSTRING in)
{
    std::string typeAsKey;
    if (SUCCEEDED(HStringToUTF8(in, typeAsKey)))
    {
        return typeAsKey;
    }

    return {};
}

template<typename TSharedBaseType, typename TAdaptiveBaseType, typename TAdaptiveType>
std::shared_ptr<TSharedBaseType> GetSharedModel(_In_ TAdaptiveBaseType* item)
{
    ComPtr<TAdaptiveType> adaptiveElement = PeekInnards<TAdaptiveType>(item);

    std::shared_ptr<TSharedBaseType> sharedModelElement;
    if (adaptiveElement && SUCCEEDED(adaptiveElement->GetSharedModel(sharedModelElement)))
    {
        return sharedModelElement;
    }
    else
    {
        return nullptr;
    }
}

HRESULT GenerateSharedElement(_In_ ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveCardElement* item,
                              std::shared_ptr<AdaptiveCards::BaseCardElement>& baseCardElement)
{
    ABI::AdaptiveCards::ObjectModel::Uwp::ElementType elementType;
    RETURN_IF_FAILED(item->get_ElementType(&elementType));

    switch (elementType)
    {
    case ABI::AdaptiveCards::ObjectModel::Uwp::ElementType::ActionSet:
        baseCardElement =
            GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveCardElement, AdaptiveCards::ObjectModel::Uwp::AdaptiveActionSet>(
                item);
        break;
    case ABI::AdaptiveCards::ObjectModel::Uwp::ElementType::ChoiceSetInput:
        baseCardElement =
            GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveCardElement, AdaptiveCards::ObjectModel::Uwp::AdaptiveChoiceSetInput>(
                item);
        break;
    case ABI::AdaptiveCards::ObjectModel::Uwp::ElementType::Column:
        baseCardElement =
            GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveCardElement, AdaptiveCards::ObjectModel::Uwp::AdaptiveColumn>(
                item);
        break;
    case ABI::AdaptiveCards::ObjectModel::Uwp::ElementType::ColumnSet:
        baseCardElement =
            GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveCardElement, AdaptiveCards::ObjectModel::Uwp::AdaptiveColumnSet>(
                item);
        break;
    case ABI::AdaptiveCards::ObjectModel::Uwp::ElementType::Container:
        baseCardElement =
            GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveCardElement, AdaptiveCards::ObjectModel::Uwp::AdaptiveContainer>(
                item);
        break;
    case ABI::AdaptiveCards::ObjectModel::Uwp::ElementType::DateInput:
        baseCardElement =
            GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveCardElement, AdaptiveCards::ObjectModel::Uwp::AdaptiveDateInput>(
                item);
        break;
    case ABI::AdaptiveCards::ObjectModel::Uwp::ElementType::FactSet:
        baseCardElement =
            GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveCardElement, AdaptiveCards::ObjectModel::Uwp::AdaptiveFactSet>(
                item);
        break;
    case ABI::AdaptiveCards::ObjectModel::Uwp::ElementType::Image:
        baseCardElement =
            GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveCardElement, AdaptiveCards::ObjectModel::Uwp::AdaptiveImage>(
                item);
        break;
    case ABI::AdaptiveCards::ObjectModel::Uwp::ElementType::ImageSet:
        baseCardElement =
            GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveCardElement, AdaptiveCards::ObjectModel::Uwp::AdaptiveImageSet>(
                item);
        break;
    case ABI::AdaptiveCards::ObjectModel::Uwp::ElementType::NumberInput:
        baseCardElement =
            GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveCardElement, AdaptiveCards::ObjectModel::Uwp::AdaptiveNumberInput>(
                item);
        break;
    case ABI::AdaptiveCards::ObjectModel::Uwp::ElementType::Media:
        baseCardElement =
            GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveCardElement, AdaptiveCards::ObjectModel::Uwp::AdaptiveMedia>(
                item);
        break;
    case ABI::AdaptiveCards::ObjectModel::Uwp::ElementType::RichTextBlock:
        baseCardElement =
            GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveCardElement, AdaptiveCards::ObjectModel::Uwp::AdaptiveRichTextBlock>(
                item);
        break;
    case ABI::AdaptiveCards::ObjectModel::Uwp::ElementType::TextBlock:
        baseCardElement =
            GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveCardElement, AdaptiveCards::ObjectModel::Uwp::AdaptiveTextBlock>(
                item);
        break;
    case ABI::AdaptiveCards::ObjectModel::Uwp::ElementType::TextInput:
        baseCardElement =
            GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveCardElement, AdaptiveCards::ObjectModel::Uwp::AdaptiveTextInput>(
                item);
        break;
    case ABI::AdaptiveCards::ObjectModel::Uwp::ElementType::TimeInput:
        baseCardElement =
            GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveCardElement, AdaptiveCards::ObjectModel::Uwp::AdaptiveTimeInput>(
                item);
        break;
    case ABI::AdaptiveCards::ObjectModel::Uwp::ElementType::ToggleInput:
        baseCardElement =
            GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveCardElement, AdaptiveCards::ObjectModel::Uwp::AdaptiveToggleInput>(
                item);
        break;
    case ABI::AdaptiveCards::ObjectModel::Uwp::ElementType::Unsupported:
        baseCardElement =
            GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveCardElement, AdaptiveCards::ObjectModel::Uwp::AdaptiveUnsupportedElement>(
                item);
        break;
    case ABI::AdaptiveCards::ObjectModel::Uwp::ElementType::Custom:
        baseCardElement = std::make_shared<CustomElementWrapper>(item);
        break;
    }

    if (baseCardElement == nullptr)
    {
        return E_INVALIDARG;
    }

    return S_OK;
}

HRESULT GenerateSharedElements(_In_ ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveCardElement*>* items,
                               std::vector<std::shared_ptr<AdaptiveCards::BaseCardElement>>& containedElements)
{
    containedElements.clear();

    IterateOverVector<ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveCardElement>(
        items, [&](ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveCardElement* item) {
            std::shared_ptr<AdaptiveCards::BaseCardElement> baseCardElement;
            RETURN_IF_FAILED(GenerateSharedElement(item, baseCardElement));
            containedElements.push_back(std::move(baseCardElement));

            return S_OK;
        });

    return S_OK;
}

HRESULT GenerateSharedAction(_In_ ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveActionElement* action,
                             std::shared_ptr<AdaptiveCards::BaseActionElement>& sharedAction)
{
    ABI::AdaptiveCards::ObjectModel::Uwp::ActionType actionType;
    RETURN_IF_FAILED(action->get_ActionType(&actionType));

    switch (actionType)
    {
    case ABI::AdaptiveCards::ObjectModel::Uwp::ActionType::OpenUrl:
        sharedAction =
            GetSharedModel<AdaptiveCards::BaseActionElement, ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveActionElement, AdaptiveCards::ObjectModel::Uwp::AdaptiveOpenUrlAction>(
                action);
        break;
    case ABI::AdaptiveCards::ObjectModel::Uwp::ActionType::ShowCard:
        sharedAction =
            GetSharedModel<AdaptiveCards::BaseActionElement, ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveActionElement, AdaptiveCards::ObjectModel::Uwp::AdaptiveShowCardAction>(
                action);
        break;
    case ABI::AdaptiveCards::ObjectModel::Uwp::ActionType::Submit:
        sharedAction =
            GetSharedModel<AdaptiveCards::BaseActionElement, ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveActionElement, AdaptiveCards::ObjectModel::Uwp::AdaptiveSubmitAction>(
                action);
        break;
    case ABI::AdaptiveCards::ObjectModel::Uwp::ActionType::ToggleVisibility:
        sharedAction =
            GetSharedModel<AdaptiveCards::BaseActionElement, ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveActionElement, AdaptiveCards::ObjectModel::Uwp::AdaptiveToggleVisibilityAction>(
                action);
        break;
    case ABI::AdaptiveCards::ObjectModel::Uwp::ActionType::Execute:
        sharedAction =
            GetSharedModel<AdaptiveCards::BaseActionElement, ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveActionElement, AdaptiveCards::ObjectModel::Uwp::AdaptiveExecuteAction>(
                action);
        break;
    case ABI::AdaptiveCards::ObjectModel::Uwp::ActionType::Custom:
        sharedAction = std::make_shared<CustomActionWrapper>(action);
        break;
    case ABI::AdaptiveCards::ObjectModel::Uwp::ActionType::Unsupported:
        sharedAction =
            GetSharedModel<AdaptiveCards::BaseActionElement, ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveActionElement, AdaptiveCards::ObjectModel::Uwp::AdaptiveUnsupportedAction>(
                action);
    }

    return S_OK;
}

HRESULT GenerateSharedActions(_In_ ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveActionElement*>* actions,
                              std::vector<std::shared_ptr<AdaptiveCards::BaseActionElement>>& containedElements)
{
    containedElements.clear();

    IterateOverVector<ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveActionElement>(
        actions, [&](ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveActionElement* action) {
            std::shared_ptr<AdaptiveCards::BaseActionElement> baseActionElement;
            GenerateSharedAction(action, baseActionElement);
            containedElements.push_back(baseActionElement);
            return S_OK;
        });

    return S_OK;
}

HRESULT GenerateSharedRequirements(
    _In_ ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::ObjectModel::Uwp::AdaptiveRequirement*>* adaptiveRequirements,
    std::unordered_map<std::string, AdaptiveCards::SemanticVersion>& sharedRequirements) noexcept
try
{
    sharedRequirements.clear();

    IterateOverVector<ABI::AdaptiveCards::ObjectModel::Uwp::AdaptiveRequirement, ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveRequirement>(
        adaptiveRequirements, [&](ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveRequirement* requirement) {
            HString nameHString;
            RETURN_IF_FAILED(requirement->get_Name(nameHString.GetAddressOf()));

            HString versionHString;
            RETURN_IF_FAILED(requirement->get_Version(versionHString.GetAddressOf()));

            std::string nameString;
            RETURN_IF_FAILED(HStringToUTF8(nameHString.Get(), nameString));

            std::string versionString;
            RETURN_IF_FAILED(HStringToUTF8(versionHString.Get(), versionString));

            if (versionString == "*")
            {
                sharedRequirements.emplace(std::move(nameString), "0");
            }
            else
            {
                sharedRequirements.emplace(std::move(nameString), std::move(versionString));
            }

            return S_OK;
        });

    return S_OK;
}
CATCH_RETURN;

HRESULT GenerateSharedImages(_In_ ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::ObjectModel::Uwp::AdaptiveImage*>* images,
                             std::vector<std::shared_ptr<AdaptiveCards::Image>>& containedElements)
{
    containedElements.clear();

    IterateOverVector<ABI::AdaptiveCards::ObjectModel::Uwp::AdaptiveImage, ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveImage>(
        images, [&](ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveImage* image) {
            ComPtr<ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveImage> localImage = image;
            ComPtr<ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveCardElement> imageAsElement;
            localImage.As(&imageAsElement);

            std::shared_ptr<AdaptiveCards::BaseCardElement> sharedImage =
                GetSharedModel<AdaptiveCards::BaseCardElement, ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveCardElement, AdaptiveCards::ObjectModel::Uwp::AdaptiveImage>(
                    imageAsElement.Get());
            containedElements.push_back(std::AdaptivePointerCast<AdaptiveCards::Image>(sharedImage));

            return S_OK;
        });

    return S_OK;
}

HRESULT GenerateSharedFacts(_In_ ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::ObjectModel::Uwp::AdaptiveFact*>* facts,
                            std::vector<std::shared_ptr<AdaptiveCards::Fact>>& containedElements)
{
    containedElements.clear();

    IterateOverVector<ABI::AdaptiveCards::ObjectModel::Uwp::AdaptiveFact, ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveFact>(
        facts, [&](ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveFact* fact) {
            ComPtr<AdaptiveCards::ObjectModel::Uwp::AdaptiveFact> adaptiveElement =
                PeekInnards<AdaptiveCards::ObjectModel::Uwp::AdaptiveFact>(fact);
            if (adaptiveElement == nullptr)
            {
                return E_INVALIDARG;
            }

            std::shared_ptr<AdaptiveCards::Fact> sharedFact;
            RETURN_IF_FAILED(adaptiveElement->GetSharedModel(sharedFact));
            containedElements.push_back(std::AdaptivePointerCast<AdaptiveCards::Fact>(sharedFact));
            return S_OK;
        });

    return S_OK;
}

HRESULT GenerateSharedChoices(_In_ ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::ObjectModel::Uwp::AdaptiveChoiceInput*>* choices,
                              std::vector<std::shared_ptr<AdaptiveCards::ChoiceInput>>& containedElements)
{
    containedElements.clear();

    IterateOverVector<ABI::AdaptiveCards::ObjectModel::Uwp::AdaptiveChoiceInput, ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveChoiceInput>(
        choices, [&](ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveChoiceInput* choice) {
            ComPtr<AdaptiveCards::ObjectModel::Uwp::AdaptiveChoiceInput> adaptiveElement =
                PeekInnards<AdaptiveCards::ObjectModel::Uwp::AdaptiveChoiceInput>(choice);
            if (adaptiveElement == nullptr)
            {
                return E_INVALIDARG;
            }

            std::shared_ptr<AdaptiveCards::ChoiceInput> sharedChoice;
            RETURN_IF_FAILED(adaptiveElement->GetSharedModel(sharedChoice));
            containedElements.push_back(std::AdaptivePointerCast<AdaptiveCards::ChoiceInput>(sharedChoice));
            return S_OK;
        });

    return S_OK;
}

HRESULT GenerateSharedMediaSources(
    _In_ ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::ObjectModel::Uwp::AdaptiveMediaSource*>* sources,
    std::vector<std::shared_ptr<AdaptiveCards::MediaSource>>& containedElements)
{
    containedElements.clear();

    ComPtr<ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::ObjectModel::Uwp::AdaptiveMediaSource*>> localSources(sources);
    ComPtr<IIterable<ABI::AdaptiveCards::ObjectModel::Uwp::AdaptiveMediaSource*>> vectorIterable;
    THROW_IF_FAILED(localSources.As<IIterable<ABI::AdaptiveCards::ObjectModel::Uwp::AdaptiveMediaSource*>>(&vectorIterable));

    Microsoft::WRL::ComPtr<IIterator<ABI::AdaptiveCards::ObjectModel::Uwp::AdaptiveMediaSource*>> vectorIterator;
    HRESULT hr = vectorIterable->First(&vectorIterator);

    boolean hasCurrent;
    THROW_IF_FAILED(vectorIterator->get_HasCurrent(&hasCurrent));

    while (SUCCEEDED(hr) && hasCurrent)
    {
        ComPtr<ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveMediaSource> source;
        THROW_IF_FAILED(vectorIterator->get_Current(&source));

        ComPtr<AdaptiveCards::ObjectModel::Uwp::AdaptiveMediaSource> adaptiveElement =
            PeekInnards<AdaptiveCards::ObjectModel::Uwp::AdaptiveMediaSource>(source);
        if (adaptiveElement == nullptr)
        {
            return E_INVALIDARG;
        }

        std::shared_ptr<AdaptiveCards::MediaSource> sharedSource;
        RETURN_IF_FAILED(adaptiveElement->GetSharedModel(sharedSource));
        containedElements.push_back(std::AdaptivePointerCast<AdaptiveCards::MediaSource>(sharedSource));

        hr = vectorIterator->MoveNext(&hasCurrent);
    }

    return S_OK;
}

HRESULT GenerateSharedInlines(ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveInline*>* inlines,
                              std::vector<std::shared_ptr<AdaptiveCards::Inline>>& containedElements)
{
    containedElements.clear();

    IterateOverVector<ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveInline, ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveInline>(
        inlines, [&](ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveInline* thisInline) {
            // We only support text runs for now
            ComPtr<AdaptiveCards::ObjectModel::Uwp::AdaptiveTextRun> adaptiveTextRun =
                PeekInnards<AdaptiveCards::ObjectModel::Uwp::AdaptiveTextRun>(thisInline);
            if (adaptiveTextRun == nullptr)
            {
                return E_INVALIDARG;
            }

            std::shared_ptr<AdaptiveCards::TextRun> sharedTextRun;
            RETURN_IF_FAILED(adaptiveTextRun->GetSharedModel(sharedTextRun));
            containedElements.push_back(std::AdaptivePointerCast<AdaptiveCards::Inline>(sharedTextRun));
            return S_OK;
        });

    return S_OK;
}

HRESULT GenerateSharedToggleElements(
    _In_ ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::ObjectModel::Uwp::AdaptiveToggleVisibilityTarget*>* targets,
    std::vector<std::shared_ptr<AdaptiveCards::ToggleVisibilityTarget>>& containedElements)
{
    containedElements.clear();

    ComPtr<ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::ObjectModel::Uwp::AdaptiveToggleVisibilityTarget*>> localTargets(
        targets);
    ComPtr<IIterable<ABI::AdaptiveCards::ObjectModel::Uwp::AdaptiveToggleVisibilityTarget*>> vectorIterable;
    THROW_IF_FAILED(localTargets.As<IIterable<ABI::AdaptiveCards::ObjectModel::Uwp::AdaptiveToggleVisibilityTarget*>>(&vectorIterable));

    Microsoft::WRL::ComPtr<IIterator<ABI::AdaptiveCards::ObjectModel::Uwp::AdaptiveToggleVisibilityTarget*>> vectorIterator;
    HRESULT hr = vectorIterable->First(&vectorIterator);

    boolean hasCurrent;
    THROW_IF_FAILED(vectorIterator->get_HasCurrent(&hasCurrent));

    while (SUCCEEDED(hr) && hasCurrent)
    {
        ComPtr<ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveToggleVisibilityTarget> target;
        THROW_IF_FAILED(vectorIterator->get_Current(&target));

        ComPtr<AdaptiveCards::ObjectModel::Uwp::AdaptiveToggleVisibilityTarget> adaptiveElement =
            PeekInnards<AdaptiveCards::ObjectModel::Uwp::AdaptiveToggleVisibilityTarget>(target);
        if (adaptiveElement == nullptr)
        {
            return E_INVALIDARG;
        }

        std::shared_ptr<AdaptiveCards::ToggleVisibilityTarget> sharedTarget;
        RETURN_IF_FAILED(adaptiveElement->GetSharedModel(sharedTarget));
        containedElements.push_back(std::AdaptivePointerCast<AdaptiveCards::ToggleVisibilityTarget>(sharedTarget));

        hr = vectorIterator->MoveNext(&hasCurrent);
    }

    return S_OK;
}

HRESULT GenerateElementProjection(_In_ const std::shared_ptr<AdaptiveCards::BaseCardElement>& baseElement,
                                  _COM_Outptr_ ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveCardElement** projectedElement) noexcept
try
{
    *projectedElement = nullptr;
    switch (baseElement->GetElementType())
    {
    case CardElementType::TextBlock:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::Uwp::AdaptiveTextBlock>(
            projectedElement, std::AdaptivePointerCast<AdaptiveCards::TextBlock>(baseElement)));
        break;
    case CardElementType::Image:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::Uwp::AdaptiveImage>(
            projectedElement, std::AdaptivePointerCast<AdaptiveCards::Image>(baseElement)));
        break;
    case CardElementType::Container:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::Uwp::AdaptiveContainer>(
            projectedElement, std::AdaptivePointerCast<AdaptiveCards::Container>(baseElement)));
        break;
    case CardElementType::ColumnSet:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::Uwp::AdaptiveColumnSet>(
            projectedElement, std::AdaptivePointerCast<AdaptiveCards::ColumnSet>(baseElement)));
        break;
    case CardElementType::FactSet:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::Uwp::AdaptiveFactSet>(
            projectedElement, std::AdaptivePointerCast<AdaptiveCards::FactSet>(baseElement)));
        break;
    case CardElementType::ImageSet:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::Uwp::AdaptiveImageSet>(
            projectedElement, std::AdaptivePointerCast<AdaptiveCards::ImageSet>(baseElement)));
        break;
    case CardElementType::ChoiceSetInput:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::Uwp::AdaptiveChoiceSetInput>(
            projectedElement, std::AdaptivePointerCast<AdaptiveCards::ChoiceSetInput>(baseElement)));
        break;
    case CardElementType::DateInput:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::Uwp::AdaptiveDateInput>(
            projectedElement, std::AdaptivePointerCast<AdaptiveCards::DateInput>(baseElement)));
        break;
    case CardElementType::Media:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::Uwp::AdaptiveMedia>(
            projectedElement, std::AdaptivePointerCast<AdaptiveCards::Media>(baseElement)));
        break;
    case CardElementType::NumberInput:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::Uwp::AdaptiveNumberInput>(
            projectedElement, std::AdaptivePointerCast<AdaptiveCards::NumberInput>(baseElement)));
        break;
    case CardElementType::TextInput:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::Uwp::AdaptiveTextInput>(
            projectedElement, std::AdaptivePointerCast<AdaptiveCards::TextInput>(baseElement)));
        break;
    case CardElementType::TimeInput:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::Uwp::AdaptiveTimeInput>(
            projectedElement, std::AdaptivePointerCast<AdaptiveCards::TimeInput>(baseElement)));
        break;
    case CardElementType::ToggleInput:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::Uwp::AdaptiveToggleInput>(
            projectedElement, std::AdaptivePointerCast<AdaptiveCards::ToggleInput>(baseElement)));
        break;
    case CardElementType::ActionSet:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::Uwp::AdaptiveActionSet>(
            projectedElement, std::AdaptivePointerCast<AdaptiveCards::ActionSet>(baseElement)));
        break;
    case CardElementType::RichTextBlock:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::Uwp::AdaptiveRichTextBlock>(
            projectedElement, std::AdaptivePointerCast<AdaptiveCards::RichTextBlock>(baseElement)));
        break;
    case CardElementType::Column:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::Uwp::AdaptiveColumn>(
            projectedElement, std::AdaptivePointerCast<AdaptiveCards::Column>(baseElement)));
        break;
    case CardElementType::Custom:
        RETURN_IF_FAILED(
            std::AdaptivePointerCast<::AdaptiveCards::ObjectModel::Uwp::CustomElementWrapper>(baseElement)->GetWrappedElement(projectedElement));
        break;
    case CardElementType::Unknown:
    default:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::Uwp::AdaptiveUnsupportedElement>(
            projectedElement, std::AdaptivePointerCast<AdaptiveCards::UnknownElement>(baseElement)));
        break;
    }

    return S_OK;
}
CATCH_RETURN;

HRESULT GenerateContainedElementsProjection(
    const std::vector<std::shared_ptr<AdaptiveCards::BaseCardElement>>& containedElements,
    _In_ ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveCardElement*>* projectedParentContainer) noexcept
try
{
    for (auto& containedElement : containedElements)
    {
        ComPtr<ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveCardElement> projectedContainedElement;
        GenerateElementProjection(containedElement, &projectedContainedElement);
        if (projectedContainedElement != nullptr)
        {
            RETURN_IF_FAILED(projectedParentContainer->Append(projectedContainedElement.Detach()));
        }
    }
    return S_OK;
}
CATCH_RETURN;

HRESULT GenerateActionsProjection(
    const std::vector<std::shared_ptr<AdaptiveCards::BaseActionElement>>& containedActions,
    _In_ ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveActionElement*>* projectedParentContainer) noexcept
try
{
    for (auto& containedAction : containedActions)
    {
        ComPtr<ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveActionElement> projectedContainedAction;
        RETURN_IF_FAILED(GenerateActionProjection(containedAction, &projectedContainedAction));

        RETURN_IF_FAILED(projectedParentContainer->Append(projectedContainedAction.Detach()));
    }
    return S_OK;
}
CATCH_RETURN;

HRESULT GenerateActionProjection(const std::shared_ptr<AdaptiveCards::BaseActionElement>& action,
                                 _COM_Outptr_ ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveActionElement** projectedAction) noexcept
try
{
    if (action == nullptr)
    {
        *projectedAction = nullptr;
        return S_OK;
    }

    switch (action->GetElementType())
    {
    case ActionType::OpenUrl:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::Uwp::AdaptiveOpenUrlAction>(
            projectedAction, std::AdaptivePointerCast<AdaptiveCards::OpenUrlAction>(action)));
        break;
    case ActionType::ShowCard:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::Uwp::AdaptiveShowCardAction>(
            projectedAction, std::AdaptivePointerCast<AdaptiveCards::ShowCardAction>(action)));
        break;
    case ActionType::Submit:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::Uwp::AdaptiveSubmitAction>(
            projectedAction, std::AdaptivePointerCast<AdaptiveCards::SubmitAction>(action)));
        break;
    case ActionType::ToggleVisibility:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::Uwp::AdaptiveToggleVisibilityAction>(
            projectedAction, std::AdaptivePointerCast<AdaptiveCards::ToggleVisibilityAction>(action)));
        break;
    case ActionType::Execute:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::Uwp::AdaptiveExecuteAction>(
            projectedAction, std::AdaptivePointerCast<AdaptiveCards::ExecuteAction>(action)));
        break;
    case ActionType::Custom:
        RETURN_IF_FAILED(std::AdaptivePointerCast<CustomActionWrapper>(action)->GetWrappedElement(projectedAction));
        break;
    case ActionType::UnknownAction:
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::Uwp::AdaptiveUnsupportedAction>(
            projectedAction, std::AdaptivePointerCast<AdaptiveCards::UnknownAction>(action)));
        break;
    default:
        return E_UNEXPECTED;
        break;
    }

    return S_OK;
}
CATCH_RETURN;

HRESULT GenerateColumnsProjection(
    const std::vector<std::shared_ptr<AdaptiveCards::Column>>& containedElements,
    _In_ ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::ObjectModel::Uwp::AdaptiveColumn*>* projectedParentContainer) noexcept
try
{
    for (auto& containedElement : containedElements)
    {
        ComPtr<ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveColumn> projectedContainedElement;
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::Uwp::AdaptiveColumn>(
            &projectedContainedElement, std::static_pointer_cast<AdaptiveCards::Column>(containedElement)));

        RETURN_IF_FAILED(projectedParentContainer->Append(projectedContainedElement.Detach()));
    }
    return S_OK;
}
CATCH_RETURN;

HRESULT GenerateFactsProjection(const std::vector<std::shared_ptr<AdaptiveCards::Fact>>& containedElements,
                                _In_ ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::ObjectModel::Uwp::AdaptiveFact*>* projectedParentContainer) noexcept
try
{
    for (auto& containedElement : containedElements)
    {
        ComPtr<ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveFact> projectedContainedElement;
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::Uwp::AdaptiveFact>(
            &projectedContainedElement, std::static_pointer_cast<AdaptiveCards::Fact>(containedElement)));

        RETURN_IF_FAILED(projectedParentContainer->Append(projectedContainedElement.Detach()));
    }
    return S_OK;
}
CATCH_RETURN;

HRESULT GenerateInlinesProjection(const std::vector<std::shared_ptr<AdaptiveCards::Inline>>& containedElements,
                                  ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveInline*>* projectedParentContainer) noexcept
try
{
    for (auto& containedElement : containedElements)
    {
        // Only support text runs for now
        if (containedElement->GetInlineType() != InlineElementType::TextRun)
        {
            return E_NOTIMPL;
        }

        ComPtr<ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveInline> projectedContainedElement;
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::Uwp::AdaptiveTextRun>(
            &projectedContainedElement, std::static_pointer_cast<AdaptiveCards::TextRun>(containedElement)));

        RETURN_IF_FAILED(projectedParentContainer->Append(projectedContainedElement.Detach()));
    }
    return S_OK;
}
CATCH_RETURN;

HRESULT GenerateRequirementsProjection(
    const std::unordered_map<std::string, SemanticVersion>& sharedRequirements,
    _In_ ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::ObjectModel::Uwp::AdaptiveRequirement*>* projectedRequirementVector) noexcept
try
{
    for (const auto& sharedRequirement : sharedRequirements)
    {
        ComPtr<ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveRequirement> projectedRequirement;
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::Uwp::AdaptiveRequirement>(&projectedRequirement,
                                                                                                   sharedRequirement));
        RETURN_IF_FAILED(projectedRequirementVector->Append(projectedRequirement.Detach()));
    }
    return S_OK;
}
CATCH_RETURN;

HRESULT GenerateImagesProjection(
    const std::vector<std::shared_ptr<AdaptiveCards::Image>>& containedElements,
    _In_ ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::ObjectModel::Uwp::AdaptiveImage*>* projectedParentContainer) noexcept
try
{
    for (auto& containedElement : containedElements)
    {
        ComPtr<ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveImage> projectedContainedElement;
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::Uwp::AdaptiveImage>(
            &projectedContainedElement, std::static_pointer_cast<AdaptiveCards::Image>(containedElement)));

        RETURN_IF_FAILED(projectedParentContainer->Append(projectedContainedElement.Detach()));
    }
    return S_OK;
}
CATCH_RETURN;

HRESULT GenerateInputChoicesProjection(
    const std::vector<std::shared_ptr<AdaptiveCards::ChoiceInput>>& containedElements,
    _In_ ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::ObjectModel::Uwp::AdaptiveChoiceInput*>* projectedParentContainer) noexcept
try
{
    for (auto& containedElement : containedElements)
    {
        ComPtr<ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveChoiceInput> projectedContainedElement;
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::Uwp::AdaptiveChoiceInput>(
            &projectedContainedElement, std::static_pointer_cast<AdaptiveCards::ChoiceInput>(containedElement)));

        RETURN_IF_FAILED(projectedParentContainer->Append(projectedContainedElement.Detach()));
    }
    return S_OK;
}
CATCH_RETURN;

HRESULT GenerateMediaSourcesProjection(
    const std::vector<std::shared_ptr<AdaptiveCards::MediaSource>>& containedElements,
    _In_ ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::ObjectModel::Uwp::AdaptiveMediaSource*>* projectedParentContainer) noexcept
try
{
    for (auto& containedElement : containedElements)
    {
        ComPtr<ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveMediaSource> projectedContainedElement;
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::Uwp::AdaptiveMediaSource>(
            &projectedContainedElement, std::static_pointer_cast<AdaptiveCards::MediaSource>(containedElement)));

        RETURN_IF_FAILED(projectedParentContainer->Append(projectedContainedElement.Detach()));
    }
    return S_OK;
}
CATCH_RETURN;

HRESULT GenerateToggleTargetProjection(
    const std::vector<std::shared_ptr<AdaptiveCards::ToggleVisibilityTarget>>& containedElements,
    _In_ ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::ObjectModel::Uwp::AdaptiveToggleVisibilityTarget*>* projectedParentContainer) noexcept
try
{
    for (auto& containedElement : containedElements)
    {
        ComPtr<ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveToggleVisibilityTarget> projectedContainedElement;
        RETURN_IF_FAILED(MakeAndInitialize<::AdaptiveCards::ObjectModel::Uwp::AdaptiveToggleVisibilityTarget>(
            &projectedContainedElement, std::static_pointer_cast<AdaptiveCards::ToggleVisibilityTarget>(containedElement)));

        RETURN_IF_FAILED(projectedParentContainer->Append(projectedContainedElement.Detach()));
    }
    return S_OK;
}
CATCH_RETURN;

// Get a Color object from color string
// Expected formats are "#AARRGGBB" (with alpha channel) and "#RRGGBB" (without alpha channel)
HRESULT GetColorFromString(const std::string& colorString, _Out_ ABI::Windows::UI::Color* color) noexcept
try
{
    if (colorString.length() > 0 && colorString.front() == '#')
    {
        // Get the pure hex value (without #)
        std::string hexColorString = colorString.substr(1, std::string::npos);

        std::regex colorWithAlphaRegex("[0-9a-f]{8}", std::regex_constants::icase);
        if (regex_match(hexColorString, colorWithAlphaRegex))
        {
            // If color string has alpha channel, extract and set to color
            std::string alphaString = hexColorString.substr(0, 2);
            INT32 alpha = strtol(alphaString.c_str(), nullptr, 16);

            color->A = static_cast<BYTE>(alpha);

            hexColorString = hexColorString.substr(2, std::string::npos);
        }
        else
        {
            // Otherwise, set full opacity
            std::string alphaString = "FF";
            INT32 alpha = strtol(alphaString.c_str(), nullptr, 16);
            color->A = static_cast<BYTE>(alpha);
        }

        // A valid string at this point should have 6 hex characters (RRGGBB)
        std::regex colorWithoutAlphaRegex("[0-9a-f]{6}", std::regex_constants::icase);
        if (regex_match(hexColorString, colorWithoutAlphaRegex))
        {
            // Then set all other Red, Green, and Blue channels
            std::string redString = hexColorString.substr(0, 2);
            INT32 red = strtol(redString.c_str(), nullptr, 16);

            std::string greenString = hexColorString.substr(2, 2);
            INT32 green = strtol(greenString.c_str(), nullptr, 16);

            std::string blueString = hexColorString.substr(4, 2);
            INT32 blue = strtol(blueString.c_str(), nullptr, 16);

            color->R = static_cast<BYTE>(red);
            color->G = static_cast<BYTE>(green);
            color->B = static_cast<BYTE>(blue);

            return S_OK;
        }
    }

    // All other formats are ignored (set alpha to 0)
    color->A = static_cast<BYTE>(0);

    return S_OK;
}
CATCH_RETURN;

HRESULT StringToJsonObject(const std::string& inputString, _COM_Outptr_ IJsonObject** result)
{
    HString asHstring;
    RETURN_IF_FAILED(UTF8ToHString(inputString, asHstring.GetAddressOf()));
    return HStringToJsonObject(asHstring.Get(), result);
}

HRESULT HStringToJsonObject(const HSTRING& inputHString, _COM_Outptr_ IJsonObject** result)
{
    ComPtr<IJsonObjectStatics> jObjectStatics;
    RETURN_IF_FAILED(GetActivationFactory(HStringReference(RuntimeClass_Windows_Data_Json_JsonObject).Get(), &jObjectStatics));
    ComPtr<IJsonObject> jObject;
    HRESULT hr = jObjectStatics->Parse(inputHString, &jObject);
    if (FAILED(hr))
    {
        RETURN_IF_FAILED(ActivateInstance(HStringReference(RuntimeClass_Windows_Data_Json_JsonObject).Get(), &jObject));
    }
    *result = jObject.Detach();
    return S_OK;
}

HRESULT JsonObjectToString(_In_ IJsonObject* inputJson, std::string& result)
{
    HString asHstring;
    RETURN_IF_FAILED(JsonObjectToHString(inputJson, asHstring.GetAddressOf()));
    return HStringToUTF8(asHstring.Get(), result);
}

HRESULT JsonObjectToHString(_In_ IJsonObject* inputJson, _Outptr_ HSTRING* result)
{
    if (!inputJson)
    {
        return E_INVALIDARG;
    }
    ComPtr<IJsonObject> localInputJson(inputJson);
    ComPtr<IJsonValue> asJsonValue;
    RETURN_IF_FAILED(localInputJson.As(&asJsonValue));
    return (asJsonValue->Stringify(result));
}

HRESULT StringToJsonValue(const std::string inputString, _COM_Outptr_ IJsonValue** result)
{
    HString asHstring;
    RETURN_IF_FAILED(UTF8ToHString(inputString, asHstring.GetAddressOf()));
    return HStringToJsonValue(asHstring.Get(), result);
}

HRESULT HStringToJsonValue(const HSTRING& inputHString, _COM_Outptr_ IJsonValue** result)
{
    ComPtr<IJsonValueStatics> jValueStatics;
    RETURN_IF_FAILED(GetActivationFactory(HStringReference(RuntimeClass_Windows_Data_Json_JsonValue).Get(), &jValueStatics));
    ComPtr<IJsonValue> jValue;
    HRESULT hr = jValueStatics->Parse(inputHString, &jValue);
    if (FAILED(hr))
    {
        RETURN_IF_FAILED(ActivateInstance(HStringReference(RuntimeClass_Windows_Data_Json_JsonValue).Get(), &jValue));
    }
    *result = jValue.Detach();
    return S_OK;
}

HRESULT JsonValueToString(_In_ IJsonValue* inputValue, std::string& result)
{
    HString asHstring;
    RETURN_IF_FAILED(JsonValueToHString(inputValue, asHstring.GetAddressOf()));
    return HStringToUTF8(asHstring.Get(), result);
}

HRESULT JsonValueToHString(_In_ IJsonValue* inputJsonValue, _Outptr_ HSTRING* result)
{
    if (!inputJsonValue)
    {
        return E_INVALIDARG;
    }
    ComPtr<IJsonValue> localInputJsonValue(inputJsonValue);
    return (localInputJsonValue->Stringify(result));
}

HRESULT JsonCppToJsonObject(const Json::Value& jsonCppValue, _COM_Outptr_ IJsonObject** result)
{
    std::string jsonString = ParseUtil::JsonToString(jsonCppValue);
    return StringToJsonObject(jsonString, result);
}

HRESULT JsonObjectToJsonCpp(_In_ ABI::Windows::Data::Json::IJsonObject* jsonObject, _Out_ Json::Value* jsonCppValue)
{
    std::string jsonString;
    RETURN_IF_FAILED(JsonObjectToString(jsonObject, jsonString));

    *jsonCppValue = ParseUtil::GetJsonValueFromString(jsonString);

    return S_OK;
}

HRESULT ProjectedActionTypeToHString(ABI::AdaptiveCards::ObjectModel::Uwp::ActionType projectedActionType, _Outptr_ HSTRING* result)
{
    ActionType sharedActionType = static_cast<ActionType>(projectedActionType);
    return UTF8ToHString(ActionTypeToString(sharedActionType), result);
}

HRESULT ProjectedElementTypeToHString(ABI::AdaptiveCards::ObjectModel::Uwp::ElementType projectedElementType, _Outptr_ HSTRING* result)
{
    CardElementType sharedElementType = static_cast<CardElementType>(projectedElementType);
    return UTF8ToHString(CardElementTypeToString(sharedElementType), result);
}

HRESULT MeetsRequirements(_In_ ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveCardElement* cardElement,
                          _In_ ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveFeatureRegistration* featureRegistration,
                          _Out_ bool* meetsRequirements)
{
    std::shared_ptr<AdaptiveCards::BaseCardElement> sharedElement;
    RETURN_IF_FAILED(GenerateSharedElement(cardElement, sharedElement));

    ComPtr<AdaptiveFeatureRegistration> featureRegistrationImpl = PeekInnards<AdaptiveFeatureRegistration>(featureRegistration);
    const std::shared_ptr<AdaptiveCards::FeatureRegistration>& sharedFeatureRegistration =
        featureRegistrationImpl->GetSharedFeatureRegistration();

    *meetsRequirements = sharedElement->MeetsRequirements(*sharedFeatureRegistration);
    return S_OK;
}

HRESULT IsBackgroundImageValid(_In_ ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveBackgroundImage* backgroundImageElement,
                               _Out_ BOOL* isValid)
{
    *isValid = FALSE;
    ComPtr<ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveBackgroundImage> backgroundImage(backgroundImageElement);
    if (backgroundImage != NULL)
    {
        HString url;
        RETURN_IF_FAILED(backgroundImage->get_Url(url.GetAddressOf()));
        *isValid = url.IsValid();
    }
    return S_OK;
}

void RemoteResourceElementToRemoteResourceInformationVector(_In_ ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveElementWithRemoteResources* remoteResourceElement,
                                                            std::vector<RemoteResourceInformation>& resourceUris)
{
    ComPtr<ABI::Windows::Foundation::Collections::IVectorView<ABI::AdaptiveCards::ObjectModel::Uwp::AdaptiveRemoteResourceInformation*>> remoteResources;
    THROW_IF_FAILED(remoteResourceElement->GetResourceInformation(remoteResources.GetAddressOf()));

    ComPtr<IIterable<ABI::AdaptiveCards::ObjectModel::Uwp::AdaptiveRemoteResourceInformation*>> vectorIterable;
    THROW_IF_FAILED(remoteResources.As<IIterable<ABI::AdaptiveCards::ObjectModel::Uwp::AdaptiveRemoteResourceInformation*>>(&vectorIterable));

    Microsoft::WRL::ComPtr<IIterator<ABI::AdaptiveCards::ObjectModel::Uwp::AdaptiveRemoteResourceInformation*>> vectorIterator;
    HRESULT hr = vectorIterable->First(&vectorIterator);

    boolean hasCurrent;
    THROW_IF_FAILED(vectorIterator->get_HasCurrent(&hasCurrent));

    while (SUCCEEDED(hr) && hasCurrent)
    {
        ComPtr<ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveRemoteResourceInformation> resourceInformation;
        THROW_IF_FAILED(vectorIterator->get_Current(&resourceInformation));

        HString url;
        THROW_IF_FAILED(resourceInformation->get_Url(url.GetAddressOf()));

        RemoteResourceInformation uriInfo;
        THROW_IF_FAILED(HStringToUTF8(url.Get(), uriInfo.url));

        HString mimeType;
        THROW_IF_FAILED(resourceInformation->get_MimeType(mimeType.GetAddressOf()));

        uriInfo.mimeType = HStringToUTF8(mimeType.Get());

        resourceUris.push_back(uriInfo);

        hr = vectorIterator->MoveNext(&hasCurrent);
    }
}

HRESULT SharedWarningsToAdaptiveWarnings(
    const std::vector<std::shared_ptr<AdaptiveCardParseWarning>>& sharedWarnings,
    _In_ ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::ObjectModel::Uwp::AdaptiveWarning*>* adaptiveWarnings)
{
    for (const auto& sharedWarning : sharedWarnings)
    {
        HString warningMessage;
        RETURN_IF_FAILED(UTF8ToHString(sharedWarning->GetReason(), warningMessage.GetAddressOf()));

        ABI::AdaptiveCards::ObjectModel::Uwp::WarningStatusCode statusCode =
            static_cast<ABI::AdaptiveCards::ObjectModel::Uwp::WarningStatusCode>(sharedWarning->GetStatusCode());

        ComPtr<ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveWarning> adaptiveWarning;
        RETURN_IF_FAILED(MakeAndInitialize<AdaptiveWarning>(&adaptiveWarning, statusCode, warningMessage.Get()));

        RETURN_IF_FAILED(adaptiveWarnings->Append(adaptiveWarning.Get()));
    }

    return S_OK;
}

HRESULT AdaptiveWarningsToSharedWarnings(
    _In_ ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::ObjectModel::Uwp::AdaptiveWarning*>* adaptiveWarnings,
    std::vector<std::shared_ptr<AdaptiveCardParseWarning>>& sharedWarnings)
{
    ComPtr<ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveCards::ObjectModel::Uwp::AdaptiveWarning*>> localAdaptiveWarnings{
        adaptiveWarnings};
    ComPtr<IIterable<ABI::AdaptiveCards::ObjectModel::Uwp::AdaptiveWarning*>> vectorIterable;
    RETURN_IF_FAILED(localAdaptiveWarnings.As<IIterable<ABI::AdaptiveCards::ObjectModel::Uwp::AdaptiveWarning*>>(&vectorIterable));

    Microsoft::WRL::ComPtr<IIterator<ABI::AdaptiveCards::ObjectModel::Uwp::AdaptiveWarning*>> vectorIterator;
    HRESULT hr = vectorIterable->First(&vectorIterator);

    boolean hasCurrent;
    THROW_IF_FAILED(vectorIterator->get_HasCurrent(&hasCurrent));

    while (SUCCEEDED(hr) && hasCurrent)
    {
        ComPtr<ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveWarning> adaptiveWarning;
        RETURN_IF_FAILED(vectorIterator->get_Current(&adaptiveWarning));

        HString message;
        RETURN_IF_FAILED(adaptiveWarning->get_Message(message.GetAddressOf()));

        ABI::AdaptiveCards::ObjectModel::Uwp::WarningStatusCode statusCode;
        RETURN_IF_FAILED(adaptiveWarning->get_StatusCode(&statusCode));

        sharedWarnings.push_back(std::make_shared<AdaptiveCardParseWarning>(static_cast<AdaptiveCards::WarningStatusCode>(statusCode),
                                                                            HStringToUTF8(message.Get())));

        hr = vectorIterator->MoveNext(&hasCurrent);
    }

    return S_OK;
}

Color GenerateLHoverColor(const Color& originalColor)
{
    const double hoverIncrement = 0.25;

    Color hoverColor;
    hoverColor.A = originalColor.A;
    hoverColor.R = originalColor.R - static_cast<BYTE>(originalColor.R * hoverIncrement);
    hoverColor.G = originalColor.G - static_cast<BYTE>(originalColor.G * hoverIncrement);
    hoverColor.B = originalColor.B - static_cast<BYTE>(originalColor.B * hoverIncrement);
    return hoverColor;
}

DateTime GetDateTime(unsigned int year, unsigned int month, unsigned int day)
{
    SYSTEMTIME systemTime = {(WORD)year, (WORD)month, 0, (WORD)day};

    // Convert to UTC
    TIME_ZONE_INFORMATION timeZone;
    GetTimeZoneInformation(&timeZone);
    TzSpecificLocalTimeToSystemTime(&timeZone, &systemTime, &systemTime);

    // Convert to ticks
    FILETIME fileTime;
    SystemTimeToFileTime(&systemTime, &fileTime);
    DateTime dateTime{(INT64)fileTime.dwLowDateTime + (((INT64)fileTime.dwHighDateTime) << 32)};

    return dateTime;
}

HRESULT GetDateTimeReference(unsigned int year, unsigned int month, unsigned int day, _COM_Outptr_ IReference<DateTime>** dateTimeReference)
{
    DateTime dateTime = GetDateTime(year, month, day);

    ComPtr<IPropertyValueStatics> factory;
    RETURN_IF_FAILED(GetActivationFactory(HStringReference(RuntimeClass_Windows_Foundation_PropertyValue).Get(), &factory));

    ComPtr<IInspectable> inspectable;
    RETURN_IF_FAILED(factory->CreateDateTime(dateTime, &inspectable));

    ComPtr<IReference<DateTime>> localDateTimeReference;
    RETURN_IF_FAILED(inspectable.As(&localDateTimeReference));

    *dateTimeReference = localDateTimeReference.Detach();

    return S_OK;
}

ABI::AdaptiveCards::ObjectModel::Uwp::FallbackType MapSharedFallbackTypeToUwp(const AdaptiveCards::FallbackType type)
{
    switch (type)
    {
    case FallbackType::Drop:
    {
        return ABI::AdaptiveCards::ObjectModel::Uwp::FallbackType::Drop;
    }

    case FallbackType::Content:
    {
        return ABI::AdaptiveCards::ObjectModel::Uwp::FallbackType::Content;
    }

    case FallbackType::None:
    default:
    {
        return ABI::AdaptiveCards::ObjectModel::Uwp::FallbackType::None;
    }
    }
}

AdaptiveCards::FallbackType MapUwpFallbackTypeToShared(const ABI::AdaptiveCards::ObjectModel::Uwp::FallbackType type)
{
    switch (type)
    {
    case ABI::AdaptiveCards::ObjectModel::Uwp::FallbackType::Drop:
    {
        return FallbackType::Drop;
    }

    case ABI::AdaptiveCards::ObjectModel::Uwp::FallbackType::Content:
    {
        return FallbackType::Content;
    }

    case ABI::AdaptiveCards::ObjectModel::Uwp::FallbackType::None:
    default:
    {
        return FallbackType::None;
    }
    }
}

HRESULT CopyTextElement(_In_ ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveTextElement* textElement,
                        _COM_Outptr_ ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveTextElement** copiedTextElement)
{
    ComPtr<AdaptiveCards::ObjectModel::Uwp::AdaptiveTextElement> localCopiedTextElement;
    RETURN_IF_FAILED(MakeAndInitialize<AdaptiveCards::ObjectModel::Uwp::AdaptiveTextRun>(&localCopiedTextElement));

    ABI::AdaptiveCards::ObjectModel::Uwp::ForegroundColor color;
    RETURN_IF_FAILED(textElement->get_Color(&color));
    RETURN_IF_FAILED(localCopiedTextElement->put_Color(color));

    ABI::AdaptiveCards::ObjectModel::Uwp::FontType fontType;
    RETURN_IF_FAILED(textElement->get_FontType(&fontType));
    RETURN_IF_FAILED(localCopiedTextElement->put_FontType(fontType));

    boolean isSubtle;
    RETURN_IF_FAILED(textElement->get_IsSubtle(&isSubtle));
    RETURN_IF_FAILED(localCopiedTextElement->put_IsSubtle(isSubtle));

    HString language;
    RETURN_IF_FAILED(textElement->get_Language(language.GetAddressOf()));
    RETURN_IF_FAILED(localCopiedTextElement->put_Language(language.Get()));

    ABI::AdaptiveCards::ObjectModel::Uwp::TextSize size;
    RETURN_IF_FAILED(textElement->get_Size(&size));
    RETURN_IF_FAILED(localCopiedTextElement->put_Size(size));

    ABI::AdaptiveCards::ObjectModel::Uwp::TextWeight weight;
    RETURN_IF_FAILED(textElement->get_Weight(&weight));
    RETURN_IF_FAILED(localCopiedTextElement->put_Weight(weight));

    HString text;
    RETURN_IF_FAILED(textElement->get_Text(text.GetAddressOf()));
    RETURN_IF_FAILED(localCopiedTextElement->put_Text(text.Get()));

    RETURN_IF_FAILED(localCopiedTextElement.CopyTo(copiedTextElement));
    return S_OK;
}

HRESULT GetAdaptiveActionParserRegistrationFromSharedModel(
    const std::shared_ptr<ActionParserRegistration>& sharedActionParserRegistration,
    _COM_Outptr_ ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveActionParserRegistration** adaptiveActionParserRegistration)
{
    // Look up the well known action parser registration to see if we've got a custom action registration to pass
    std::shared_ptr<ActionElementParser> sharedActionParser =
        sharedActionParserRegistration->GetParser(c_upwActionParserRegistration);

    if (sharedActionParser != nullptr)
    {
        // The shared model wraps the passed in parsers. Get our SharedModelActionParser from it so we can retrieve the
        // IAdaptiveActionParserRegistration
        std::shared_ptr<ActionElementParserWrapper> parserWrapper =
            std::static_pointer_cast<ActionElementParserWrapper>(sharedActionParser);

        std::shared_ptr<SharedModelActionParser> sharedModelParser =
            std::static_pointer_cast<SharedModelActionParser>(parserWrapper->GetActualParser());

        RETURN_IF_FAILED(sharedModelParser->GetAdaptiveParserRegistration(adaptiveActionParserRegistration));
    }
    else
    {
        RETURN_IF_FAILED(MakeAndInitialize<AdaptiveCards::ObjectModel::Uwp::AdaptiveActionParserRegistration>(adaptiveActionParserRegistration));
    }

    return S_OK;
}

HRESULT GetAdaptiveElementParserRegistrationFromSharedModel(
    const std::shared_ptr<ElementParserRegistration>& sharedElementParserRegistration,
    _COM_Outptr_ ABI::AdaptiveCards::ObjectModel::Uwp::IAdaptiveElementParserRegistration** adaptiveElementParserRegistration)
{
    // Look up the well known Element parser registration to see if we've got a custom Element registration to pass
    std::shared_ptr<BaseCardElementParser> sharedElementParser =
        sharedElementParserRegistration->GetParser(c_uwpElementParserRegistration);

    if (sharedElementParser != nullptr)
    {
        // The shared model wraps the passed in parsers. Get our SharedModelElementParser from it so we can retrieve the
        // IAdaptiveElementParserRegistration
        std::shared_ptr<BaseCardElementParserWrapper> parserWrapper =
            std::static_pointer_cast<BaseCardElementParserWrapper>(sharedElementParser);

        std::shared_ptr<SharedModelElementParser> sharedModelParser =
            std::static_pointer_cast<SharedModelElementParser>(parserWrapper->GetActualParser());

        RETURN_IF_FAILED(sharedModelParser->GetAdaptiveParserRegistration(adaptiveElementParserRegistration));
    }
    else
    {
        RETURN_IF_FAILED(MakeAndInitialize<AdaptiveCards::ObjectModel::Uwp::AdaptiveElementParserRegistration>(
            adaptiveElementParserRegistration));
    }

    return S_OK;
}
