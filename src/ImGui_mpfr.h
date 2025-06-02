#pragma once

#include <string>
#include <boost/multiprecision/mpfr.hpp>
#include <imgui.h>

namespace ImGui {

bool InputMPFRFloat(const char* label, boost::multiprecision::mpfr_float& value, ImGuiInputTextCallback callback = (ImGuiInputTextCallback)0, void* user_data = (void*)0);

}