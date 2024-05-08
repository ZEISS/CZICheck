// SPDX-FileCopyrightText: 2024 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include "checkerbase.h"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <utility>

struct DimensionView {
    libCZI::DimensionIndex DimensionIndex{ libCZI::DimensionIndex::invalid };
    char DimensionName{'0'};
    int Start{ -1 };
    int Size{ -1 };
    bool IsValid() const
    {
        // this is an object used exclusively for this checker
        //  a Size (SizeC, SizeX, etc.) is not needed to yield a "valid" dimension for this
        if (this->Start >= 0 && this->DimensionIndex != libCZI::DimensionIndex::invalid)
        {
            return true;
        }

        return false;
    };
};

class CCheckTopgraphyApplianceMetadata : public IChecker, CCheckerBase
{
private:
    static constexpr const char* kTopographyItemId = "Topography:1";
    static constexpr const char* kImageAppliancePath = "ImageDocument/Metadata/Appliances";
    static constexpr const wchar_t* kTextureItemKey = L"Texture";
    static constexpr const wchar_t* kHeighMapItemKey = L"HeightMap";

    std::vector<std::unordered_map<char, DimensionView>> texture_views;
    std::vector<std::unordered_map<char, DimensionView>> heightmap_views;

public:
    static const CZIChecks kCheckType = CZIChecks::ApplianceMetadataTopographyItemValid;
    static const char* kDisplayName;
    static const char* kShortName;
    
    CCheckTopgraphyApplianceMetadata(
        const std::shared_ptr<libCZI::ICZIReader>& reader,
        CResultGatherer& result_gatherer,
        const CheckerCreateInfo& additional_info);
    void RunCheck() override;

private:
    void CheckValidDimensionInTopographyDataItems(const std::shared_ptr<libCZI::ICziMetadata>& czi_metadata);

private:
    bool SetBoundsFromVector(const std::vector<std::pair<std::wstring, std::wstring>>&, std::vector<std::unordered_map<char, DimensionView>>&);
    bool ExtractMetaDataDimensions(const std::shared_ptr<libCZI::ICziMetadata>& czi_metadata);
};
