#pragma once

#include <Eigen/Dense>

using Eigen::Vector2, Eigen::Vector2i, Eigen::Matrix3;
 

template <typename T>
class Viewport {
private:
	Vector2<T> _position;
	Vector2i _windowSize;
	T _zoom;

public:
	Viewport() {};
	~Viewport() {};


};