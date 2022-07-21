#include "renderer.h"


void Renderer::render(const scene& scene, shared_ptr<frameBuffer> m_frameBuffer, int samples_per_pixel, int max_depth) {
    const int image_width = m_frameBuffer->getWidth();
    const int image_height = m_frameBuffer->getHeight();
    auto cam = scene.mCam;
#pragma region openmp_donot_use
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
    //                pixel_color += ray_color(r, background, world, max_depth);
    //            }
    //            pixel_color /= samples_per_pixel;
    //            m_framebuffer.setColor(i, j, pixel_color);
    //        }
    //    }
#pragma endregion

#pragma region multithreadtask
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
                    ray r = cam->get_ray(u, v);
                    pixel_color += scene.raycast(r, max_depth);
                }
                pixel_color /= samples_per_pixel;
                // avoid NAN
                if (pixel_color[0] != pixel_color[0]) pixel_color[0] = 0;
                if (pixel_color[1] != pixel_color[1]) pixel_color[1] = 0;
                if (pixel_color[2] != pixel_color[2]) pixel_color[2] = 0;
                m_frameBuffer->setColor(i, j, pixel_color);
                process++;
            }

            //print mutex to print process
            std::lock_guard<std::mutex> g1(print_mutex);
            UpdateProgress(1.0 * process / (image_width * image_height));
        }
    };
#pragma endregion

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

    m_frameBuffer->saveAsPPM("binary.ppm", 2.2);
}