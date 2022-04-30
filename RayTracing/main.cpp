#include "rtweekend.h"
#include "camera.h"
#include "hittable_list.h"
#include "sphere.h"
#include "material.h"
#include "moving_sphere.h"
#include "frameBuffer.h"
#include<chrono>
#include <mutex>
#include <thread>
#include <iostream>

////Witty style
//color ray_color(const ray& r, const hittable& world, int depth) {
//    hit_record rec;
//    if (depth <= 0)
//        return color(0, 0, 0);
//    if (world.hit(r, 0.001, infinity, rec)) {
//        ray scattered;
//        color attenuation;
//        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
//            return attenuation * ray_color(scattered, world, depth - 1);
//        //in object, return dark;
//        return color(0, 0, 0);
//    }
//    vec3 unit_direction = unit_vector(r.direction());
//    auto t = 0.5 * (unit_direction.y() + 1.0);
//    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
//}




//path tracing
color ray_color(const ray& r, const hittable& world, int depth) {
    hit_record rec;
    if (depth <= 0)
        return color(0, 0, 0);
    if (world.hit(r, 0.001, infinity, rec)) {
        ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth - 1);
        //in object, return dark;
        return color(0, 0, 0);
    }
    //skybox
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}


hittable_list random_scene() {
    hittable_list world;

    auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(checker)));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    auto center2 = center + vec3(0, random_double(0, .5), 0);
                    world.add(make_shared<moving_sphere>(
                        center, center2, 0.0, 1.0, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
                else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}

hittable_list two_spheres() {
    hittable_list objects;

    auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));

    objects.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(checker)));
    objects.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker)));

    return objects;
}

hittable_list two_perlin_spheres() {
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(4);
    objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    return objects;
}

std::mutex print_mutex;
int main() {
    auto startTime = std::chrono::system_clock::now();

    // Image
    auto aspect_ratio = 16.0 / 9.0;
    int image_width = 400;
    int samples_per_pixel = 1;
    const int max_depth = 10;
    std::cout << "SPP: " << samples_per_pixel << "\n";

    //// World
    //auto world = random_scene();

    //point3 lookfrom(13, 2, 3);
    //point3 lookat(0, 0, 0);
    //vec3 vup(0, 1, 0);
    //auto dist_to_focus = 10.0;
    //auto aperture = 0.1;
    //int image_height = static_cast<int>(image_width / aspect_ratio);


    //camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);
    hittable_list world;

    point3 lookfrom;
    point3 lookat;
    auto vfov = 40.0;
    auto aperture = 0.0;

    switch (0) {
    case 1:
        world = random_scene();
        lookfrom = point3(13, 2, 3);
        lookat = point3(0, 0, 0);
        vfov = 20.0;
        aperture = 0.1;
        break;
    case 2:
        world = two_spheres();
        lookfrom = point3(13, 2, 3);
        lookat = point3(0, 0, 0);
        vfov = 20.0;
        break;
    default:
    case 3:
        world = two_perlin_spheres();
        lookfrom = point3(13, 2, 3);
        lookat = point3(0, 0, 0);
        vfov = 20.0;
        break;
    }

    // Camera

    vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    int image_height = static_cast<int>(image_width / aspect_ratio);

    camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);




    // Render
    frameBuffer m_framebuffer(image_width, image_height);

// use openmp to accelerate, this is easy but hard to print process
// print mutex will delay whole process

//#pragma omp parallel for 
//    for (int j = 0; j <image_height; ++j) {
//#pragma omp parallel for
//        for (int i = 0; i < image_width; ++i) {
//            color pixel_color(0, 0, 0);
//            for (int s = 0; s < samples_per_pixel; ++s) {
//                auto u = (i + random_double()) / (image_width - 1);
//                auto v = (j + random_double()) / (image_height - 1);
//                v = 1 - v;//reverse v
//                ray r = cam.get_ray(u, v);
//                pixel_color += ray_color(r, world,max_depth);
//            }
//            pixel_color /= samples_per_pixel;
//            m_framebuffer.setColor(i, j, pixel_color);
//        }
//    }


    int process = 0;
    auto castRayMultiThreading = [&](uint32_t rowStart, uint32_t rowEnd, uint32_t colStart, uint32_t colEnd)
    {
        for (uint32_t j = rowStart; j < rowEnd; ++j) {
            for (uint32_t i = colStart; i < colEnd; ++i) {
                // generate primary ray direction
                color pixel_color(0, 0, 0);
                for (int s = 0; s < samples_per_pixel; ++s) {
                    auto u = (i + random_double()) / (image_width - 1);
                    auto v = (j + random_double()) / (image_height - 1);
                    v = 1 - v;//reverse v
                    ray r = cam.get_ray(u, v);
                    pixel_color += ray_color(r, world, max_depth);
                }
                pixel_color /= samples_per_pixel;
                m_framebuffer.setColor(i, j, pixel_color);
                process++;
            }

            //print mutex to print process
            std::lock_guard<std::mutex> g1(print_mutex);
            UpdateProgress(1.0 * process / (image_width*image_height));
        }
    };

    int id = 0;
    constexpr int bx = 5;
    constexpr int by = 5;
    std::thread th[bx * by];

    int strideX = image_width / bx + 1;// + 1 for safety
    int strideY = image_height / by + 1;

    // divide 5*5 block
    for (int i = 0; i < image_height; i += strideY)
    {
        for (int j = 0; j < image_width; j += strideX)
        {
            th[id] = std::thread(castRayMultiThreading, i, std::min(i + strideY, image_height), j, std::min(j + strideX, image_width));
            id++;
        }
    }

    for (int i = 0; i < bx * by; i++) th[i].join();
    UpdateProgress(1.f);



    m_framebuffer.saveAsPPM("binary.ppm", 2.2);

    auto stop = std::chrono::system_clock::now();
    std::cout << "Render complete: \n";
    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::hours>(stop - startTime).count() << " hours\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::minutes>(stop - startTime).count() << " minutes\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::seconds>(stop - startTime).count() << " seconds\n";
}