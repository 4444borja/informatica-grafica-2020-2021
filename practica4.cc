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
#include "esfera.cc"
#include "plano.cc"

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

std::tuple<int,int,int> funcionL(vector<Geometria*> escena, Ray r){
    Punto_Vector origen_rayo = r.origen;
    Punto_Vector dir_rayo = r.direccion;
    rgb colores_figura;
    // Esfera
    int i_figura = 0;
    double t_valor_min = numeric_limits<double>::max();

    for(int i = 0; i < escena.size() ; i++){
        double t_valor = escena[i]->get_interseccion(origen_rayo,dir_rayo);
        if((t_valor < t_valor_min) && t_valor >= 0){
            t_valor_min = t_valor;
            colores_figura = escena[i]->get_colores();
            i_figura = i;
        }
    }
    if(t_valor_min != numeric_limits<double>::max()){
        if (i_figura == 2 ) {
            // ha intersectado con la segunda figura, que consideramos luz
            //cout << "interseccion con luz" << endl;
            return std::make_tuple(1000, 1000, 1000);
        }
        else {
            // NO HA INTERSECTADO CON LUZ, PERO SI CON OTRO OBJETO

            Punto_Vector vector_y;
            // Punto en coordenadas globales
            Punto_Vector punto_figura = r.origen + t_valor_min*dir_rayo;

            bool plano = false;

            if (dynamic_cast<Esfera*>(escena[i_figura]) == nullptr)
            {
                // no es una esfera -> es un PLANO
                vector_y = escena[i_figura]->get_normal();
                vector_y = vector_y.normalizar();
            }
            if (dynamic_cast<Plano*>(escena[i_figura]) == nullptr)
            {
                // no es un plano -> es una ESFERA
                Punto_Vector centro_figura = escena[i_figura]->get_centro();

                // Obtenemos la normal y creamos coordenadas locales en base a esa normal
                vector_y = centro_figura - punto_figura; 
                vector_y = vector_y.normalizar();
            }

           

            /*vector_y = vector_y.normalizar();
            Punto_Vector vector_z;
            Punto_Vector vector_x = operatorx(vector_y,Punto_Vector(0,0,1,0)); 

            if(vector_x.x == 0.0 && vector_x.y == 0.0 && vector_x.z == 0.0){
                vector_z = operatorx(vector_y,Punto_Vector(1,0,0,0));
                vector_z = vector_z.normalizar();
                vector_x = operatorx(vector_z, vector_y);
                vector_x = vector_x.normalizar();
            }
            else{
                vector_x = vector_x.normalizar();
                vector_z = operatorx(vector_x, vector_y);
                vector_z = vector_z.normalizar();
            }*/
            Punto_Vector vector_z,vector_x;
            if(vector_y.z < 0){
                const float a = 1.0f / (1.0f - vector_y.z);
                const float b = vector_y.x * vector_y.y * a;
                vector_x = Punto_Vector(1.0f - vector_y.x * vector_y.x * a, -b, vector_y.x, 0);
                vector_z = Punto_Vector(b, vector_y.y * vector_y.y * 1 - 1.0f, -vector_y.y, 0);
            }
            else{
                const float a = 1.0f / (1.0f + vector_y.z);
                const float b = -vector_y.x * vector_y.y * a;
                vector_x = Punto_Vector(1.0f - vector_y.x * vector_y.x * a, b, -vector_y.x, 0);
                vector_z = Punto_Vector(b, 1.0f - vector_y.y * vector_y.y * a, -vector_y.y, 0);
            }

            Matrix matrizLocal = Matrix(vector_x,vector_y,vector_z,punto_figura);

            // PARA MATERIAL DIFUSO
            ////////////////////////
            float random1 = (float) rand()/RAND_MAX;
            float random2 = (float) rand()/RAND_MAX;

            double zeta = acos(sqrt(1 - random2));
            double fi = 2 * PI * random1;

            Punto_Vector direc_local = Punto_Vector(sin(zeta) * cos(fi),
                                                        sin(zeta) * sin(fi),
                                                        cos(zeta),
                                                        0);
            // Transformamos de coordenadas locales a globales, con la inversa y con eso tenemos el punto y direccion
            Punto_Vector direc_global = matrizLocal*direc_local;

            double valor_w_normal = abs(vector_y^direc_global);
            r.origen = punto_figura + vector_y * 0.02;;
            r.direccion = direc_global;
            ////////////////////////////////////////

            // color de la esfera con la que se ha chocado
            // rgb color = escena[i_figura]->get_colores();
            //cout << "colores vvv: " << color.get_red() << " " << color.get_green() << " " <<  color.get_blue() << endl;

            // ruleta rusa para ver si seguimos
            if( rand() % 10 < 8){
                // el rayo sigue
                std::tuple<int, int, int> siguiente = funcionL(escena,r);

                if (plano){
                    //cout << "color siguiente del plano " << get<0>(siguiente) << " " << get<1>(siguiente) << " " << get<2>(siguiente) << endl;
                }


                // pasamos los colores a [0, 1] para multiplicarlos
                float red = (get<0>(siguiente) / 255.0) * (colores_figura.get_red() / 255.0);
                float green = (get<1>(siguiente) / 255.0) * (colores_figura.get_green() / 255.0);
                float blue = (get<2>(siguiente) / 255.0) * (colores_figura.get_blue() / 255.0);

                

                /*if(red > 1 || red < 0 || green > 1 || green < 0 || blue > 1 || blue < 0){
                    cout << "ERROR:"<< endl;
                    cout << get<0>(siguiente) << endl;
                    cout << colores_figura.get_red() << endl;
                    cout << (get<0>(siguiente) / 255.0) << endl;
                    cout << (colores_figura.get_red() / 255.0) << endl;
                    cout << red << endl;
                }*/

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



void rellenar_imagen_esfera(vector<float> &imagen, const int resolution,  vector<Geometria*> escena, Camera cam, int thread, int number_of_rays){
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

    if(argc < 2) {
        cout << "Argument ppp missing" << endl;
        exit(1);
    }

    int number_of_rays = atoi(argv[1]);

    const int resolution = 500;

    vector<float> imagen (resolution * resolution * 3);

    ofstream out;
    out.open("practica4.ppm");
    out << "P3" << endl;
    out << resolution << " " << resolution << endl;
    out << 255 << endl;
    
    std::vector<Geometria*> geo;
    geo.push_back(new Plano(Punto_Vector(0,0.1,0,0),30,240,120,110 ));
    geo.push_back(new Plano(Punto_Vector(0,-0.1,0,0),30, 120,240,110 ));

    geo.push_back(new Plano(Punto_Vector(0.1,0,0,0),30,255,255,255 ));
    geo.push_back(new Plano(Punto_Vector(-0.1,0,0,0),30,255,255,255 ));

    geo.push_back(new Plano(Punto_Vector(0,0,-0.1,0),50,255,255,255 ));
    geo.push_back(new Plano(Punto_Vector(0,0,0.1,0),1,255,255,255 )); 

    //geo.push_back(new Esfera(Punto_Vector(50,0,50 ,1), 10, 255, 255, 255));
    //geo.push_back(new Esfera(Punto_Vector(0,5,20,1), 5, 0, 255, 0));
    //geo.push_back(new Esfera(Punto_Vector(10,5,20,1), 5, 255, 0, 255));

    Camera cam = Camera(Punto_Vector(0,0,0,1),
                        Punto_Vector(0,1,0,0),
                        Punto_Vector(1,0,0,0),
                        Punto_Vector(0,0,1,0));

    chrono::steady_clock::time_point begin = chrono::steady_clock::now();

    // generar threads
    std::thread threads[8];
    for (int t = 1; t <= 8; t++) {
        threads[t-1] = std::thread(rellenar_imagen_esfera, std::ref(imagen), resolution, geo, cam, t, number_of_rays);
    }
    for (int t = 1; t <= 8; t++) {
        threads[t-1].join();
    }
    
    // gamma
    float gamma = 0.5;
    for (int i = 0; i < resolution; i++) {
        for (int j = 0; j < resolution; j++) {
            for( int k = 0 ; k < 3 ; k++){
                // eq
                //imagen[i*resolution*3 + j*3 + k] = (imagen[i*resolution*3 + j*3 + k] ) / 255;
                // gamma
                imagen[i*resolution*3 + j*3 + k] = pow(imagen[i*resolution*3 + j*3 + k] / 255 , gamma) * 255;
                //clamping
                if (imagen[i*resolution*3 + j*3 + k] > 255) {
                    imagen[i*resolution*3 + j*3 + k] = 255;
                }
            }
        }
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

    std::cout << "Tiempo: " << (chrono::duration_cast<std::chrono::microseconds>(end - begin).count())/1000000.0 << " segundos" << std::endl;
    return 0;
}