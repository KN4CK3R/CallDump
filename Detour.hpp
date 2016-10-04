#pragma once

#include <cstdint>

/// <summary>
/// Hooks a function and redirects the control flow to <see cref="dst" />.
/// </summary>
/// <param name="src">Address of the function to hook.</param>
/// <param name="dst">Address of the function to redirect to.</param>
/// <returns>The address of the trampoline to call the original function.</returns>
uintptr_t HookFunction(uintptr_t src, uintptr_t dst);
