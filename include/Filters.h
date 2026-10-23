#pragma once
#include "Image.h"
#include <vector>
#include <string>

namespace Filters {

    // ==========================================
    // ВАРИАНТ №1: Выделение контуров
    // ==========================================

    // 1. Оператор Собеля (Sobel)
    //    Использует дифференциальные маски 3x3 по X и Y:
    //    Gx = [[-1, 0, 1], [-2, 0, 2], [-1, 0, 1]]
    //    Gy = [[-1, -2, -1], [0, 0, 0], [1, 2, 1]]
    //    G = sqrt(Gx^2 + Gy^2)
    Image applySobel(const Image& src);

    // 2. Оператор Робертса (Roberts Cross)
    //    Использует диагональные маски 2x2:
    //    Gx = [[1, 0], [0, -1]], Gy = [[0, 1], [-1, 0]]
    //    G = sqrt(Gx^2 + Gy^2)
    Image applyRoberts(const Image& src);

    // 3. Оператор Лапласа (Laplacian)
    //    Вторая производная (изотропный оператор):
    //    Ядро 3x3: [[0, 1, 0], [1, -4, 1], [0, 1, 0]]
    //    или с диагоналями: [[1, 1, 1], [1, -8, 1], [1, 1, 1]]
    Image applyLaplacian(const Image& src);


    // ==========================================
    // ВАРИАНТ №2: Усредняющие фильтры (сглаживание)
    // ==========================================

    // 1. Простое прямоугольное / арифметическое усреднение (Box Blur)
    //    Матрица k x k с весами 1 / (k * k)
    Image applyBoxBlur(const Image& src, int kernelSize = 3);

    // 2. Взвешенное гауссово усреднение (Gaussian Blur)
    //    Аппроксимация 2D распределения Гаусса с заданным радиусом и sigma
    Image applyGaussianBlur(const Image& src, int kernelSize = 3, double sigma = 1.0);



} // namespace Filters
