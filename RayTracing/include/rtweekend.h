#pragma once
#include <cmath>
#include <limits>
#include <memory>
#include <random>
// Usings

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

// Common Headers

#include "ray.h"
#include "vec3.h"

#include <cstdlib>

inline double random_double() {
    //rand() return [0,RAND_MAX]
    // Returns a random real in [0,1).
    return rand() / (RAND_MAX + 1.0);
}


//为什么出问题？
//inline double random_double()
//{
//    static std::random_device dev;
//    std::mt19937_64 rng(dev());
//    std::uniform_real_distribution<double> dist(0.f, 1.f);
//
//    return dist(rng);
//}




inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max - min) * random_double();
}

//#include <random>
//
//inline double random_double() {
//    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
//    static std::mt19937 generator;
//    return distribution(generator);
//}

inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

inline int random_int(int min, int max) {
    // Returns a random integer in [min,max].
    return static_cast<int>(random_double(min, max + 1));
}
inline double lerp(double a, double b, double f) {
    return a + (b - a) * f;
}

//  -2t^3 + 3t^2  一阶导数为0 在0/1处
inline double smooth_3order(double t) {
    return t * t * (3 - 2 * t);
}
//   6t^5-15t^4+10t^3  二阶导数为0 在0/1处
inline double smooth_5order(double t) {
    return t * t * t *(t*(6*t-15)+10);
}

//reject method
//vec3 random_in_unit_sphere() {
//    while (true) {
//        auto p = vec3::random(-1, 1);
//        if (p.length_squared() >= 1) continue;
//        return p;
//    }
//}
//vec3 random_unit_vector() {
//    return unit_vector(random_in_unit_sphere());
//}
//vec3 random_in_hemisphere(const vec3& normal) {
//    vec3 in_unit_sphere = random_in_unit_sphere();
//    if (dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
//        return in_unit_sphere;
//    else
//        return -in_unit_sphere;
//}
//vec3 random_in_unit_disk() {
//    while (true) {
//        auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
//        if (p.length_squared() >= 1) continue;
//        return p;
//    }
//}


//sample theory renference
//https://zhuanlan.zhihu.com/p/376432029
//theta is the angle with z axis
vec3 random_on_unit_sphere() {// cos theta = 1-2rand1   phi = 2* PI*rand2
    double phi = 2*pi*random_double();
    double rand1 = random_double();
    double sintheta =2*sqrt(rand1 * (1 - rand1));
    return {cos(phi) * sintheta, sin(phi) * sintheta,1.0 - 2 * rand1 };
}
vec3 random_on_unit_hemisphere(){//cos theta = rand1  phi = 2*PI*rand2
    double phi = 2*pi*random_double();
    double costheta = random_double();
    double sintheta = sqrt(1-costheta*costheta);
    return { sintheta * cos(phi),sintheta*sin(phi),costheta };
}
vec3 random_in_unit_sphere(){
    double radius = cbrt(random_double());
    double phi = 2*pi*random_double();
    double rand1 = random_double();
    double sintheta =2*sqrt(rand1 * (1 - rand1));
    return {radius*cos(phi)*sintheta,radius*sin(phi)*sintheta, radius * (1.0-2*rand1) };
}

vec3 random_on_triangle(const vec3&A,const vec3& B,const vec3& C){
    double u = random_double();
    double v = (1-u)*random_double();
    return {u*A+v*B+(1-u-v)*C};
}

vec3 random_on_unit_disk() {
    double phi = 2 * pi * random_double();
    double radius = sqrt(random_double());
    return { radius * cos(phi),radius * sin(phi),0 };
}

inline void UpdateProgress(float progress)
{
    int barWidth = 70;

    std::cout << "[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();
};