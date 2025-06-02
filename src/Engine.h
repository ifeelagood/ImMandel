#pragma once

#include <cstdint>
#include <span>

#include "Surface.h"
#include "Transform.h"

template <Real T>
class Engine {
protected:
	T* _cx = nullptr;
	T* _cy = nullptr;

public:
	// reallocate internal buffers
	//virtual void reserve(size_t px);

	// compute iterations for a transform
	virtual void compute(const Transform<T>& transform, Surface<uint32_t> out, uint32_t max_it) = 0;
	virtual ~Engine() = default;
};