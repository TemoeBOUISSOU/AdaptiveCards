// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#include "pch.h"

#include "AdaptiveImageSet.h"

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace ABI::AdaptiveCards::Rendering::Uwp;
using namespace ABI::Windows::Foundation::Collections;
using namespace ABI::Windows::UI::Xaml;
using namespace ABI::Windows::UI::Xaml::Controls;

namespace AdaptiveCards::Rendering::Uwp
{
    AdaptiveImageSet::AdaptiveImageSet() { m_images = Microsoft::WRL::Make<Vector<AdaptiveImage*>>(); }

    HRESULT AdaptiveImageSet::RuntimeClassInitialize() noexcept
    try
    {
        std::shared_ptr<AdaptiveCards::ImageSet> imageSet = std::make_shared<AdaptiveCards::ImageSet>();
        return RuntimeClassInitialize(imageSet);
    }
    CATCH_RETURN;

    HRESULT AdaptiveImageSet::RuntimeClassInitialize(const std::shared_ptr<AdaptiveCards::ImageSet>& sharedImageSet)
    try
    {
        if (sharedImageSet == nullptr)
        {
            return E_INVALIDARG;
        }

        GenerateImagesProjection(sharedImageSet->GetImages(), m_images.Get());

        m_imageSize = static_cast<ABI::AdaptiveCards::Rendering::Uwp::ImageSize>(sharedImageSet->GetImageSize());

        InitializeBaseElement(std::static_pointer_cast<BaseCardElement>(sharedImageSet));
        return S_OK;
    }
    CATCH_RETURN;

    IFACEMETHODIMP AdaptiveImageSet::get_Images(_COM_Outptr_ IVector<AdaptiveImage*>** images)
    {
        return m_images.CopyTo(images);
    }

    HRESULT AdaptiveImageSet::get_ImageSize(_Out_ ABI::AdaptiveCards::Rendering::Uwp::ImageSize* imageSize)
    {
        *imageSize = m_imageSize;
        return S_OK;
    }

    HRESULT AdaptiveImageSet::put_ImageSize(ABI::AdaptiveCards::Rendering::Uwp::ImageSize imageSize)
    {
        m_imageSize = imageSize;
        return S_OK;
    }

    IFACEMETHODIMP AdaptiveImageSet::get_ElementType(_Out_ ElementType* elementType)
    {
        *elementType = ElementType::ImageSet;
        return S_OK;
    }

    HRESULT AdaptiveImageSet::GetSharedModel(std::shared_ptr<AdaptiveCards::BaseCardElement>& sharedModel)
    try
    {
        std::shared_ptr<AdaptiveCards::ImageSet> imageSet = std::make_shared<AdaptiveCards::ImageSet>();

        RETURN_IF_FAILED(CopySharedElementProperties(*imageSet));

        imageSet->SetImageSize(static_cast<AdaptiveCards::ImageSize>(m_imageSize));

        RETURN_IF_FAILED(GenerateSharedImages(m_images.Get(), imageSet->GetImages()));

        sharedModel = std::move(imageSet);
        return S_OK;
    }
    CATCH_RETURN;
}
