#include "ImGui_mpfr.h"

#include <stdexcept>

namespace ImGui {

bool InputMPFRFloat(const char* label, boost::multiprecision::mpfr_float& value, ImGuiInputTextCallback callback, void* user_data)
{
	unsigned bits = value.precision();
	unsigned digits = std::clamp(static_cast<unsigned>(bits * 0.30103), 10u, 200u); // log10(2)

	constexpr const size_t BUFFER_SIZE = 512;
	static thread_local char buffer[BUFFER_SIZE];


	std::string str = value.str(digits, std::ios_base::fmtflags(0));

	// safe buffer copy
	std::memcpy(buffer, 0, BUFFER_SIZE);
	std::memcpy(buffer, str.c_str(), std::min(str.size(), BUFFER_SIZE - 1));

	ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;


	if (ImGui::InputText(label, buffer, BUFFER_SIZE, flags, callback, user_data)) {
		try {
			value = boost::multiprecision::mpfr_float(buffer);
			return true;
		}
		catch (std::exception& e) {

		}
	}
	return false;
}

}
