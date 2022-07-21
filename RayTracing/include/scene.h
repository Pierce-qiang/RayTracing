#pragma once
#include "rtweekend.h"
#include "material.h"
#include "camera.h"
#include "pdf.h"
class scene
{
public:
	scene(shared_ptr<camera> cam):mCam(cam), mBackground(color(0,0,0)){}
	scene(shared_ptr<hittable> lights, hittable_list& objects, shared_ptr<camera> cam):mLights(lights),mObjects(objects), mCam(cam){}

	void addObject(shared_ptr<hittable> object) { mObjects.add(object); }

	shared_ptr<camera> GetCamera() { return mCam; }

	color raycast(const ray& r, int depth) const;
	void setBackground(const color& c) { mBackground = c; }

public:
	shared_ptr<hittable> mLights;
	hittable_list mObjects;
	shared_ptr<camera> mCam;
	color mBackground;
	//float RussianRoulette = 0.8;
};