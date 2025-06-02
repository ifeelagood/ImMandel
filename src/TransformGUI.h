#pragma once

#include <type_traits>
#include <limits>

#include "Transform.h"
#include "ImGuiDrawable.h"
#include "ImGui_mpfr.h"


template <Real T>
class TransformGUI : public ImGuiDrawable {
private:
	Transform<T>& _transform;
public:
	explicit TransformGUI(Transform<T>& t) : _transform(t) {};
	void draw() override;
	
};

template<Real T>
inline void TransformGUI<T>::draw()
{
	if (ImGui::CollapsingHeader("Transform")) {
		unsigned long long zoom_curr = _transform.get_zoom();
		unsigned long long zoom_new = zoom_curr;

		ImGui::InputScalar("Zoom", ImGuiDataType_U64, &zoom_new);

		if (zoom_new != zoom_curr) {
			_transform.set_zoom(zoom_new);
		}


		T x_curr = _transform.get_position().x();
		T y_curr = _transform.get_position().y();

		T x_new = x_curr;
		T y_new = y_curr;

		if constexpr (std::is_same_v<T, float>) {

			ImGui::InputFloat("cx", &x_new, 0.0001f, 0.1f, "%.10f");
			ImGui::InputFloat("cy", &y_new, 0.0001f, 0.1f, "%.10f");
		}
		else if constexpr (std::is_same_v<T, double>) {
			ImGui::InputDouble("cx", &x_new, 0.000001, 0.1, "%.10f");
			ImGui::InputDouble("cy", &y_new, 0.000001, 0.1, "%.10f");
		}
		else if constexpr (std::is_same_v<T, boost::multiprecision::mpfr_float>) {
			ImGui::InputMPFRFloat("cx", &x_new);
			ImGui::InputMPFRFloat("cy", &y_new);
		}

		if (x_new != x_curr || y_new != y_curr) {
			Eigen::Vector2<T> new_position{ x_new, y_new };
			_transform.set_position(new_position);
		}
	}
}
