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
	Eigen::Vector2i _size; // size of screen in pixels
	unsigned long long _zoom = 1; // 
	Eigen::Vector2<T> _position = { 0.0, 0.0 };	// represents top left of viewport
	Eigen::Vector4<T> _rect = { -2.0, 1.0, -1.5, 1.5 }; // (xmin, xmax, ymin, ymax)

	Eigen::Vector2<T> _scale; // invariant: this is updated in the setters
	Eigen::Vector2<T> _translate;

	bool _changed = true;

	void compute_scale();
	void compute_translate();

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

	// get centre of screen in fractal coordinates
	Eigen::Vector2<T> centre() const;

	// returns changed and resets 
	bool changed();
};

template<Real T>
inline void Transform<T>::compute_scale()
{
	_scale = (_rect.segment<2>(1) - _rect.segment<2>(0)).cwiseQuotient(_size.cast<T>()) / static_cast<T>(_zoom);
}

template <Real T>
inline void Transform<T>::compute_translate() {
	_translate = _position - _scale.cwiseProduct(_size.cast<T>() / 2);
}

template<Real T>
inline Transform<T>::Transform(Eigen::Vector2i size)
	: _size(size)
{
	compute_scale();
	compute_translate();
}

template<Real T>
inline Eigen::Vector2<T> Transform<T>::transform_point(const Eigen::Vector2<T> p) const
{
	return _scale.cwiseProduct(p) + _translate;
}

template<Real T>
inline Eigen::Vector2<T> Transform<T>::inverse_transform_point(const Eigen::Vector2<T> p) const
{
	return (p - _translate).cwiseQuotient(_scale);
}

template<Real T>
inline void Transform<T>::transform_all(T*& x, T*& y) const
{	
	x = new T[_size.x() * _size.y()];
	y = new T[_size.x() * _size.y()];

	for (size_t i = 0; i < _size.y(); i++) {
		for (size_t j = 0; j < _size.x(); j++) {
			x[_size.x() * i + j] = _scale.x() * static_cast<T>(j) + _translate.x();
			y[_size.x() * i + j] = _scale.y() * static_cast<T>(i) + _translate.y();
		}
	}

}

template<Real T>
inline void Transform<T>::set_position(const Eigen::Vector2<T>& position)
{
	_position = position;
	compute_translate();
	_changed = true;
}

template<Real T>
inline void Transform<T>::set_zoom(unsigned long long zoom, const Eigen::Vector2<T> point)
{	
	Eigen::Vector2<T> point_screen = inverse_transform_point(point);

	_scale = _scale * static_cast<T>(_zoom) / static_cast<T>(zoom);
	_zoom = zoom;
	_translate = point - _scale.cwiseProduct(point_screen);
	_position = _translate + _scale.cwiseProduct(_size.cast<T>() / 2);
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
	// modify screen size in place 
	_scale = _scale.cwiseProduct(_size.cast<T>()).cwiseQuotient(size.cast<T>());
	_size = size;

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
