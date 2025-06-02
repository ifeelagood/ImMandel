#pragma once

template <typename T>
struct Surface {
	T* base = nullptr;
	size_t pitch;
	unsigned w;
	unsigned h;

	T& at(size_t x, size_t y) {
		char* row = reinterpret_cast<char*>(base) + y * pitch;
		return reinterpret_cast<T*>(row)[x];
	}

	const T& at(size_t x, size_t y) const {
		const char* row = reinterpret_cast<const char*>(base) + y * pitch;
		return reinterpret_cast<const T*>(row)[x];
	}

	T& operator[](size_t idx) {
		size_t y = idx / static_cast<size_t>(w);
		size_t x = idx % static_cast<size_t>(w);
		return at(x, y);
	}

	const T& operator[](size_t idx) const {
		size_t y = idx / static_cast<size_t>(w);
		size_t x = idx % static_cast<size_t>(w);
		return at(x, y);
	}
};