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


/* Transforms from screen space into fractal space
*/
template <Real T>
class Transform {
private:
	unsigned long long _zoom = 1; // quotient of scale
	Eigen::Vector4<T> _rect = { -2.0, 1.0, -1.5, 1.5 }; // (xmin, xmax, ymin, ymax)
	Eigen::Vector2i _size; // size of screen in pixels
	Eigen::Vector2<T> _position; // represents centre of viewport in fractal space

	// Eigen::Vector2<T> _scale; // invariant: this is updated in the setters
	T _scale; // uniform scale

	bool _changed = true;

	void compute_scale();

public:
	Transform(Eigen::Vector2i size);


public:
	// transform a single point from screen space to fractal space
	Eigen::Vector2<T> transform_point(const Eigen::Vector2<T> p) const;

	// inverse transfrom from fractal space to screen space
	Eigen::Vector2<T> inverse_transform_point(const Eigen::Vector2<T> p) const;

	// generate and transform all points. x and y are sized size.x * size.y, must be deleted after
	void transform_all(T*& x, T*& y) const;
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
	const Eigen::Vector4<T>& get_rect() const { return _rect;  }

	// get centre of screen in fractal coordinates
	Eigen::Vector2<T> centre() const;

	// returns changed and resets 
	bool changed();
};

template<Real T>
inline void Transform<T>::compute_scale()
{
	Eigen::Vector2<T> range = {
		_rect.y() - _rect.x(),
		_rect.w() - _rect.z()
	};
	Eigen::Vector2<T> px_size = range.cwiseQuotient(_size.cast<T>());
	_scale = px_size.maxCoeff() / static_cast<T>(_zoom);
}


template<Real T>
inline Transform<T>::Transform(Eigen::Vector2i size)
	: _size(size)
{
	compute_scale();

	_position = Eigen::Vector2<T>({ _rect.x(), _rect.z() }) + _scale * (_size.cast<T>() / 2.0);
}

template<Real T>
inline Eigen::Vector2<T> Transform<T>::transform_point(const Eigen::Vector2<T> p) const
{
	return (p - _size.cast<T>() / 2) * _scale + _position;
}

template<Real T>
inline Eigen::Vector2<T> Transform<T>::inverse_transform_point(const Eigen::Vector2<T> p) const
{
	return (p - _position) / _scale + _size.cast<T>() / 2;
}

template<Real T>
inline void Transform<T>::transform_all(T*& x, T*& y) const
{
	const Eigen::Vector2<T> centre = _size.cast<T>() / 2;

	x = new T[_size.x() * _size.y()];
	y = new T[_size.x() * _size.y()];

	for (size_t i = 0; i < _size.y(); i++) {
		for (size_t j = 0; j < _size.x(); j++) {
			x[_size.x() * i + j] = _scale * (static_cast<T>(j) - centre.x()) + _position.x();
			y[_size.x() * i + j] = _scale * (static_cast<T>(i) - centre.y()) + _position.y();
		}
	}

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
	// point must be fixed. if p in screen space, t(p) = t'(p) 
	Eigen::Vector2<T> anchor = inverse_transform_point(point);

	_zoom = zoom;
	compute_scale();

	_position = point - _scale * (anchor - _size.cast<T>() / 2);

	_changed = true;
}

template<Real T>
inline void Transform<T>::set_rect(const Eigen::Vector4<T>& rect)
{
	_rect = rect;
	compute_scale();
	_changed = true;
}

template<Real T>
inline void Transform<T>::set_size(const Eigen::Vector2i& size)
{
	// t(centre) == t'(centre) 
	Eigen::Vector2<T> offset_old = (_size.cast<T>() / 2 )* _scale;
	
	_size = size;
	compute_scale(); // updates _scale

	Eigen::Vector2<T> offset_new = (_size.cast<T>() / 2) * _scale;

	_position += offset_old - offset_new;

	_changed = true;
}

template<Real T>
inline Eigen::Vector2<T> Transform<T>::centre() const
{
	return transform_point(_size.cast<T>() / static_cast<T>(2));
}

template<Real T>
inline bool Transform<T>::changed()
{
	bool val = _changed;
	_changed = false;
	return val;
}
