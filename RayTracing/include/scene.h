#pragma once
#include "hittable.h"
#include "rtweekend.h"
#include "material.h"
class scene
{
public:
	scene(std::vector<shared_ptr<hittable>>& lights, 
		std::vector<shared_ptr<hittable>>& objects):mLights(lights),mObjects(objects){}
	void addLight(shared_ptr<hittable> light) { mLights.push_back(light); }
	void addObject(shared_ptr<hittable> object) { mObjects.push_back(object); }
	bool trace(const ray& r,hit_record& rec);
	
	color raycast(const ray& r);

public:
	std::vector<shared_ptr<hittable>> mLights;
	std::vector<shared_ptr<hittable>> mObjects;
};

bool scene::trace(const ray& r, hit_record& rec) {
	hit_record temp;
	double tmin, tmax;
	tmin = tmax = 10000;
	bool hit = false;
	for (auto& light : mLights) {
		if (light->hit(r, 0.0001, tmax, temp)) {
			hit = true;
			if (temp.t < tmin) {
				tmin = temp.t;
				rec = temp;
			}
		}
	}
	for (auto& object : mObjects) {
		if (object->hit(r, 0.0001, tmax, temp)) {
			hit = true;
			if (temp.t < tmin) {
				tmin = rec.t;
				rec = temp;
			}
		}
	}
	return hit;
}

color scene::raycast(const ray& r) {
	hit_record rec;
	ray scaterRay;
	color attenuation;
	if (!trace(r, rec)) return { 0,0,0 };
	if (!rec.mat_ptr->scatter(r, rec, attenuation, scaterRay)) {
		//not finished
		// TODO: from here
		return rec.mat_ptr->emitted(0, 0, rec.p);
	}

	return { 0,0,0 };
}