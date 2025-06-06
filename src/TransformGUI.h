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

	Eigen::Vector2<T> _cursor_pos{ 0.0,0.0 }; // cursor position in fractal space
	bool _zoom_on_centre = false;
public:
	explicit TransformGUI(Transform<T>& t) : _transform(t) {};
	void draw() override;
	
};

template<Real T>
inline void TransformGUI<T>::draw()
{

	if (ImGui::CollapsingHeader("Transform")) {
		ImGui::Checkbox("Zoom On Centre", &_zoom_on_centre);

		unsigned long long zoom_curr = _transform.get_zoom();
		unsigned long long zoom_new = zoom_curr;

		ImGui::InputScalar("Zoom", ImGuiDataType_U64, &zoom_new);

		// zoom is set after the mouse position

		ImGui::Text("Centre Coordinate");

		T x_curr = _transform.get_position().x();
		T y_curr = _transform.get_position().y();

		T x_new = x_curr;
		T y_new = y_curr;

		// compute step
		T step_scalar = 0.05;
		T step_fast_scalar = 0.1;

		Eigen::Vector2<T> topleft = _transform.transform_point({ 0.0,0.0 });
		Eigen::Vector2<T> botright = _transform.transform_point(_transform.get_size().cast<T>());
		
		T step_x = (botright.x() - topleft.x()) / step_scalar;
		T step_y = (botright.y() - topleft.y()) / step_scalar;
		T step_fast_x = (botright.x() - topleft.x()) / step_fast_scalar;
		T step_fast_y = (botright.y() - topleft.y()) / step_fast_scalar;

		if constexpr (std::is_same_v<T, float>) {

			ImGui::InputFloat("cx", &x_new, step_x, step_fast_x, "%.10f");
			ImGui::InputFloat("cy", &y_new, step_y, step_fast_y, "%.10f");
		}
		else if constexpr (std::is_same_v<T, double>) {
			ImGui::InputDouble("cx", &x_new, step_x, step_fast_x, "%.10f");
			ImGui::InputDouble("cy", &y_new, step_y, step_fast_y, "%.10f");
		}
		else if constexpr (std::is_same_v<T, boost::multiprecision::mpfr_float>) {
			ImGui::InputMPFRFloat("cx", &x_new);
			ImGui::InputMPFRFloat("cy", &y_new);
		}

		if (x_new != x_curr || y_new != y_curr) {
			Eigen::Vector2<T> new_position{ x_new, y_new };
			_transform.set_position(new_position);
		}

		ImGui::Text("Cursor Coordinate");

		ImGuiIO& io = ImGui::GetIO();
		ImVec2 mouse_pos = io.MousePos;

		if (ImGui::IsMousePosValid(&mouse_pos)) {
			_cursor_pos = _transform.transform_point({ static_cast<T>(mouse_pos.x), static_cast<T>(mouse_pos.y) });
		}

		if constexpr (std::is_same_v<T, float>) {

			ImGui::InputFloat("cx", &_cursor_pos.x(), 0, 0, "%.10f", ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat("cy", &_cursor_pos.y(), 0, 0, "%.10f", ImGuiInputTextFlags_ReadOnly);
		}
		else if constexpr (std::is_same_v<T, double>) {
			ImGui::InputDouble("cx", &_cursor_pos.x(), 0, 0, "%.10f", ImGuiInputTextFlags_ReadOnly);
			ImGui::InputDouble("cy", &_cursor_pos.y(), 0, 0, "%.10f", ImGuiInputTextFlags_ReadOnly);
		}
		else if constexpr (std::is_same_v<T, boost::multiprecision::mpfr_float>) {
			// ImGui::InputMPFRFloat("cx", &x_new);
			// ImGui::InputMPFRFloat("cy", &y_new);
		}

		// click to move centre
		if (io.MouseClicked[ImGuiMouseButton_Right]) {
			_transform.set_position(_cursor_pos); // error: position is top left. we need to make it centered. get centre by .centre()
		}

		// mouse wheel zoom 
		if (io.MouseWheel > 0.0f) {
			if (zoom_new < UINT64_MAX) {
				++zoom_new;
			}
		}
		if (io.MouseWheel < 0.0f) {
			if (zoom_new > 1) {
				--zoom_new;
			}
		}

		if (zoom_new != zoom_curr) {
			if (_zoom_on_centre) {
				_transform.set_zoom(zoom_new, _transform.centre());
			}
			else {
				_transform.set_zoom(zoom_new, _cursor_pos);
			}
		}
	}
}
