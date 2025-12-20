#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.h"

class hit_record;

// Abstract base class for all materials.
// The core function is scatter(), which determines how a ray behaves when it hits the material.
class material {
  public:
    virtual ~material() = default;

    // Determines if a ray is scattered, and if so, how.
    // r_in: The incoming ray.
    // rec: The hit_record of the intersection.
    // attenuation: The color attenuation of the material.
    // scattered: The scattered ray.
    virtual bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const = 0;
};

// A diffuse (matte) material.
class lambertian : public material {
  public:
    lambertian(const color& a) : albedo(a) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
    const override {
        auto scatter_direction = rec.normal + random_unit_vector();

        // Catch degenerate scatter direction
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

  private:
    color albedo;
};

// A metallic material.
class metal : public material {
  public:
    metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
    const override {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

  private:
    color albedo;
    double fuzz;
};

#endif