#pragma once

#include "state.h"
#include <imgui.h>
#include <windows.h>

void Patches_Frame(ImFont *nms_font_medium, ImFont *nms_font);

constexpr uintptr_t CalculateRawOffsetFromRVA(uintptr_t va, uintptr_t rva, uintptr_t raw)
{
    return va + (raw - rva);
}