#include <iostream>
#include <string>
#include <fstream>
#include <time.h>
#include <cmath>
#include <stdio.h>
#include <stdlib.h> 
#include <vector>
#include <chrono>
#include <thread>
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

void rellenar_imagen_esfera(vector<float> &imagen, const int resolution, Punto_Vector centro_esfera, double radio_esfera, Camera cam, int thread){
    int j1, j2;
    j1 = (thread - 1) * (resolution / 8);
    j2 = thread * (resolution / 8);

    if (thread < 1 || thread > 8) {
        std::cout << "Error en los threads" << endl;
        exit(1);
    }

    for (int i = resolution-1; i >= 0; i--){
        for (int j = j1; j < j2; j++){
            // para cada pixel
            Ray r = cam.get_center_ray(i,j,resolution);

            Punto_Vector origen_rayo = r.origen;
            Punto_Vector dir_rayo = r.direccion;
            
            // Esfera
            double a = pow(dir_rayo.modulo(), 2);
            double b = dir_rayo ^ (centro_esfera - origen_rayo) * 2;
            double c = pow((centro_esfera - origen_rayo).modulo(), 2) - pow(radio_esfera, 2);
            //cout << "VALORes: " << a << " " << b<< " "  << c << endl;
            if ((b*b - 4*a*c) < 0) {
                // si es negativo no ha dado a nada, escribimos negro
                imagen[i*resolution*3 + j*3] = 0;
                imagen[i*resolution*3 + j*3 + 1] = 0;
                imagen[i*resolution*3 + j*3 + 2] = 0;
            }
            else if((b*b - 4*a*c) == 0) {
                // si es tangente (muy improbable)
                imagen[i*resolution*3 + j*3] = 0;
                imagen[i*resolution*3 + j*3 + 1] = 255;
                imagen[i*resolution*3 + j*3 + 2] = 0;
                cout << "TANGENTE" << endl;
            }
            else {
                // ha dado a la esfera
                imagen[i*resolution*3 + j*3] = 255;
                imagen[i*resolution*3 + j*3 + 1] = 255;
                imagen[i*resolution*3 + j*3 + 2] = 0;
            }
            
        }
    }
}


int main(int argc, char **argv) {
    int number_of_rays = 1;

    const int resolution = 2048;

    vector<float> imagen (resolution * resolution * 3);

    ofstream out;
    out.open("practica_3.ppm");
    out << "P3" << endl;
    out << resolution << " " << resolution << endl;
    out << 255 << endl;

    // definir una esfera justo delante de la cámara a distancia 3
    Punto_Vector centro_esfera = Punto_Vector(0,0,15,1);
    double radio_esfera = 5;

    // definir un plano limitado a distancia 20 de la cámaro por delante
    double distancia_origen_limit = 20;
    double plano_x = 0.0;
    double plano_y = 0.0;
    double plano_z = -1.0;
    Punto_Vector normal_plano_limit = Punto_Vector(plano_x,plano_y,plano_z,0);

    

    Camera cam = Camera(Punto_Vector(0,0,0,1),
                        Punto_Vector(0,1,0,0),
                        Punto_Vector(1,0,0,0),
                        Punto_Vector(0,0,1,0));

    chrono::steady_clock::time_point begin = chrono::steady_clock::now();

    /*for (int i = resolution-1; i >= 0; i--){
        for (int j = 0; j < resolution; j++){
            // para cada pixel
            for(int k = 0; k < number_of_rays; k++) {
                Ray r = cam.get_center_ray(i,j,resolution);

                Punto_Vector origen_rayo = r.origen;
                Punto_Vector dir_rayo = r.direccion;
                
                // Esfera
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
                    out << "255 255 0 ";
                }

                // Plano 
                double paralelo = dir_rayo ^ normal_plano_limit;
                //cout << paralelo << endl;
                if(paralelo == 0){
                    // El plano es paralelo, mostramos negro
                    out << "0 0 0 ";
                }
                else{
                    //cout << (-(distancia_origen+(origen_rayo^normal_plano))/paralelo) << endl;
                    double t = -(distancia_origen_limit+(origen_rayo^normal_plano_limit))/paralelo;
                    if (t < 0){
                        // El plano está detrás de la cámara, mostramos negro
                        out << "0 0 0 ";
                    }
                    else{
                        // Ha dado al plano, mostramos amarillo
                        double punto_x = origen_rayo.x + t*dir_rayo.x;
                        double punto_y = origen_rayo.y + t*dir_rayo.y;
                        double punto_z = origen_rayo.z + t*dir_rayo.z;
                        if((plano_x*punto_x+plano_y*punto_y+plano_z*punto_z + distancia_origen_limit) == 0){
                            out << "255 255 0 ";
                        }
                        else{
                            out << "0 0 0 ";
                        }
                    }
                }

            }
        }
        out << endl;
    }*/

    // generar threads
    std::thread threads[8];
    for (int t = 1; t <= 8; t++) {
        threads[t-1] = std::thread(rellenar_imagen_esfera, std::ref(imagen), resolution, centro_esfera, radio_esfera, cam, t);
    }
    for (int t = 1; t <= 8; t++) {
        threads[t-1].join();
    }

    // volcarlo al fichero
    for (int i = 0; i < resolution; i++) {
        for (int j = 0; j < resolution; j++) {
            for (int k = 0 ; k < 3 ; k++){
                out << (int)(imagen[i*resolution*3 + j*3 + k] * 255 / 255) << " " ;
            }
        }
        out << endl;
    }

    chrono::steady_clock::time_point end = chrono::steady_clock::now();

    cout << "Tiempo: " << (chrono::duration_cast<std::chrono::microseconds>(end - begin).count())/1000000.0 << " segundos" << endl;
    return 0;
}