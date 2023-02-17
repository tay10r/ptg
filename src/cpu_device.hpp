#pragma once

#include <ptg.h>

#include "device.hpp"

#include <memory>

namespace ptg {

std::shared_ptr<device>
create_cpu_device(void* logger_data, ptg_log_callback logger_func);

} // namespace ptg
