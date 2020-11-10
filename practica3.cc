#include <iostream>
#include <string>
#include <fstream>
#include <time.h>
#include <cmath>
#include <stdio.h>
#include <stdlib.h> 
#include <vector>
#include "punto_y_vector.cc"

using namespace std;

class Ray{
    public:
    Punto_Vector origen;
    Punto_Vector direccion;

    
        Ray (Punto_Vector _o, Punto_Vector _d) {
            origen = _o;
            direccion = _d;
        }
};

class Camera{
    Punto_Vector origen, u, l, f;

    public:

        Camera (Punto_Vector _c, Punto_Vector _u, Punto_Vector _l, Punto_Vector _f) {
            origen = _c;
            u = _u.normalizar();
            l = _l.normalizar();
            f = _f;
            srand(time(NULL));
        }

        Ray get_center_ray(const int x, const int y, const int resolucion) {
            double dim_pixel = 2.0/(double)resolucion;
            double new_x = -1 + dim_pixel / 2 + x * dim_pixel;
            double new_y = 1 - dim_pixel / 2 - y * dim_pixel;
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

    const int resolution = 32;

    vector<float> imagen (resolution * resolution * 3);

    ofstream out;
    out.open("practica_3.ppm");
    out << "P3" << endl;
    out << resolution << " " << resolution << endl;
    out << 255 << endl;

    // definir una esfera justo delante de la cámara a distancia 3
    Punto_Vector centro_esfera = Punto_Vector(3,5,10,1);
    double radio_esfera = 3;

    Camera cam = Camera(Punto_Vector(0,0,0,1),
                        Punto_Vector(0,1,0,0),
                        Punto_Vector(1,0,0,0),
                        Punto_Vector(0,0,1,0));

    for (int i = resolution-1; i >= 0; i--){
        for (int j = 0; j < resolution; j++){
            // para cada pixel
            for(int k = 0; k < number_of_rays; k++) {
                Ray r = cam.get_center_ray(i,j,resolution);

                Punto_Vector origen_rayo = r.origen;
                Punto_Vector dir_rayo = r.direccion;
                double a = pow(dir_rayo.modulo(), 2);
                double b = dir_rayo ^ (centro_esfera - origen_rayo) * 2;
                double c = pow((centro_esfera - origen_rayo).modulo(), 2) - pow(radio_esfera, 2);
                //cout << "VALORes: " << a << " " << b<< " "  << c << endl;
                if ((b*b - 4*a*c) < 0) {
                    // si es negativo no ha dado a nada, escribimos negro
                    out << "0 0 0 ";
                }
                else if((b*b - 4*a*c) == 0) {
                    // si es tangente (muy improbable)
                    out << "0 0 255 ";
                    cout << "TANGENTE" << endl;
                }
                else {
                    // ha dado a la esfera
                    out << "255 0 0 ";
                }
            }
        }
        out << endl;
    }
    return 0;
}