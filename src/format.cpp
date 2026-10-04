#include "calc/format.h"

#include <cmath>
#include <cstdio>

namespace calc {

std::string format_number(double value) {
  if (value == 0.0) {
    return "0";  // also folds -0 into 0
  }
  char buffer[64];
  if (std::fabs(value) < 1e15 && value == std::trunc(value)) {
    std::snprintf(buffer, sizeof(buffer), "%.0f", value);
  } else {
    std::snprintf(buffer, sizeof(buffer), "%.15g", value);
  }
  return buffer;
}

}  // namespace calc
