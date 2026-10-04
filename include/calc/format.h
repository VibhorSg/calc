#pragma once

#include <string>

namespace calc {

// Formats a result for display: whole numbers print without a decimal point
// ("14", "-3"), everything else with up to 15 significant digits ("0.1",
// "3.14159265358979", "1e-20"). Negative zero prints as "0".
std::string format_number(double value);

}  // namespace calc
