#include "Image.h"
#include "Filters.h"
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <stdexcept>

// Все входные данные создаются программно, файлы из архивов не используются.
Image generateScene() {
    Image image(256, 256, 1);
    for (int y = 0; y < 256; ++y) for (int x = 0; x < 256; ++x) {
        // Светлый фон и четыре раздельные фигуры, без сцены из образца.
        int value = 195;
        // Равнобедренный треугольник: вершина (60,25), основание y=100.
        if (y >= 25 && y <= 100 && std::abs(x-60)*75 <= (y-25)*40) value = 55;
        // Ромб с центром (183,62).
        if (std::abs(x-183)+std::abs(y-62) <= 38) value = 125;
        // Эллипс с полуосями 42 и 27.
        if ((x-66)*(x-66)/1764.0+(y-174)*(y-174)/729.0 <= 1.0) value = 105;
        // Крест из двух прямоугольников одинаковой толщины.
        if ((x >= 174 && x <= 190 && y >= 139 && y <= 217) ||
            (x >= 147 && x <= 217 && y >= 170 && y <= 186)) value = 40;
        image.setColor(x, y, value, value, value);
    }
    return image;
}
Image addNoise(const Image& clean) {
    Image result(clean);
    std::mt19937 generator(42);
    std::normal_distribution<double> noise(0.0, 25.0);
    for (int y = 0; y < clean.height(); ++y) for (int x = 0; x < clean.width(); ++x) {
        int r=0,g=0,b=0; clean.getColor(x,y,r,g,b);
        int value = std::clamp(static_cast<int>(std::lround(r+noise(generator))),0,255);
        result.setColor(x,y,value,value,value);
    }
    return result;
}
void save(const Image& image, const std::string& path) {
    if (!image.isWrite(path)) throw std::runtime_error("Cannot write " + path);
}
void metrics(std::ostream& out, const std::string& name, const Image& image, const Image& clean) {
    double square=0, flat=0; int count=0;
    for(int y=0;y<256;++y) for(int x=0;x<256;++x) {
        int r=0,g=0,b=0,c=0; image.getColor(x,y,r,g,b); clean.getColor(x,y,c,g,b);
        double e=r-c; square+=e*e;
        if(x>=5 && x<25 && y>=110 && y<230) { flat+=e*e; ++count; }
    }
    double mse=square/65536;
    out << name << ',' << mse << ',' << 10*std::log10(255.0*255/mse) << ',' << flat/count << '\n';
}
int main() {
    try {
        std::filesystem::create_directories("test_input");
        std::filesystem::create_directories("test_output");
        Image clean=generateScene(), noisy=addNoise(clean);
        save(clean,"test_input/clean.png"); save(noisy,"test_input/input.png");
        save(Filters::applyLaplacian(noisy),"test_output/var1_laplacian.png");
        save(Filters::applyRoberts(noisy),"test_output/var1_roberts.png");
        save(Filters::applySobel(noisy),"test_output/var1_sobel.png");
        save(Filters::applyLaplacian(clean),"test_output/clean_laplacian.png");
        save(Filters::applyRoberts(clean),"test_output/clean_roberts.png");
        save(Filters::applySobel(clean),"test_output/clean_sobel.png");
        Image box3=Filters::applyBoxBlur(noisy,3),box5=Filters::applyBoxBlur(noisy,5);
        Image gaussian=Filters::applyGaussianBlur(noisy,5,1.5);
        save(box3,"test_output/var2_box_blur_3x3.png");
        save(box5,"test_output/var2_box_blur_5x5.png");
        save(gaussian,"test_output/var2_gaussian_blur.png");
        std::ofstream out("test_output/metrics.csv");
        if(!out) throw std::runtime_error("Cannot write metrics");
        out << "method,mse,psnr_db,flat_mse\n";
        metrics(out,"noisy",noisy,clean); metrics(out,"box3",box3,clean);
        metrics(out,"box5",box5,clean); metrics(out,"gaussian",gaussian,clean);
        std::cout << "Generated 256x256 grayscale scene, Gaussian noise (sigma=25, seed=42), variants 1 and 2.\n";
    } catch(const std::exception& e) { std::cerr<<e.what()<<'\n'; return 1; }
}
