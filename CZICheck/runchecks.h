// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once
#include "cmdlineoptions.h"
#include "IResultGatherer.h"
#include <memory>

/// This class is responsible for running the checks.
class CRunChecks
{
private:
    const CCmdLineOptions& opts;
    std::shared_ptr<ILog> consoleIo;
public:
    CRunChecks(const CCmdLineOptions& opts, std::shared_ptr<ILog> consoleIo);

    bool Run(IResultGatherer::AggregatedResult& result);
};
