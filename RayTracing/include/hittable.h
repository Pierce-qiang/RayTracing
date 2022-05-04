#pragma once
#include "ray.h"
#include "aabb.h"

class material;
struct hit_record {
    point3 p;
    vec3 normal;
    shared_ptr<material> mat_ptr;
    double t;
    double u;
    double v;
    bool front_face;

    inline void set_face_normal(const ray& r, const vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

// equivalent to object
class hittable {
public:
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
    virtual bool bounding_box(double time0, double time1, aabb& output_box) const = 0;
};

// translation
// 好主意，和变换光照到像素空间一样，不用改物体本身顶点数据（这太多了
// 但是遇到复杂情景要修改太多了，不易维护，还是老老实实mvp矩阵
class translate : public hittable {
public:
    translate(shared_ptr<hittable> p, const vec3& displacement)
        : ptr(p), offset(displacement) {}

    virtual bool hit(
        const ray& r, double t_min, double t_max, hit_record& rec) const override;

    virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;

public:
    shared_ptr<hittable> ptr;
    vec3 offset;
};

bool translate::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    ray moved_r(r.origin() - offset, r.direction(), r.time());
    if (!ptr->hit(moved_r, t_min, t_max, rec))
        return false;
    //注意相应变化
    rec.p += offset;
    rec.set_face_normal(moved_r, rec.normal);

    return true;
}

bool translate::bounding_box(double time0, double time1, aabb& output_box) const {
    if (!ptr->bounding_box(time0, time1, output_box))
        return false;

    output_box = aabb(
        output_box.min() + offset,
        output_box.max() + offset);

    return true;
}

// rotation
class rotate_y : public hittable {
public:
    rotate_y(shared_ptr<hittable> p, double angle);

    virtual bool hit(
        const ray& r, double t_min, double t_max, hit_record& rec) const override;

    virtual bool bounding_box(double time0, double time1, aabb& output_box) const override {
        output_box = bbox;
        return hasbox;
    }

public:
    shared_ptr<hittable> ptr;
    double sin_theta;
    double cos_theta;
    bool hasbox;
    aabb bbox;
};


rotate_y::rotate_y(shared_ptr<hittable> p, double angle) : ptr(p) {
    auto radians = degrees_to_radians(angle);
    sin_theta = sin(radians);
    cos_theta = cos(radians);
    hasbox = ptr->bounding_box(0, 1, bbox);//传入了hittable 的bbox

    point3 min(infinity, infinity, infinity);
    point3 max(-infinity, -infinity, -infinity);

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                //遍历box八个点
                auto x = i * bbox.max().x() + (1 - i) * bbox.min().x();
                auto y = j * bbox.max().y() + (1 - j) * bbox.min().y();
                auto z = k * bbox.max().z() + (1 - k) * bbox.min().z();

                // 绕y轴右手旋转
                auto newx = cos_theta * x + sin_theta * z;
                auto newz = -sin_theta * x + cos_theta * z;

                vec3 tester(newx, y, newz);

                //旋转后的包围盒还是轴对齐选择极值的xyz
                for (int c = 0; c < 3; c++) {
                    min[c] = fmin(min[c], tester[c]);
                    max[c] = fmax(max[c], tester[c]);
                }
            }
        }
    }

    bbox = aabb(min, max);
}
bool rotate_y::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    auto origin = r.origin();
    auto direction = r.direction();

    //为什么和包围盒旋转方向不一致？因为对于同样的光线，射入旋转后的物体等于反向旋转入射光线到未旋转的物体
    origin[0] = cos_theta * r.origin()[0] - sin_theta * r.origin()[2];
    origin[2] = sin_theta * r.origin()[0] + cos_theta * r.origin()[2];

    direction[0] = cos_theta * r.direction()[0] - sin_theta * r.direction()[2];
    direction[2] = sin_theta * r.direction()[0] + cos_theta * r.direction()[2];

    ray rotated_r(origin, direction, r.time());

    if (!ptr->hit(rotated_r, t_min, t_max, rec))
        return false;

    auto p = rec.p;
    auto normal = rec.normal;
    // 对于位置旋转过去
    p[0] = cos_theta * rec.p[0] + sin_theta * rec.p[2];
    p[2] = -sin_theta * rec.p[0] + cos_theta * rec.p[2];
    // 注意这里是M^(-1)T 而旋转 的逆等于转置 得 M 
    normal[0] = cos_theta * rec.normal[0] + sin_theta * rec.normal[2];
    normal[2] = -sin_theta * rec.normal[0] + cos_theta * rec.normal[2];

    rec.p = p;
    rec.set_face_normal(rotated_r, normal);

    return true;
}