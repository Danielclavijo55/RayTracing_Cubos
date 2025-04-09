#include "rtweekend.h"
#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "box.h"
#include "sphere.h"

// Función auxiliar para verificar si un punto está cerca de objetos existentes
bool is_too_close(const point3& center, double radius, const std::vector<std::pair<point3, double>>& existing_objects, double min_distance = 1.0) {
    for (const auto& obj : existing_objects) {
        if ((center - obj.first).length() < (radius + obj.second + min_distance)) {
            return true;
        }
    }
    return false;
}

int main() {
    // AJUSTE RÁPIDO: Cambia este valor para alternar entre render rápido y final
    // 0 = rápido para pruebas, 1 = calidad media, 2 = alta calidad para render final
    const int quality_level = 1;
    
    // Configuración basada en el nivel de calidad
    int image_width, samples_per_pixel, num_random_objects;
    bool add_defocus;
    
    if (quality_level == 0) {
        // Render rápido
        image_width = 400;
        samples_per_pixel = 10;
        num_random_objects = 15;
        add_defocus = false;
    } else if (quality_level == 1) {
        // Calidad media
        image_width = 800;
        samples_per_pixel = 50;
        num_random_objects = 30;
        add_defocus = true;
    } else {
        // Alta calidad
        image_width = 1200;
        samples_per_pixel = 200;
        num_random_objects = 50;
        add_defocus = true;
    }
    
    hittable_list world;
    std::vector<std::pair<point3, double>> object_positions; // Para rastrear posiciones (centro, radio/tamaño)

    // Material para el suelo
    auto ground_material = make_shared<lambertian>(color(0.48, 0.52, 0.6));
    
    // Suelo: cubo grande y plano
    world.add(make_shared<box>(point3(-20, -0.5, -20), point3(20, 0, 20), ground_material));

    // Materiales para los cubos principales con colores mejorados
    auto material1 = make_shared<dielectric>(1.6);  // Vidrio con mayor índice de refracción
    auto material2 = make_shared<lambertian>(color(0.6, 0.3, 0.1));  // Marrón rojizo más rico
    auto material3 = make_shared<metal>(color(0.8, 0.8, 0.9), 0.0);  // Metal pulido más brillante
    
    // Los tres cubos grandes principales - ajustados para tocar el suelo
    point3 center1(0.0, 1.0, 0.0);
    point3 center2(-4.0, 1.0, -0.5);
    point3 center3(4.0, 1.0, -0.5);
    double size1 = 1.0;
    double size2 = 1.0;
    double size3 = 1.0;
    
    // CORRECCIÓN: Ajuste de la coordenada Y para que los cubos descansen sobre el suelo (Y=-0.5)
    world.add(make_shared<box>(point3(center1.x() - size1, -0.5, center1.z() - size1),
                              point3(center1.x() + size1, center1.y() + size1 - 0.5, center1.z() + size1),
                              material1));
    world.add(make_shared<box>(point3(center2.x() - size2, -0.5, center2.z() - size2),
                              point3(center2.x() + size2, center2.y() + size2 - 0.5, center2.z() + size2),
                              material2));
    world.add(make_shared<box>(point3(center3.x() - size3, -0.5, center3.z() - size3),
                              point3(center3.x() + size3, center3.y() + size3 - 0.5, center3.z() + size3),
                              material3));
    
    object_positions.push_back({center1, size1 * 1.5});
    object_positions.push_back({center2, size2 * 1.5});
    object_positions.push_back({center3, size3 * 1.5});

    // Cubos adicionales con variaciones y colores interesantes - corregidos para tocar el suelo
    // Cubo pequeño verde
    auto green_material = make_shared<lambertian>(color(0.2, 0.8, 0.2));
    point3 green_center(2.5, 0.5, 3.0);
    double green_size = 0.5;
    world.add(make_shared<box>(point3(green_center.x() - green_size, -0.5, green_center.z() - green_size),
                              point3(green_center.x() + green_size, green_center.y() + green_size - 0.5, green_center.z() + green_size),
                              green_material));
    object_positions.push_back({green_center, green_size * 1.5});
    
    // Cubo pequeño azul metálico
    auto blue_metal = make_shared<metal>(color(0.2, 0.3, 0.8), 0.2);
    point3 blue_center(-3.0, 0.4, 3.5);
    double blue_size = 0.4;
    world.add(make_shared<box>(point3(blue_center.x() - blue_size, -0.5, blue_center.z() - blue_size),
                              point3(blue_center.x() + blue_size, blue_center.y() + blue_size - 0.5, blue_center.z() + blue_size),
                              blue_metal));
    object_positions.push_back({blue_center, blue_size * 1.5});
    
    // Cubo pequeño rojo
    auto red_material = make_shared<lambertian>(color(0.9, 0.1, 0.1));
    point3 red_center(-0.5, 0.6, 3.5);
    double red_size = 0.6;
    world.add(make_shared<box>(point3(red_center.x() - red_size, -0.5, red_center.z() - red_size),
                               point3(red_center.x() + red_size, red_center.y() + red_size - 0.5, red_center.z() + red_size),
                               red_material));
    object_positions.push_back({red_center, red_size * 1.5});
    
    // Cubo pequeño morado
    auto purple_material = make_shared<lambertian>(color(0.6, 0.2, 0.8));
    point3 purple_center(4.0, 0.35, -3.0);
    double purple_size = 0.35;
    world.add(make_shared<box>(point3(purple_center.x() - purple_size, -0.5, purple_center.z() - purple_size),
                               point3(purple_center.x() + purple_size, purple_center.y() + purple_size - 0.5, purple_center.z() + purple_size),
                               purple_material));
    object_positions.push_back({purple_center, purple_size * 1.5});
    
    // Cubo vidrio pequeño
    auto small_glass = make_shared<dielectric>(1.5);
    point3 glass_center(-3.0, 0.3, -3.0);
    double glass_size = 0.3;
    world.add(make_shared<box>(point3(glass_center.x() - glass_size, -0.5, glass_center.z() - glass_size),
                               point3(glass_center.x() + glass_size, glass_center.y() + glass_size - 0.5, glass_center.z() + glass_size),
                               small_glass));
    object_positions.push_back({glass_center, glass_size * 1.5});
    
    // Añadir algunas esferas a la escena - las esferas están bien posicionadas
    auto sphere_glass = make_shared<dielectric>(1.5);
    point3 sphere1_center(0.0, 0.5, 5.0);
    double sphere1_radius = 0.5;
    world.add(make_shared<sphere>(point3(sphere1_center.x(), sphere1_radius, sphere1_center.z()),
                                 sphere1_radius, sphere_glass));
    object_positions.push_back({sphere1_center, sphere1_radius * 1.5});
    
    auto sphere_metal = make_shared<metal>(color(0.7, 0.7, 0.7), 0.05);
    point3 sphere2_center(-4.5, 0.5, -4.5);
    double sphere2_radius = 0.5;
    world.add(make_shared<sphere>(point3(sphere2_center.x(), sphere2_radius, sphere2_center.z()),
                                 sphere2_radius, sphere_metal));
    object_positions.push_back({sphere2_center, sphere2_radius * 1.5});
    
    auto sphere_gold = make_shared<metal>(color(0.8, 0.6, 0.2), 0.1);
    point3 sphere3_center(5.0, 0.3, 0.5);
    double sphere3_radius = 0.3;
    world.add(make_shared<sphere>(point3(sphere3_center.x(), sphere3_radius, sphere3_center.z()),
                                 sphere3_radius, sphere_gold));
    object_positions.push_back({sphere3_center, sphere3_radius * 1.5});
    
    auto sphere_blue = make_shared<lambertian>(color(0.1, 0.3, 0.7));
    point3 sphere4_center(1.5, 0.3, -4.0);
    double sphere4_radius = 0.3;
    world.add(make_shared<sphere>(point3(sphere4_center.x(), sphere4_radius, sphere4_center.z()),
                                 sphere4_radius, sphere_blue));
    object_positions.push_back({sphere4_center, sphere4_radius * 1.5});
    
    // Objetos aleatorios (mezcla de cubos y esferas) - corregidos para tocar el suelo
    int attempts = 0;
    int objects_added = 0;
    
    while (objects_added < num_random_objects && attempts < num_random_objects * 10) {
        attempts++;
        
        double a = random_double(-9, 9);
        double b = random_double(-9, 9);
        point3 center(a, 0, b);
        
        // Verificar distancia a objetos existentes
        if (is_too_close(center, 0.6, object_positions, 0.8)) continue;
        
        double choose_mat = random_double();
        double choose_shape = random_double();  // Para decidir entre cubo o esfera
        
        shared_ptr<material> obj_material;
        
        if (choose_mat < 0.5) {
            // Material difuso con colores más saturados
            auto albedo = color(random_double(0.5, 1), random_double(0.5, 1), random_double(0.5, 1));
            obj_material = make_shared<lambertian>(albedo);
        } else if (choose_mat < 0.8) {
            // Material metálico con diferentes niveles de difusión
            auto albedo = color(random_double(0.6, 1), random_double(0.6, 1), random_double(0.6, 1));
            auto fuzz = random_double(0, 0.3);
            obj_material = make_shared<metal>(albedo, fuzz);
        } else {
            // Material de vidrio
            obj_material = make_shared<dielectric>(1.5);
        }
        
        if (choose_shape < 0.6) {
            // Crear un cubo
            double size = random_double(0.2, 0.4);
            double height = random_double(0.4, 0.8);
            
            // CORRECCIÓN: Asegurar que los cubos toquen el suelo en Y=-0.5
            world.add(make_shared<box>(point3(a - size, -0.5, b - size),
                                      point3(a + size, height - 0.5, b + size),
                                      obj_material));
            object_positions.push_back({center, size * 1.5});
        } else {
            // Crear una esfera
            double radius = random_double(0.15, 0.35);
            
            // CORRECCIÓN: Colocar la esfera con su base en el suelo
            world.add(make_shared<sphere>(point3(a, radius, b), radius, obj_material));
            object_positions.push_back({center, radius * 1.5});
        }
        
        objects_added++;
    }

    // Configuración de la cámara para el render final
    camera cam;
    
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = image_width;
    cam.samples_per_pixel = samples_per_pixel;
    cam.max_depth = 30;
    
    // Posición de la cámara ajustada para una mejor vista
    cam.vfov = 25;
    cam.lookfrom = point3(12, 3.5, 12);
    cam.lookat = point3(0, 0.5, 0);
    cam.vup = vec3(0, 1, 0);
    
    cam.defocus_angle = add_defocus ? 0.3 : 0.0;
    cam.focus_dist = 12.0;

    cam.render(world);

    return 0;
}
