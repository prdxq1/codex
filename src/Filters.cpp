#define _USE_MATH_DEFINES

#include "../include/Filters.h"
#include <algorithm>
#include <cmath>
#include <vector>
#include <stdexcept>

namespace Filters {

    inline int clampCoord(int val, int maxVal) {
        return std::clamp(val, 0, maxVal - 1);
    }

    inline int clampByte(int val) {
        return std::clamp(val, 0, 255);
    }

    // Вспомогательный метод: извлечение одного канала яркости
    static int getPixelChannel(const Image& img, int x, int y, int c) {
        int r = 0, g = 0, b = 0;
        img.getColor(clampCoord(x, img.width()), clampCoord(y, img.height()), r, g, b);
        if (img.nChannels() <= 2) {
            return r; // Для 1-2 каналов всегда возвращаем r
        }
        if (c == 0) return r;
        if (c == 1) return g;
        return b;
    }

    Image toGrayscale(const Image& src) {
        Image dst(src.width(), src.height(), 1);
        for (int y = 0; y < src.height(); ++y) {
            for (int x = 0; x < src.width(); ++x) {
                int r = 0, g = 0, b = 0;
                src.getColor(x, y, r, g, b);
                int gray = (src.nChannels() >= 3)
                    ? static_cast<int>(0.299 * r + 0.587 * g + 0.114 * b)
                    : r;
                dst.setColor(x, y, gray, gray, gray);
            }
        }
        return dst;
    }

    // ==========================================
    // ВАРИАНТ №1: Выделение контуров
    // ==========================================

    Image applySobel(const Image& src) {
        Image gray = toGrayscale(src);
        int w = gray.width();
        int h = gray.height();
        Image dst(w, h, 1);

        const int Gx[3][3] = {
            {-1, 0, 1},
            {-2, 0, 2},
            {-1, 0, 1}
        };

        const int Gy[3][3] = {
            {-1, -2, -1},
            { 0,  0,  0},
            { 1,  2,  1}
        };

        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                double sumX = 0.0;
                double sumY = 0.0;

                for (int ky = -1; ky <= 1; ++ky) {
                    for (int kx = -1; kx <= 1; ++kx) {
                        int pixel = getPixelChannel(gray, x + kx, y + ky, 0);
                        sumX += pixel * Gx[ky + 1][kx + 1];
                        sumY += pixel * Gy[ky + 1][kx + 1];
                    }
                }

                int mag = clampByte(static_cast<int>(std::sqrt(sumX * sumX + sumY * sumY)));
                dst.setColor(x, y, mag, mag, mag);
            }
        }
        return dst;
    }

    Image applyRoberts(const Image& src) {
        Image gray = toGrayscale(src);
        int w = gray.width();
        int h = gray.height();
        Image dst(w, h, 1);

        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                int p00 = getPixelChannel(gray, x,     y,     0);
                int p11 = getPixelChannel(gray, x + 1, y + 1, 0);
                int p01 = getPixelChannel(gray, x + 1, y,     0);
                int p10 = getPixelChannel(gray, x,     y + 1, 0);

                double gx = p00 - p11;
                double gy = p01 - p10;

                int mag = clampByte(static_cast<int>(std::sqrt(gx * gx + gy * gy)));
                dst.setColor(x, y, mag, mag, mag);
            }
        }
        return dst;
    }

    Image applyLaplacian(const Image& src) {
        Image gray = toGrayscale(src);
        int w = gray.width();
        int h = gray.height();
        Image dst(w, h, 1);

        const int kernel[3][3] = {
            { 0,  1,  0 },
            { 1, -4,  1 },
            { 0,  1,  0 }
        };

        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                int sum = 0;
                for (int ky = -1; ky <= 1; ++ky) {
                    for (int kx = -1; kx <= 1; ++kx) {
                        int pixel = getPixelChannel(gray, x + kx, y + ky, 0);
                        sum += pixel * kernel[ky + 1][kx + 1];
                    }
                }
                int val = clampByte(std::abs(sum));
                dst.setColor(x, y, val, val, val);
            }
        }
        return dst;
    }

    // ==========================================
    // ВАРИАНТ №2: Усредняющие фильтры
    // ==========================================

    Image applyBoxBlur(const Image& src, int kernelSize) {
        if (kernelSize <= 0 || kernelSize % 2 == 0 || kernelSize > 255) throw std::invalid_argument("Odd kernel size 1..255 required");
        int w = src.width();
        int h = src.height();
        int channels = src.nChannels();
        Image dst(w, h, channels);
        int radius = kernelSize / 2;
        int count = kernelSize * kernelSize;

        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                int sumR = 0, sumG = 0, sumB = 0;

                for (int ky = -radius; ky <= radius; ++ky) {
                    for (int kx = -radius; kx <= radius; ++kx) {
                        int r = 0, g = 0, b = 0;
                        src.getColor(clampCoord(x + kx, w), clampCoord(y + ky, h), r, g, b);
                        sumR += r;
                        sumG += g;
                        sumB += b;
                    }
                }

                dst.setColor(x, y, sumR / count, sumG / count, sumB / count);
            }
        }
        return dst;
    }

    Image applyGaussianBlur(const Image& src, int kernelSize, double sigma) {
        if (kernelSize <= 0 || kernelSize % 2 == 0 || kernelSize > 255) throw std::invalid_argument("Odd kernel size 1..255 required");
        if (!std::isfinite(sigma) || sigma < 1e-6 || sigma > 1e6) throw std::invalid_argument("sigma must be finite, 1e-6..1e6");
        int w = src.width();
        int h = src.height();
        int channels = src.nChannels();
        Image dst(w, h, channels);
        int radius = kernelSize / 2;

        std::vector<std::vector<double>> kernel(kernelSize, std::vector<double>(kernelSize));
        double sumWeights = 0.0;
        double s = 2.0 * sigma * sigma;

        for (int y = -radius; y <= radius; ++y) {
            for (int x = -radius; x <= radius; ++x) {
                double r = x * x + y * y;
                double val = std::exp(-r / s) / (3.14159265358979323846 * s);
                kernel[y + radius][x + radius] = val;
                sumWeights += val;
            }
        }

        for (int i = 0; i < kernelSize; ++i) {
            for (int j = 0; j < kernelSize; ++j) {
                kernel[i][j] /= sumWeights;
            }
        }

        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                double accR = 0.0, accG = 0.0, accB = 0.0;

                for (int ky = -radius; ky <= radius; ++ky) {
                    for (int kx = -radius; kx <= radius; ++kx) {
                        int r = 0, g = 0, b = 0;
                        src.getColor(clampCoord(x + kx, w), clampCoord(y + ky, h), r, g, b);
                        double weight = kernel[ky + radius][kx + radius];
                        accR += r * weight;
                        accG += g * weight;
                        accB += b * weight;
                    }
                }

                dst.setColor(x, y, clampByte(static_cast<int>(accR + 0.5)),
                                  clampByte(static_cast<int>(accG + 0.5)),
                                  clampByte(static_cast<int>(accB + 0.5)));
            }
        }
        return dst;
    }


} // namespace Filters
