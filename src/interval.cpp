#include "interval.h"

namespace cray {

const Interval Interval::empty(+Infinity, -Infinity);
const Interval Interval::universe(-Infinity, +Infinity);

}  // namespace cray