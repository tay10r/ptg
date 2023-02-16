#pragma once

#include "device.hpp"

#include <memory>

namespace ptg {

std::shared_ptr<device>
create_cpu_device();

} // namespace ptg
