#include "scene.h"

color scene::raycast(const ray& r, int depth) const {
    hit_record rec;
    if (depth <= 0)
        return color(0, 0, 0);
    // If the ray hits nothing, return the background color.
    if (!mObjects.hit(r, 0.001, infinity, rec)&& !mLights->hit(r,0.001, infinity, rec))
        return mBackground;

    ray scattered;
    color attenuation;
    color emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);
    double pdf_val;
    color albedo;
    if (!rec.mat_ptr->scatter(r, rec, albedo, scattered, pdf_val))
        return emitted;

    auto p0 = make_shared<hittable_pdf>(mLights, rec.p);
    auto p1 = make_shared<cosine_pdf>(rec.normal);
    mixture_pdf mixed_pdf(p0, p1);

    scattered = ray(rec.p, mixed_pdf.generate(), r.time());
    pdf_val = mixed_pdf.value(scattered.direction());

    return emitted
        + albedo * rec.mat_ptr->scattering_pdf(r, rec, scattered)
        * raycast(scattered, depth-1) / pdf_val;
}