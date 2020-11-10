#include <iostream>
#include <string>
#include "punto_y_vector.cc"
#include <time.h>

using namespace std;

class Ray{
    Punto_Vector origen;
    Punto_Vector direccion;

    public:
        Ray (Punto_Vector _o, Punto_Vector _d) {
            origen = _o;
            direccion = _d;
        }
};

class Camera{
    Punto_Vector origen, u, l, f;

    Camera (Punto_Vector _c, Punto_Vector _u, Punto_Vector _l, Punto_Vector _f) {
        origen = _c;
        u = _u.normalizar();
        l = _l.normalizar();
        f = _f;
        srand(time(NULL));
    }

    Ray get_center_ray(const int x, const int y, const int resolucion) {
        double dim_pixel = 2/resolucion;
        double new_x = -1 + dim_pixel / 2 + x * dim_pixel;
        double new_y = 1 - dim_pixel / 2 + y * dim_pixel;
        return Ray(origen, Punto_Vector(new_x,new_y,1,0));
    }

    Ray get_random_ray(const int x, const int y, const int resolucion) {
        double dim_pixel = 2/resolucion;
        // numeros aleatorios entre 0 y dim_pixel
        double aleatorio1 = (double)rand() / dim_pixel;
        double aleatorio2 = (double)rand() / dim_pixel;
        double new_x = -1 + aleatorio1 + x * dim_pixel;
        double new_y = 1 - aleatorio2 + y * dim_pixel;
        return Ray(origen, Punto_Vector(new_x,new_y,1,0));
    }

};


int main(int argc, char **argv) {
    int number_of_rays = 1;
    if (argc > 0) {
        number_of_rays = stoi(argv[0]);
    }
    return 0;
}