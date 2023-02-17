#pragma once

namespace ptg {

class kernel;

/// @brief Used for mapping kernels to identifiers.
///        In this case, instead of a dictionary, direct member access is used instead.
struct kernel_registry final
{
  /// @brief Used for raising the terrain at a certain location.
  kernel* raise_kernel{ nullptr };
};

} // namespace ptg
