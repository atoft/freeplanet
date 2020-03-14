//
// Created by alastair on 14/03/2020.
//

#pragma once

#define TestResult(condition) ((condition) ? (LogMessage("PASSED (" #condition ")"), true) : (LogError("FAILED !(" #condition ")"), false))
