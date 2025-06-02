#pragma once

#include "Engine.h"

template <Real T>
class EscapeTimeEngine : public Engine<T> {
public:
	EscapeTimeEngine() = default;

	void compute(const Transform<T>& transform, Surface<uint32_t> out, uint32_t max_it) override;
};

template<Real T>
inline void EscapeTimeEngine<T>::compute(const Transform<T>& transform, Surface<uint32_t> out, uint32_t max_it)
{
	int sx = transform.get_size().x();
	int sy = transform.get_size().y();

	transform.transform_all(this->_cx, this->_cy);

	for (size_t i = 0; i < sx * sy; i++) {
		out[i] = 0; // iterations
		T zx = 0.0;
		T zy = 0.0;
		while (zx * zx + zy * zy <= static_cast<T>(4.0) && out[i] < max_it) {
			T xtmp = zx * zx - zy * zy + this->_cx[i];
			zy = static_cast<T>(2.0) * zx * zy + this->_cy[i];
			zx = xtmp;
			out[i]++;
		}
	}


	delete[] this->_cx;
	delete[] this->_cy;
}