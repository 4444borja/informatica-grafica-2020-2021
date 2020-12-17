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
#include <tuple>
#include "punto_y_vector.cc"
#include "geometria.cc"
#include "Matrix.cc"

#define PI 3.14159265358979

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

std::tuple<int,int,int> funcionL(esfera escena[], Ray r){
    Punto_Vector origen_rayo = r.origen;
    Punto_Vector dir_rayo = r.direccion;
    rgb colores_figura;
    // Esfera
    int i_figura = 0;
    double t_valor_min = numeric_limits<double>::max();

    for(int i = 0; i < 3 ; i++){
        double t_valor = escena[i].get_interseccion(origen_rayo,dir_rayo);
        if((t_valor < t_valor_min) && t_valor >= 0){
            t_valor_min = t_valor;
            colores_figura = escena[i].get_colores();
            i_figura = i;
        }
    }
    if(t_valor_min != numeric_limits<double>::max()){
        if (i_figura == 2) {
            // ha intersectado con la tercera figura, que consideramos luz
            //cout << "interseccion con luz" << endl;
            return std::make_tuple(255, 255, 255);
        }
        else {
            // NO HA INTERSECTADO CON LUZ, PERO SI CON OTRO OBJETO

            // Punto en coordenadas globales
            Punto_Vector punto_figura = r.origen + t_valor_min*dir_rayo;
            Punto_Vector centro_figura = escena[i_figura].get_centro();

            // Obtenemos la normal y creamos coordenadas locales en base a esa normal
            Punto_Vector vector_y = punto_figura - centro_figura; 

            double vectorIncl_1[4] = {cos(90),0,-sin(90),0};
            double vectorIncl_2[4] = {0,1,0,0};
            double vectorIncl_3[4] = {sin(90),0,cos(90),0};
            double vectorIncl_4[4] = {0,0,0,1};
            Matrix matrixIncl(vectorIncl_1,vectorIncl_2,vectorIncl_3,vectorIncl_4);

            Punto_Vector vector_z = vector_y*matrixIncl;

            Punto_Vector vector_x = operatorx(vector_y,vector_z);

            // Preguntar si el sistema de coordenadas tiene un origen de 0,0,0 o el punto de origen de la figura
            Matrix matrizLocal(vector_x,vector_y,vector_z,punto_figura);

            // PARA MATERIAL DIFUSO
            ////////////////////////
            float random1 = (float) rand()/RAND_MAX;
            float random2 = (float) rand()/RAND_MAX;

            double zeta = acos(sqrt(1 - (double) random1));
            double fi = 2 * PI * (double) random2;

            Punto_Vector direc_local = Punto_Vector(sin(zeta) * cos(fi),
                                                        sin(zeta) * sin(fi),
                                                        cos(zeta),
                                                        0);
            // Transformamos de coordenadas locales a globales, con la inversa y con eso tenemos el punto y direccion
            Matrix inversaBaseM = InverseOfMatrix(matrizLocal,4);
            Punto_Vector direc_global = inversaBaseM*direc_local;

            double valor_w_normal = abs(vector_y^direc_global);
            r.origen = punto_figura;
            r.direccion = direc_global;
            ////////////////////////////////////////
            ////////////////////////////////////////

            // color de la esfera con la que se ha chocado
            rgb color = escena[i_figura].get_colores();
            //cout << "colores vvv: " << color.get_red() << " " << color.get_green() << " " <<  color.get_blue() << endl;

            // ruleta rusa para ver si seguimos
            if( rand() % 10 < 8){
                // el rayo sigue
                std::tuple<int, int, int> siguiente = funcionL(escena,r);

                // pasamos los colores a [0, 1] para multiplicarlos
                float red = (get<0>(siguiente) / 255.0) * (color.get_red() / 255.0);
                float green = (get<1>(siguiente) / 255.0) * (color.get_green() / 255.0);
                float blue = (get<2>(siguiente) / 255.0) * (color.get_blue() / 255.0);

                if(red > 1 || red < 0 || green > 1 || green < 0 || blue > 1 || blue < 0){
                    cout << "ERROR:"<< endl;
                    cout << get<0>(siguiente) << endl;
                    cout << color.get_red() << endl;
                    cout << (get<0>(siguiente) / 255.0) << endl;
                    cout << (color.get_red() / 255.0) << endl;
                    cout << red << endl;
                }

                return std::make_tuple(red*255, green*255, blue*255);
            }
            else {
                //ruleta rusa dice que paramos, devolvemos negro
                return std::make_tuple(0, 0, 0);
            }
        }
    }
    return std::make_tuple(0, 0, 0);
}



void rellenar_imagen_esfera(vector<float> &imagen, const int resolution, esfera escena[], Camera cam, int thread, int number_of_rays){
    int j1, j2;
    j1 = (thread - 1) * (resolution / 8);
    j2 = thread * (resolution / 8);

    if (thread < 1 || thread > 8) {
        std::cout << "Error en los threads" << endl;
        exit(1);
    }
    for (int i = resolution-1; i >= 0; i--){
        for (int j = j1; j < j2; j++){
    /*for (int i = resolution-1; i >= 0; i--){
        for (int j = 0; j < resolution; j++){*/
            // para cada pixel
            
            Ray r = cam.get_center_ray(i,j,resolution);
            double radiancia_total = 0;

            std::tuple<int, int, int> totalPixel = std::make_tuple(0, 0, 0);

            for (int i = 0; i < number_of_rays; i++){
                std::tuple<int, int, int> colorPixel = funcionL(escena,r);

                get<0>(totalPixel) = get<0>(totalPixel) + get<0>(colorPixel);
                get<1>(totalPixel) = get<1>(totalPixel) + get<1>(colorPixel);
                get<2>(totalPixel) = get<2>(totalPixel) + get<2>(colorPixel);
            }


            get<0>(totalPixel) = get<0>(totalPixel) / number_of_rays;
            get<1>(totalPixel) = get<1>(totalPixel) / number_of_rays;
            get<2>(totalPixel) = get<2>(totalPixel) / number_of_rays;
            
            //cout << "colores: " << get<0>(totalPixel) << " " << get<1>(totalPixel) << " " <<  get<2>(totalPixel) << endl;

            imagen[i*resolution*3 + j*3] = get<0>(totalPixel);
            imagen[i*resolution*3 + j*3 + 1] = get<1>(totalPixel);
            imagen[i*resolution*3 + j*3 + 2] = get<2>(totalPixel);
        }
    }
}


int main(int argc, char **argv) {
    int number_of_rays = 10;

    const int resolution = 512;

    vector<float> imagen (resolution * resolution * 3);

    ofstream out;
    out.open("practica_4.ppm");
    out << "P3" << endl;
    out << resolution << " " << resolution << endl;
    out << 255 << endl;
    

    esfera vector [3];
    // definir una esfera justo delante de la cámara a distancia 3
    Punto_Vector centro_esfera = Punto_Vector(0,0,15,1);
    double radio_esfera = 5;
    esfera la_esfera;
    la_esfera.set_values(centro_esfera,radio_esfera);
    la_esfera.set_color(255,0,0);
    
    vector[0] = la_esfera;

    Punto_Vector centro_esfera_2 = Punto_Vector(0,5,16,1);
    double radio_esfera_2 = 5;
    esfera la_esfera_2;
    la_esfera_2.set_values(centro_esfera_2,radio_esfera_2);
    la_esfera_2.set_color(0,255,0);
    
    vector[1] = la_esfera_2;

    Punto_Vector centro_esfera_3 = Punto_Vector(10,10,16,1);
    double radio_esfera_3 = 4;
    esfera la_esfera_3;
    la_esfera_3.set_values(centro_esfera_3,radio_esfera_3);
    la_esfera_3.set_color(0,255,0);

    vector[2] = la_esfera_3;

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

    // generar threads
    std::thread threads[8];
    for (int t = 1; t <= 8; t++) {
        threads[t-1] = std::thread(rellenar_imagen_esfera, std::ref(imagen), resolution, vector, cam, t, number_of_rays);
    }
    for (int t = 1; t <= 8; t++) {
        threads[t-1].join();
    }

    //rellenar_imagen_esfera(imagen,resolution,vector,cam,1,rebotes);

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

    std::cout << "Tiempo: " << (chrono::duration_cast<std::chrono::microseconds>(end - begin).count())/1000000.0 << " segundos" << std::endl;
    return 0;
}