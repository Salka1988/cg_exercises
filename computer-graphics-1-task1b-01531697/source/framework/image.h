


#ifndef INCLUDED_FRAMEWORK_IMAGE
#define INCLUDED_FRAMEWORK_IMAGE

#pragma once

#include <cstddef>
#include <memory>
#include <algorithm>

#include <framework/utils/memory.h>


template <typename T>
class image2D
{
	std::unique_ptr<T[]> img;

	std::size_t width = 0;
	std::size_t height = 0;

	static constexpr std::size_t size(std::size_t width, std::size_t height)
	{
		return width * height;
	}

	static auto alloc(std::size_t width, std::size_t height)
	{
		return make_unique_default<T[]>(size(width, height));
	}

public:
	image2D() = default;

	image2D(std::size_t width, std::size_t height)
		: img(alloc(width, height)),
		  width(width),
		  height(height)
	{
	}

	image2D(const image2D& s)
		: img(alloc(s.width, s.height)),
		  width(s.width),
		  height(s.height)
	{
		std::copy(&s.img[0], &s.img[0] + size(width, height), &img[0]);
	}

	image2D(image2D&& s) = default;

	image2D& operator =(const image2D& s)
	{
		width = s.width;
		height = s.height;
		auto buffer = alloc(width, height);
		std::copy(&s.img[0], &s.img[0] + size(width, height), &buffer[0]);
		img = move(buffer);
		return *this;
	}

	image2D& operator =(image2D&& s) = default;

	T& operator ()(std::size_t x, std::size_t y) const noexcept { return img[y * width + x]; }
	T& operator ()(std::size_t x, std::size_t y) noexcept { return img[y * width + x]; }

	image2D& clear(T color)
	{
		std::fill(&img[0], &img[0] + size(width, height), color);
		return *this;
	}

	friend std::size_t width(const image2D& img) noexcept
	{
		return img.width;
	}

	friend std::size_t height(const image2D& img) noexcept
	{
		return img.height;
	}

	friend const T* data(const image2D& img) noexcept
	{
		return &img.img[0];
	}

	friend T* data(image2D& img) noexcept
	{
		return &img.img[0];
	}
};

#endif  // INCLUDED_FRAMEWORK_IMAGE
