#pragma once

#include <concepts>

#include <Eigen/Dense>
#include <boost/multiprecision/mpfr.hpp>
#include <boost/multiprecision/number.hpp>



namespace mp = boost::multiprecision;

template <typename T>
concept Real = std::is_floating_point_v<T> || mp::is_number<T>::value;

template <Real T>
class TransformGUI;

template <Real T>
class Transform {
	//friend class TransformGUI<T>;
private:
	Eigen::Vector2i _size;
	unsigned long long _zoom = 1;
	Eigen::Vector2<T> _position = { 0.0, 0.0 };
	Eigen::Vector4<T> _rect = { -2.0, 1.0, -1.5, 1.5 }; // (xmin, xmax, ymin, ymax)

	//Eigen::Matrix3<T> _transform_matrix; // class invariant: transform matrix 

	Eigen::Vector2<T> _scale; // invariant: this is updated in the setters
	Eigen::Vector2<T> _translate;

	bool _changed = true;

private:
	void compute_transform();

public:
	Transform(Eigen::Vector2i size);


public:
	// transform a single point from screen space to fractal space
	Eigen::Vector2<T> transform_point(const Eigen::Vector2<T> p);

	// inverse transfrom from fractal space to screen space
	Eigen::Vector2<T> inverse_transform_point(const Eigen::Vector2<T> p);
	
	// generate and transform all points. x and y are sized size.x * size.y, must be deleted after
	void transform_all(T*& x, T*& y) const;

	Eigen::Vector2<T> centre();
public:
	// non trivial setters update transformation matrix after

	void set_position(const Eigen::Vector2<T>& position);
	void set_zoom(unsigned long long zoom, const Eigen::Vector2<T> point);
	void set_rect(const Eigen::Vector4<T>& rect);
	void set_size(const Eigen::Vector2i& size);

	//void pan(const Eigen::Vector2<T>& difference);
	//void zoom(unsigned long long factor);

	const Eigen::Vector2i& get_size() const { return _size; };
	const unsigned long long& get_zoom() const { return _zoom; };
	const Eigen::Vector2<T>& get_position() const { return _position; };

	// returns changed and resets 
	bool changed();
};

template<Real T>
inline Transform<T>::Transform(Eigen::Vector2i size)
	: _size(size)
{
	compute_transform();
}


template<Real T>
inline void Transform<T>::compute_transform()
{
	T w0 = _rect.y() - _rect.x(); // initial width
	T h0 = _rect.w() - _rect.z(); // initial height

	T z = static_cast<T>(_zoom);

	_scale.x() = w0 / (static_cast<T>(_size.x()) * z);
	_scale.y() = h0 / (static_cast<T>(_size.y()) * z);

	_position.x() = -( w0 / static_cast<T>(2));
	_position.y() = -( h0 / static_cast<T>(2));
}


template<Real T>
inline Eigen::Vector2<T> Transform<T>::transform_point(const Eigen::Vector2<T> p)
{
	return _scale.cwiseProduct(p) + _position;
}

template<Real T>
inline Eigen::Vector2<T> Transform<T>::inverse_transform_point(const Eigen::Vector2<T> p)
{
	return (p - _position).cwiseQuotient(_scale);
}

template<Real T>
inline void Transform<T>::transform_all(T*& x, T*& y) const
{
	
	x = new T[_size.x() * _size.y()];
	y = new T[_size.x() * _size.y()];

	for (size_t i = 0; i < _size.y(); i++) {
		for (size_t j = 0; j < _size.x(); j++) {
			x[_size.x() * i + j] = _scale.x() * static_cast<T>(j) + _position.x();
			y[_size.x() * i + j] = _scale.y() * static_cast<T>(i) + _position.y();

		}
	}

}

template<Real T>
inline Eigen::Vector2<T> Transform<T>::centre()
{
	return _position + _scale.cwiseProduct(_size.cast<T>() / 2.0);
}

template<Real T>
inline void Transform<T>::set_position(const Eigen::Vector2<T>& position)
{
	_position = position;
	_changed = true;
}

template<Real T>
inline void Transform<T>::set_zoom(unsigned long long zoom, const Eigen::Vector2<T> point)
{
	Eigen::Vector2<T> screen = inverse_transform_point(point);
	_zoom = zoom;
	compute_transform();
	_position = point - screen.cwiseProduct(_scale); // preserve centre location
	_changed = true;
}

template<Real T>
inline void Transform<T>::set_rect(const Eigen::Vector4<T>& rect)
{
	_rect = rect;
	compute_transform();
	_changed = true;
}

template<Real T>
inline void Transform<T>::set_size(const Eigen::Vector2i& size)
{
	// modify screen size in place 
	_scale.x() = (_scale.x() * static_cast<T>(_size.x())) / static_cast<T>(size.x());
	_scale.y() = (_scale.y() * static_cast<T>(_size.y())) / static_cast<T>(size.y());

	_size = size;
	_changed = true;
}

template<Real T>
inline bool Transform<T>::changed()
{
	bool val = _changed;
	_changed = false;
	return val;
}
