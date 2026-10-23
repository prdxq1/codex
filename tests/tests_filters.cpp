#include "doctest.h"
#include "Filters.h"
#include <cmath>
#include <limits>
static int pixel(const Image& image,int x,int y) {int r=0,g=0,b=0; image.getColor(x,y,r,g,b);return r;}
TEST_CASE("Constant image: zero edges, preserved average including borders") {
 Image image(9,9,1);for(int y=0;y<9;++y)for(int x=0;x<9;++x)image.setColor(x,y,100,100,100);
 for(const auto& result:{Filters::applyLaplacian(image),Filters::applyRoberts(image),Filters::applySobel(image)})
  for(int y=0;y<9;++y)for(int x=0;x<9;++x)CHECK(pixel(result,x,y)==0);
 for(const auto& result:{Filters::applyBoxBlur(image,3),Filters::applyBoxBlur(image,5),Filters::applyGaussianBlur(image,5,1.5)})
  for(int y=0;y<9;++y)for(int x=0;x<9;++x)CHECK(pixel(result,x,y)==100);
}
TEST_CASE("Impulse response distinguishes Laplace, Roberts, Sobel and box") {
 Image image(7,7,1);image.setColor(3,3,90,90,90);
 CHECK(pixel(Filters::applyLaplacian(image),3,3)==255);
 CHECK(pixel(Filters::applyLaplacian(image),2,3)==90);
 CHECK(pixel(Filters::applySobel(image),3,3)==0);
 CHECK(pixel(Filters::applySobel(image),2,3)==180);
 CHECK(pixel(Filters::applyRoberts(image),3,3)==90);
 CHECK(pixel(Filters::applyBoxBlur(image,3),3,3)==10);
 CHECK(pixel(Filters::applyBoxBlur(image,3),1,1)==0);
}
TEST_CASE("Gaussian symmetric impulse and rejected parameters") {
 Image image(9,9,1);image.setColor(4,4,255,255,255);
 auto result=Filters::applyGaussianBlur(image,5,1.5);
 CHECK(pixel(result,4,4)>pixel(result,3,4));
 CHECK(pixel(result,3,4)==pixel(result,5,4));
 CHECK(pixel(result,4,3)==pixel(result,4,5));
 CHECK_THROWS_AS(Filters::applyBoxBlur(image,0),std::invalid_argument);
 CHECK_THROWS_AS(Filters::applyBoxBlur(image,4),std::invalid_argument);
 CHECK_THROWS_AS(Filters::applyGaussianBlur(image,3,0),std::invalid_argument);
 CHECK_THROWS_AS(Filters::applyGaussianBlur(image,3,std::numeric_limits<double>::quiet_NaN()),std::invalid_argument);
}
TEST_CASE("Image deep assignment, empty image and channel validation") {
 Image a(2,2,3);a.setColor(0,0,12,34,56);Image b;b=a;a.setColor(0,0,200,200,200);
 CHECK(pixel(b,0,0)==12);b=b;CHECK(pixel(b,0,0)==12);
 CHECK(Image(2,2,5).width()==0);
 CHECK(Filters::applySobel(Image()).width()==0);
 CHECK(Filters::applyBoxBlur(Image(),3).width()==0);
}
