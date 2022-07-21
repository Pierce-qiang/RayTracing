#pragma once
#include "hittable.h"
#include "vec3.h"

class sphere : public hittable {
public:
    sphere():radius(1.0) {}
    sphere(point3 cen, double r, shared_ptr<material> m)
        : center(cen), radius(r), mat_ptr(m) {};

    virtual bool hit(
        const ray& r, double t_min, double t_max, hit_record& rec) const override;
    virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;

private:
    static void get_sphere_uv(const point3& p, double& u, double& v) {

        auto theta = acos(p.y());//angle from pos y axis
        auto phi = atan2(p.z(), p.x()) + pi;//phi is -pi to pi

        u = phi / (2 * pi);
        v = 1.0 - theta / pi;//invert v from bottom begin
    }

public:
    point3 center;
    double radius;
    shared_ptr<material> mat_ptr;
};