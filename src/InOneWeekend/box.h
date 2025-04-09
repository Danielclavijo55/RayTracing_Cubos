#ifndef BOX_H
#define BOX_H

#include "rtweekend.h"
#include "hittable.h"

class box : public hittable {
  public:
    box() {}
    box(const point3& p0, const point3& p1, shared_ptr<material> mat);

    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const override;

  private:
    point3 box_min;
    point3 box_max;
    shared_ptr<material> mat_ptr;
};

box::box(const point3& p0, const point3& p1, shared_ptr<material> mat) {
    box_min = p0;
    box_max = p1;
    mat_ptr = mat;
}

bool box::hit(const ray& r, interval ray_t, hit_record& rec) const {
    // Para cada dimensión (x, y, z), calculamos el tiempo de entrada y salida del rayo
    double t_min = ray_t.min;
    double t_max = ray_t.max;
    
    for (int a = 0; a < 3; a++) {
        auto invD = 1.0f / r.direction()[a];
        auto t0 = (box_min[a] - r.origin()[a]) * invD;
        auto t1 = (box_max[a] - r.origin()[a]) * invD;
        
        // Ordenar los tiempos de entrada y salida
        if (invD < 0.0f) {
            std::swap(t0, t1);
        }
        
        // Actualizar los tiempos
        t_min = t0 > t_min ? t0 : t_min;
        t_max = t1 < t_max ? t1 : t_max;
        
        if (t_max <= t_min)
            return false;
    }
    
    // Si llegamos aquí, hay una intersección
    rec.t = t_min;
    rec.p = r.at(rec.t);
    
    // Calcular la normal en el punto de intersección
    vec3 outward_normal;
    
    // Determinando qué cara fue golpeada
    const double EPSILON = 0.0001;
    
    if (std::abs(rec.p.x() - box_min.x()) < EPSILON)
        outward_normal = vec3(-1, 0, 0);
    else if (std::abs(rec.p.x() - box_max.x()) < EPSILON)
        outward_normal = vec3(1, 0, 0);
    else if (std::abs(rec.p.y() - box_min.y()) < EPSILON)
        outward_normal = vec3(0, -1, 0);
    else if (std::abs(rec.p.y() - box_max.y()) < EPSILON)
        outward_normal = vec3(0, 1, 0);
    else if (std::abs(rec.p.z() - box_min.z()) < EPSILON)
        outward_normal = vec3(0, 0, -1);
    else
        outward_normal = vec3(0, 0, 1);
    
    rec.set_face_normal(r, outward_normal);
    rec.mat = mat_ptr;
    
    return true;
}

#endif
