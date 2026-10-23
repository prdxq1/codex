#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "../include/stb_image.h"
#include "../include/stb_image_write.h"
#include "../include/Image.h"
#include <cstring>
#include <stdexcept>
#include <algorithm>

Image::Image() = default;

Image::Image(int width, int height, int nChannels)
	: m_width(width), m_height(height), m_nChannels(nChannels) {
	if (m_width > 0 && m_height > 0 && m_nChannels > 0 && m_nChannels <= 4) {
		m_size = static_cast<size_t>(m_width) * m_height * m_nChannels;
		m_data = std::make_unique<uint8_t[]>(m_size);
		std::memset(m_data.get(), 0, m_size);

		if (m_nChannels == 2 || m_nChannels == 4) {
			for (size_t i = m_nChannels - 1; i < m_size; i += m_nChannels) {
				m_data[i] = 255;
			}
		}
	} else {
		m_width = 0;
		m_height = 0;
		m_nChannels = 0;
		m_size = 0;
	}
}

Image::Image(const std::string& filepath) {
	if (!isRead(filepath)) {
		throw std::runtime_error("Failed to load image: " + filepath);
	}
}

Image::Image(const Image& other)
	: m_width(other.m_width), m_height(other.m_height),
	  m_nChannels(other.m_nChannels), m_size(other.m_size) {
	if (other.m_data && m_size > 0) {
		m_data = std::make_unique<uint8_t[]>(m_size);
		std::memcpy(m_data.get(), other.m_data.get(), m_size);
	}
}

Image& Image::operator=(const Image& other) {
	if (this == &other) return *this;

	m_width = other.m_width;
	m_height = other.m_height;
	m_nChannels = other.m_nChannels;
	m_size = other.m_size;

	if (other.m_data && m_size > 0) {
		m_data = std::make_unique<uint8_t[]>(m_size);
		std::memcpy(m_data.get(), other.m_data.get(), m_size);
	} else {
		m_data.reset();
	}

	return *this;
}

bool Image::isRead(const std::string& filepath) {
	int w = 0, h = 0, n = 0;
	unsigned char* raw = stbi_load(filepath.c_str(), &w, &h, &n, 0);
	if (!raw) return false;

	m_width = w;
	m_height = h;
	m_nChannels = n;
	m_size = static_cast<size_t>(w) * h * n;
	m_data = std::make_unique<uint8_t[]>(m_size);

	std::memcpy(m_data.get(), raw, m_size);
	stbi_image_free(raw);
	return true;
}

bool Image::isWrite(const std::string& filepath) const {
	if (!m_data || m_width <= 0 || m_height <= 0 || m_nChannels <= 0) {
		return false;
	}
	int stride = m_width * m_nChannels;
	return stbi_write_png(filepath.c_str(), m_width, m_height, m_nChannels, m_data.get(), stride) != 0;
}

void Image::setColor(int x, int y, int r, int g, int b) {
    if (!m_data) return;
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) return;

    size_t index = (static_cast<size_t>(y) * m_width + x) * m_nChannels;

    if (m_nChannels == 1) {
        m_data[index] = static_cast<uint8_t>(r);
    } else if (m_nChannels == 2) {
        m_data[index + 0] = static_cast<uint8_t>(r);
        m_data[index + 1] = 255; // Alpha = непрозрачный
    } else if (m_nChannels == 3) {
        m_data[index + 0] = static_cast<uint8_t>(r);
        m_data[index + 1] = static_cast<uint8_t>(g);
        m_data[index + 2] = static_cast<uint8_t>(b);
    } else if (m_nChannels == 4) {
        m_data[index + 0] = static_cast<uint8_t>(r);
        m_data[index + 1] = static_cast<uint8_t>(g);
        m_data[index + 2] = static_cast<uint8_t>(b);
        m_data[index + 3] = 255;
    }
}

void Image::getColor(int x, int y, int& r, int& g, int& b) const {
	if (!m_data) return;
	if (x < 0 || x >= m_width || y < 0 || y >= m_height) return;

	size_t index = (static_cast<size_t>(y) * m_width + x) * m_nChannels;

	if (m_nChannels == 1 || m_nChannels == 2) {
		// костыль
		r = m_data[index];
	} else if (m_nChannels >= 3) {
		r = m_data[index + 0];
		g = m_data[index + 1];
		b = m_data[index + 2];
	}
}
