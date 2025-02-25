#include <iostream>
#include <cmath>
#include <vector>
#include <chrono>

#include <Eigen/Dense>


#include <imgui.h>

#include "MandelbrotEngineAVX2/escape_time_avx2.h"
#include "Window.h"

#define ALIGN32 __declspec(align(32))

using Eigen::Matrix;
using Eigen::Vector2;
using Eigen::Vector3;

using Eigen::Vector2i;



template <typename T>
Matrix<T, 3, 3> computeTransformationMatrix(const Vector2i& window, const Vector2<T>& position, T zoom) {
	T a = static_cast<T>(std::max(window.x(), window.y()));

	Matrix<T,3,3> mat;

	mat(0, 0) = (2 * zoom) / (window.x() * a);
	mat(0, 1) = 0.0;
	mat(0, 2) = position.x() - (zoom / a);

	mat(1, 0) = 0.0;
	mat(1, 1) = (2 * zoom) / (window.y() * a);
	mat(1, 2) = position.y() - (zoom / a);

	mat(1, 0) = 0.0;
	mat(1, 1) = 0.0;
	mat(1, 2) = 1.0;

	return mat;
}

template <typename T>
void generateCoords(const Vector2i& window, const Matrix<T,3,3>& transform, T* cx, T* cy) {

	for (int i = 0; i < window.y(); i++) {
		for (int j = 0; j < window.x(); j++) {
			Vector3<T> c = transform * Vector3<T>(static_cast<T>(j), static_cast<T>(i), 1);
			size_t idx = i * window.x() + j;

			cx[idx] = c.x();
			cy[idx] = c.y();
		}
	}
	return;
}

int main(int argc, char* argv[]) {

	/*
	Vector2i window;
	window << 2560, 1440;

	Vector2<float> position;
	position << 0.0f, 0.0f;

	float zoom = 1.0f;

	Matrix<float,3,3> transform = computeTransformationMatrix(window, position, zoom);


	size_t n = window.x() * window.y();
	ALIGN32 float* cx = new float[n];
	ALIGN32 float* cy = new float[n];
	ALIGN32 unsigned* iters = new unsigned[n];

	auto start = std::chrono::high_resolution_clock::now();
	

	generateCoords<float>(window, transform, cx, cy);

	

	auto stop = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

	std::cout << "eigen transformation took " << duration.count() << " microseconds";


	for (int i = 0; i < window.x() * window.y(); i += 8) {
		escape_time_avx2(cx + i, cy + i, iters + i, 256);
	}*/

	Window window(L"mandel", 1920, 1080);

	bool show_demo_window = true;

	while (!window.ShouldClose()) {
		window.BeginFrame();

		ImGui::ShowDemoWindow(&show_demo_window);

		window.EndFrame();
	}




	return EXIT_SUCCESS;
}