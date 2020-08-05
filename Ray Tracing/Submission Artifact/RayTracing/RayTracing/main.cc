
#include "constants.h"

#include "box.h"
#include "bvh.h"
#include "camera.h"
#include "color.h"
#include "constant_medium.h"
#include "hittable_list.h"
#include "material.h"
#include "moving_sphere.h"
#include "sphere.h"
#include "texture.h"

#include <iostream>
#include <fstream>

using namespace std;

colour ray_colour(
	const ray& r,
	const colour& background,
	const hittable& world,
	shared_ptr<hittable> lights,
	int depth
) {
	hit_record rec;

	// If we've exceeded the ray bounce limit, no more light is gathered.
	if (depth <= 0)
		return colour(0, 0, 0);

	// If the ray hits nothing, return the background color.
	if (!world.hit(r, 0.001, infinity, rec))
		return background;

	scatter_record srec;
	colour emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);

	if (!rec.mat_ptr->scatter(r, rec, srec))
		return emitted;

	if (srec.is_specular) {
		return srec.attenuation
			* ray_colour(srec.specular_ray, background, world, lights, depth - 1);
	}

	auto light_ptr = make_shared<hittable_pdf>(lights, rec.p);
	mixture_pdf p(light_ptr, srec.pdf_ptr);
	ray scattered = ray(rec.p, p.generate(), r.time());
	auto pdf_val = p.value(scattered.direction());

	return emitted
		+ srec.attenuation * rec.mat_ptr->scattering_pdf(r, rec, scattered)
		* ray_colour(scattered, background, world, lights, depth - 1)
		/ pdf_val;
}


//hittable_list random_scene() {
//    hittable_list world;
//
//    auto checker = make_shared<checker_texture>(
//        make_shared<solid_colour>(0.2, 0.3, 0.1),
//        make_shared<solid_colour>(0.9, 0.9, 0.9)
//    );
//
//    world.add(make_shared<sphere>(point(0,-1000,0), 1000, make_shared<lambertian>(checker)));
//
//    for (int a = -11; a < 11; a++) {
//        for (int b = -11; b < 11; b++) {
//            auto choose_mat = random_double();
//            point center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());
//
//            if ((center - vector3(4, 0.2, 0)).length() > 0.9) {
//                shared_ptr<material> sphere_material;
//
//                if (choose_mat < 0.8) {
//                    // diffuse
//                    auto albedo = colour::random() * colour::random();
//                    sphere_material = make_shared<lambertian>(make_shared<solid_colour>(albedo));
//                    auto center2 = center + vector3(0, random_double(0,.5), 0);
//                    world.add(make_shared<moving_sphere>(
//                        center, center2, 0.0, 1.0, 0.2, sphere_material));
//                } else if (choose_mat < 0.95) {
//                    // metal
//                    auto albedo = colour::random(0.5, 1);
//                    auto fuzz = random_double(0, 0.5);
//                    sphere_material = make_shared<metal>(albedo, fuzz);
//                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
//                } else {
//                    // glass
//                    sphere_material = make_shared<dielectric>(1.5);
//                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
//                }
//            }
//        }
//    }
//
//    auto material1 = make_shared<dielectric>(1.5);
//    world.add(make_shared<sphere>(point(0, 1, 0), 1.0, material1));
//
//    auto material2 = make_shared<lambertian>(make_shared<solid_colour>(colour(0.4, 0.2, 0.1)));
//    world.add(make_shared<sphere>(point(-4, 1, 0), 1.0, material2));
//
//    auto material3 = make_shared<metal>(colour(0.7, 0.6, 0.5), 0.0);
//    world.add(make_shared<sphere>(point(4, 1, 0), 1.0, material3));
//
//    return hittable_list(make_shared<bvh_node>(world, 0.0, 1.0));
//}
//
//hittable_list two_spheres() {
//    hittable_list objects;
//
//    auto checker = make_shared<checker_texture>(
//        make_shared<solid_colour>(0.2, 0.3, 0.1),
//        make_shared<solid_colour>(0.9, 0.9, 0.9)
//    );
//
//    objects.add(make_shared<sphere>(point(0,-10, 0), 10, make_shared<lambertian>(checker)));
//    objects.add(make_shared<sphere>(point(0, 10, 0), 10, make_shared<lambertian>(checker)));
//
//    return objects;
//}
//
//hittable_list two_perlin_spheres() {
//    hittable_list objects;
//
//    auto pertext = make_shared<noise_texture>(4);
//    objects.add(make_shared<sphere>(point(0,-1000,0), 1000, make_shared<lambertian>(pertext)));
//    objects.add(make_shared<sphere>(point(0,2,0), 2, make_shared<lambertian>(pertext)));
//
//    return objects;
//}
//
//hittable_list earth() {
//    auto earth_texture = make_shared<image_texture>("earthmap.jpg");
//    auto earth_surface = make_shared<lambertian>(earth_texture);
//    auto globe = make_shared<sphere>(point(0,0,0), 2, earth_surface);
//
//    return hittable_list(globe);
//}
//
//hittable_list simple_light() {
//    hittable_list objects;
//
//    auto pertext = make_shared<noise_texture>(4);
//    objects.add(make_shared<sphere>(point(0,-1000,0), 1000, make_shared<lambertian>(pertext)));
//    objects.add(make_shared<sphere>(point(0,2,0), 2, make_shared<lambertian>(pertext)));
//
//    auto difflight = make_shared<diffuse_light>(make_shared<solid_colour>(4,4,4));
//    objects.add(make_shared<sphere>(point(0,7,0), 2, difflight));
//    objects.add(make_shared<xy_rect>(3, 5, 1, 3, -2, difflight));
//
//    return objects;
//}
//
//hittable_list cornell_balls() {
//    hittable_list objects;
//
//    auto red   = make_shared<lambertian>(make_shared<solid_colour>(.65, .05, .05));
//    auto white = make_shared<lambertian>(make_shared<solid_colour>(.73, .73, .73));
//    auto green = make_shared<lambertian>(make_shared<solid_colour>(.12, .45, .15));
//    auto light = make_shared<diffuse_light>(make_shared<solid_colour>(5,5,5));
//
//    objects.add(make_shared<flip_face>(make_shared<yz_rect>(0, 555, 0, 555, 555, green)));
//    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
//    objects.add(make_shared<xz_rect>(113, 443, 127, 432, 554, light));
//    objects.add(make_shared<flip_face>(make_shared<xz_rect>(0, 555, 0, 555, 555, white)));
//    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
//    objects.add(make_shared<flip_face>(make_shared<xy_rect>(0, 555, 0, 555, 555, white)));
//
//    auto boundary = make_shared<sphere>(point(160,100,145), 100, make_shared<dielectric>(1.5));
//    objects.add(boundary);
//    objects.add(make_shared<constant_medium>(boundary, 0.1, make_shared<solid_colour>(1,1,1)));
//
//    shared_ptr<hittable> box1 = make_shared<box>(point(0,0,0), point(165,330,165), white);
//    box1 = make_shared<rotate_y>(box1, 15);
//    box1 = make_shared<translate>(box1, vector3(265,0,295));
//    objects.add(box1);
//
//    return objects;
//}
//
//hittable_list cornell_smoke() {
//    hittable_list objects;
//
//    auto red   = make_shared<lambertian>(make_shared<solid_colour>(.65, .05, .05));
//    auto white = make_shared<lambertian>(make_shared<solid_colour>(.73, .73, .73));
//    auto green = make_shared<lambertian>(make_shared<solid_colour>(.12, .45, .15));
//    auto light = make_shared<diffuse_light>(make_shared<solid_colour>(7, 7, 7));
//
//    objects.add(make_shared<flip_face>(make_shared<yz_rect>(0, 555, 0, 555, 555, green)));
//    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
//    objects.add(make_shared<xz_rect>(113, 443, 127, 432, 554, light));
//    objects.add(make_shared<flip_face>(make_shared<xz_rect>(0, 555, 0, 555, 555, white)));
//    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
//    objects.add(make_shared<flip_face>(make_shared<xy_rect>(0, 555, 0, 555, 555, white)));
//
//    shared_ptr<hittable> box1 = make_shared<box>(point(0,0,0), point(165,330,165), white);
//    box1 = make_shared<rotate_y>(box1,  15);
//    box1 = make_shared<translate>(box1, vector3(265,0,295));
//
//    shared_ptr<hittable> box2 = make_shared<box>(point(0,0,0), point(165,165,165), white);
//    box2 = make_shared<rotate_y>(box2, -18);
//    box2 = make_shared<translate>(box2, vector3(130,0,65));
//
//    objects.add(make_shared<constant_medium>(box1, 0.01, make_shared<solid_colour>(0,0,0)));
//    objects.add(make_shared<constant_medium>(box2, 0.01, make_shared<solid_colour>(1,1,1)));
//
//    return objects;
//}
//
//hittable_list cornell_final() {
//    hittable_list objects;
//
//    auto pertext = make_shared<noise_texture>(0.1);
//
//    auto mat = make_shared<lambertian>(make_shared<image_texture>("earthmap.jpg"));
//
//    auto red   = make_shared<lambertian>(make_shared<solid_colour>(.65, .05, .05));
//    auto white = make_shared<lambertian>(make_shared<solid_colour>(.73, .73, .73));
//    auto green = make_shared<lambertian>(make_shared<solid_colour>(.12, .45, .15));
//    auto light = make_shared<diffuse_light>(make_shared<solid_colour>(7, 7, 7));
//
//    objects.add(make_shared<flip_face>(make_shared<yz_rect>(0, 555, 0, 555, 555, green)));
//    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
//    objects.add(make_shared<xz_rect>(123, 423, 147, 412, 554, light));
//    objects.add(make_shared<flip_face>(make_shared<xz_rect>(0, 555, 0, 555, 555, white)));
//    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
//    objects.add(make_shared<flip_face>(make_shared<xy_rect>(0, 555, 0, 555, 555, white)));
//
//    shared_ptr<hittable> boundary2 =
//        make_shared<box>(point(0,0,0), point(165,165,165), make_shared<dielectric>(1.5));
//    boundary2 = make_shared<rotate_y>(boundary2, -18);
//    boundary2 = make_shared<translate>(boundary2, vector3(130,0,65));
//
//    auto tex = make_shared<solid_colour>(0.9, 0.9, 0.9);
//
//    objects.add(boundary2);
//    objects.add(make_shared<constant_medium>(boundary2, 0.2, tex));
//
//    return objects;
//}
//
//hittable_list final_scene() {
//    hittable_list boxes1;
//    auto ground = make_shared<lambertian>(make_shared<solid_colour>(0.48, 0.83, 0.53));
//
//    const int boxes_per_side = 20;
//    for (int i = 0; i < boxes_per_side; i++) {
//        for (int j = 0; j < boxes_per_side; j++) {
//            auto w = 100.0;
//            auto x0 = -1000.0 + i*w;
//            auto z0 = -1000.0 + j*w;
//            auto y0 = 0.0;
//            auto x1 = x0 + w;
//            auto y1 = random_double(1,101);
//            auto z1 = z0 + w;
//
//            boxes1.add(make_shared<box>(point(x0,y0,z0), point(x1,y1,z1), ground));
//        }
//    }
//
//    hittable_list objects;
//
//    objects.add(make_shared<bvh_node>(boxes1, 0, 1));
//
//    auto light = make_shared<diffuse_light>(make_shared<solid_colour>(7, 7, 7));
//    objects.add(make_shared<xz_rect>(123, 423, 147, 412, 554, light));
//
//    auto center1 = point(400, 400, 200);
//    auto center2 = center1 + vector3(30,0,0);
//    auto moving_sphere_material =
//        make_shared<lambertian>(make_shared<solid_colour>(0.7, 0.3, 0.1));
//    objects.add(make_shared<moving_sphere>(center1, center2, 0, 1, 50, moving_sphere_material));
//
//    objects.add(make_shared<sphere>(point(260, 150, 45), 50, make_shared<dielectric>(1.5)));
//    objects.add(make_shared<sphere>(point(0, 150, 145), 50, make_shared<metal>(colour(0.8, 0.8, 0.9), 10.0)
//    ));
//
//    auto boundary = make_shared<sphere>(point(360,150,145), 70, make_shared<dielectric>(1.5));
//    objects.add(boundary);
//    objects.add(make_shared<constant_medium>(
//        boundary, 0.2, make_shared<solid_colour>(0.2, 0.4, 0.9)
//    ));
//    boundary = make_shared<sphere>(point(0,0,0), 5000, make_shared<dielectric>(1.5));
//    objects.add(make_shared<constant_medium>(boundary, .0001, make_shared<solid_colour>(1,1,1)));
//
//    auto emat = make_shared<lambertian>(make_shared<image_texture>("earthmap.jpg"));
//    objects.add(make_shared<sphere>(point(400,200,400), 100, emat));
//
//    auto pertext = make_shared<noise_texture>(4);
//    objects.add(make_shared<sphere>(point(220,280,300), 80, make_shared<lambertian>(pertext)));
//
//    hittable_list boxes2;
//    auto white = make_shared<lambertian>(make_shared<solid_colour>(.73, .73, .73));
//    int ns = 1000;
//    for (int j = 0; j < ns; j++) {
//        boxes2.add(make_shared<sphere>(point::random(0,165), 10, white));
//    }
//
//    objects.add(make_shared<translate>(
//        make_shared<rotate_y>(
//            make_shared<bvh_node>(boxes2, 0.0, 1.0), 15),
//            vector3(-100,270,395)
//        )
//    );
//
//    return objects;
//}


hittable_list cornell_box(camera& cam, double aspect) 
{
	hittable_list world;

	auto red = make_shared<lambertian>(make_shared<solid_colour>(.65, .05, .05));
	auto white = make_shared<lambertian>(make_shared<solid_colour>(.73, .73, .73));
	auto blue = make_shared<lambertian>(make_shared<solid_colour>(.12, .85, .85));
	auto light = make_shared<diffuse_light>(make_shared<solid_colour>(15, 15, 15));
	auto blue_light = make_shared<diffuse_light>(make_shared<solid_colour>(0.2, 4, 4));
	auto red_light = make_shared<diffuse_light>(make_shared<solid_colour>(4, 0.2, 0.2));

	shared_ptr<hittable> smokesphere = make_shared<sphere>(point(148, 140, 140), 60, white);
	world.add(make_shared<constant_medium>(smokesphere, 0.01, make_shared<solid_colour>(0,0,0)));

	auto checker = make_shared<checker_texture>(
		        make_shared<solid_colour>(0.1, 0.1, 0.1),
		        make_shared<solid_colour>(0.9, 0.9, 0.9)
		    );
		
	world.add(make_shared<sphere>(point(408, 420,400), 60, make_shared<lambertian>(checker)));
	world.add(make_shared<sphere>(point(408, 140, 140), 60, red));

	world.add(make_shared<sphere>(point(148, 270, 270), 60, make_shared<metal>(colour(0.8, 0.8, 0.9), 1)));
	world.add(make_shared<sphere>(point(408, 270, 270), 60, make_shared<metal>(colour(0.8, 0.8, 0.9), 0.3)));

	auto pertext = make_shared<noise_texture>(0.25);
	world.add(make_shared<sphere>(point(278, 420,400), 60, make_shared<lambertian>(pertext)));

	auto emat = make_shared<lambertian>(make_shared<image_texture>("earthmap.jpg"));
	world.add(make_shared<sphere>(point(278,270,270), 60, emat));

	auto glass = make_shared<dielectric>(1.5);
	world.add(make_shared<sphere>(point(278, 140, 140), 60, glass));

	    auto boundary = make_shared<sphere>(point(148,420,400), 60, make_shared<dielectric>(1.5));
    world.add(boundary);
	world.add(make_shared<constant_medium>(
		boundary, 0.2, make_shared<solid_colour>(0.2, 0.4, 0.9)));

	world.add(make_shared<flip_face>(make_shared<xz_rect>(213, 343, 227, 332, 554, light)));

	world.add(make_shared<flip_face>(make_shared<yz_rect>(0, 555, 0, 555, 555, blue)));
	world.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
	world.add(make_shared<flip_face>(make_shared<xz_rect>(0, 555, 0, 555, 555, white)));
	world.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
	world.add(make_shared<flip_face>(make_shared<xy_rect>(0, 555, 0, 555, 555, white)));

	//Central Column + Glass Ball

	/*shared_ptr<hittable> box1 = make_shared<box>(point(0, 0, 0), point(30, 100, 30), white);
	box1 = make_shared<rotate_y>(box1, 45);
	box1 = make_shared<translate>(box1, vector3(263, 0, 263));
	world.add(box1);*/

	//Red Doorway
	world.add(make_shared<yz_rect>(0, 356, 200, 356, 1, red_light));

	shared_ptr<hittable> box2 = make_shared<box>(point(0, 0, 0), point(15, 356, 15), white);
	box2 = make_shared<rotate_y>(box2, 0);
	box2 = make_shared<translate>(box2, vector3(0, 0, 185));
	world.add(box2);
	shared_ptr<hittable> box3 = make_shared<box>(point(0, 0, 0), point(15, 356, 15), white);
	box3 = make_shared<rotate_y>(box3, 0);
	box3 = make_shared<translate>(box3, vector3(0, 0, 356));
	world.add(box3);
	shared_ptr<hittable> box4 = make_shared<box>(point(0, 0, 0), point(15, 15, 186), white);
	box4 = make_shared<rotate_y>(box4, 0);
	box4 = make_shared<translate>(box4, vector3(0, 356, 185));
	world.add(box4);

	//Blue Doorway
	world.add(make_shared<flip_face>(make_shared<yz_rect>(0, 356, 200, 356, 554, blue_light)));

	shared_ptr<hittable> box5 = make_shared<box>(point(0, 0, 0), point(15, 356, 15), white);
	box5 = make_shared<rotate_y>(box5, 0);
	box5 = make_shared<translate>(box5, vector3(540, 0, 185));
	world.add(box5);
	shared_ptr<hittable> box6 = make_shared<box>(point(0, 0, 0), point(15, 356, 15), white);
	box6 = make_shared<rotate_y>(box6, 0);
	box6 = make_shared<translate>(box6, vector3(540, 0, 356));
	world.add(box6);
	shared_ptr<hittable> box7 = make_shared<box>(point(0, 0, 0), point(15, 15, 186), white);
	box7 = make_shared<rotate_y>(box7, 0);
	box7 = make_shared<translate>(box7, vector3(540, 356, 185));
	world.add(box7);


	shared_ptr<hittable> box8 = make_shared<box>(point(0, 0, 0), point(10, 10, 600), white);
	box8 = make_shared<rotate_y>(box8, 0);
	box8 = make_shared<translate>(box8, vector3(545, 545, -50));
	world.add(box8);
	shared_ptr<hittable> box9 = make_shared<box>(point(0, 0, 0), point(10, 10, 600), white);
	box9 = make_shared<rotate_y>(box9, 0);
	box9 = make_shared<translate>(box9, vector3(0, 545, -50));
	world.add(box9);
	shared_ptr<hittable> box10 = make_shared<box>(point(0, 0, 0), point(555, 10, 10), white);
	box10 = make_shared<rotate_y>(box10, 0);
	box10 = make_shared<translate>(box10, vector3(0, 545, 545));
	world.add(box10);

	//Camera Settings

	point lookfrom(278, 278, -500);
	point lookat(278, 278, 0);
	vector3 up(0, 1, 0);
	auto dist_to_focus = 10.0;
	auto aperture = 0.0;
	auto vfov = 40.0;
	auto t0 = 0.0;
	auto t1 = 1.0;

	cam = camera(lookfrom, lookat, up, vfov, aspect, aperture, dist_to_focus, t0, t1);

	return world;
}


int main() {
    const auto aspect_ratio = 1.0 / 1.0;
    const int image_width = 500;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
	int samples_per_pixel = 2000;
	int max_depth = 50;

	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	colour background(0, 0, 0);

	camera cam;
	auto world = cornell_box(cam, aspect_ratio);

	ofstream img("picture.ppm");
	img << "P3" << endl;
	img << image_width << " " << image_height << endl;
	img << "255" << endl;

	auto lights = make_shared<hittable_list>();
	lights->add(make_shared<xz_rect>(213, 343, 227, 332, 554, shared_ptr<material>()));
	lights->add(make_shared<sphere>(point(190, 90, 190), 90, shared_ptr<material>()));

	for (int j = image_height-1; j >= 0; --j) 
	{
		std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
		for (int i = 0; i < image_width; ++i) 
		{
			colour pixel_color(0, 0, 0);
				for (int s=0; s < samples_per_pixel; ++s) 
				{
					auto u = (i + random_double()) / image_width;
					auto v = (j + random_double()) / image_height;
					ray r = cam.get_ray(u, v);
					pixel_color += ray_colour(r, background, world, lights, max_depth);
				}
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }



    std::cerr << "\nDone.\n";
}
