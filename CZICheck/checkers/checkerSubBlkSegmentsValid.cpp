// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include "checkerSubBlkSegmentsValid.h"
#include <exception>
#include <sstream>
#include <memory>

using namespace libCZI;
using namespace std;

/*static*/const char* CCheckSubBlkSegmentsValid::kDisplayName = "SubBlock-Segments in SubBlockDirectory are valid";
/*static*/const char* CCheckSubBlkSegmentsValid::kShortName = "subblksegmentsvalid";

CCheckSubBlkSegmentsValid::CCheckSubBlkSegmentsValid(
    const std::shared_ptr<libCZI::ICZIReader>& reader,
    CResultGatherer& result_gatherer,
    const CheckerCreateInfo& additional_info) :
    CCheckerBase(reader, result_gatherer, additional_info)
{
}

void CCheckSubBlkSegmentsValid::RunCheck()
{
    this->result_gatherer_.StartCheck(CCheckSubBlkSegmentsValid::kCheckType);

    this->reader_->EnumerateSubBlocks(
        [this](int index, const SubBlockInfo& info)->bool
        {
            try
            {
                auto sub_block = this->reader_->ReadSubBlock(index);
                const auto compression_mode = sub_block->GetSubBlockInfo().GetCompressionMode();
                if (compression_mode != CompressionMode::Invalid)
                {
                    // According to documentation, for a subblock with a compression mode which is *not* supported by
                    //  libCZI, we'd be getting CompressionMode::Invalid here. So, if we get a valid compression mode,
                    //  then we can rightfully expect that the subblock can be decoded, or that we can get a bitmap here
                    try
                    {
                        auto bitmap = sub_block->CreateBitmap();
                    }
                    catch (exception& exception)
                    {
                        CResultGatherer::Finding finding(CCheckSubBlkSegmentsValid::kCheckType);
                        finding.severity = CResultGatherer::Severity::Fatal;
                        stringstream ss;
                        ss << "Error decoding subblock #" << index;
                        finding.information = ss.str();
                        finding.details = exception.what();
                        this->result_gatherer_.ReportFinding(finding);
                    }
                }
                else
                {
                    CResultGatherer::Finding finding(CCheckSubBlkSegmentsValid::kCheckType);
                    finding.severity = CResultGatherer::Severity::Info;
                    stringstream ss;
                    ss << "Subblock #" << index << " has a non-standard compression mode (" << sub_block->GetSubBlockInfo().compressionModeRaw << ")";
                    finding.information = ss.str();
                    this->result_gatherer_.ReportFinding(finding);
                }
            }
            catch (exception& exception)
            {
                CResultGatherer::Finding finding(CCheckSubBlkSegmentsValid::kCheckType);
                finding.severity = CResultGatherer::Severity::Fatal;
                stringstream ss;
                ss << "Error reading subblock #" << index;
                finding.information = ss.str();
                finding.details = exception.what();
                this->result_gatherer_.ReportFinding(finding);
            }

            return true;
        });

    this->result_gatherer_.FinishCheck(CCheckSubBlkSegmentsValid::kCheckType);
}
