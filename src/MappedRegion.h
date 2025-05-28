#pragma once

#include <span>

template <typename T>
struct MappedRegion {
	void* base = nullptr;
	unsigned stride = 0;
	size_t size = 0;

	MappedRegion(void* base, unsigned stride, size_t size) 
		: base(base), stride(stride), size(size) {}

	T& operator[](size_t i) {
		return *(reinterpret_cast<T*>(base) + i * stride);
	}

	const T& operator[](size_t i) const {
		return *reinterpret_cast<T*>(base) + i * stride;
	}

	std::span<T> range(size_t offset, size_t length) {
		return std::span<T>(reinterpret_cast<T*>(base) + offset * stride, length);
	}
};
