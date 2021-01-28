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

std::tuple<int,int,int> funcionL(vector<Geometria*> escena, Ray r, double n0, double figura_actual, bool rayo_origen){
    Punto_Vector origen_rayo = r.origen;
    Punto_Vector dir_rayo = r.direccion;
    // Esfera
    int i_figura = 0;
    double t_valor_min = numeric_limits<double>::max();
    Punto_Vector normal;
    for(int i = 0; i < escena.size() ; i++){
        double t_valor = escena[i]->get_interseccion(origen_rayo,dir_rayo, normal);
        if((t_valor < t_valor_min) && t_valor >= 0){
            t_valor_min = t_valor;
            i_figura = i;
        }
    }
    if(t_valor_min != numeric_limits<double>::max()){
        if (escena[i_figura]->es_luz() ) {
            // ha intersectado con la segunda figura, que consideramos luz
            //cout << "interseccion con luz" << endl;
            rgb light = escena[i_figura]->get_colores_kd();
            return std::make_tuple(255 *light.get_red()*escena[i_figura]->get_p_light(), 
                                   255 * light.get_green()*escena[i_figura]->get_p_light(), 
                                   255 * light.get_blue()*escena[i_figura]->get_p_light());
        }
        else {
            // NO HA INTERSECTADO CON LUZ, PERO SI CON OTRO OBJETO

            //Punto_Vector normal;
            // Punto en coordenadas globales
            Punto_Vector punto_figura = r.origen + t_valor_min*dir_rayo;

            if (dynamic_cast<Esfera*>(escena[i_figura]) == nullptr)
            {
                // no es una esfera -> es un PLANO
                bool denom_neg = escena[i_figura]->es_denom_neg(dir_rayo);
                normal = escena[i_figura]->get_normal();
                if(!denom_neg){
                    normal = Punto_Vector(-normal.x,-normal.y,-normal.z,0);
                }
                normal = normal.normalizar();
            }
            if (dynamic_cast<Plano*>(escena[i_figura]) == nullptr)
            {
                // no es un plano -> es una ESFERA
                Punto_Vector centro_figura = escena[i_figura]->get_centro();

                // Obtenemos la normal y creamos coordenadas locales en base a esa normal
                normal = punto_figura - centro_figura; 
                normal = normal.normalizar();
            }


            //next event estimation

            Punto_Vector punto_luz = Punto_Vector(-20,0,20,1);
            double distancia_a_luz = sqrt( pow(punto_figura.x - punto_luz.x,2) +
                                           pow(punto_figura.y - punto_luz.y,2) +
                                           pow(punto_figura.z - punto_luz.z,2) );
            
            
            Punto_Vector normal_nee;
            Punto_Vector dir_rayo_nee = punto_luz - punto_figura;
            dir_rayo_nee = dir_rayo_nee.normalizar();

            punto_figura = punto_figura + normal * 0.02;

            //cout << dir_rayo_nee.x << " " << dir_rayo_nee.y << " " << dir_rayo_nee.z << " " << endl;

            for(int i = 0; i < escena.size() ; i++){
                /*if (i_figura == i){
                    continue;
                }*/
                double distancia_a_objeto = escena[i]->get_interseccion(punto_figura,dir_rayo_nee, normal_nee);

                if((distancia_a_objeto < distancia_a_luz) && distancia_a_objeto >= 0){
                    // se hace sombra
                    return std::make_tuple(0, 0, 0);
                    //goto Sombra;
                }
            }
            //cout << "luz" << endl;
            return std::make_tuple(255, 255, 255);


            Sombra:

                

            double maxpd, maxps, maxpt, pd, ps,pt;
            if(escena[i_figura]->es_dielectrico()){
                dir_rayo = dir_rayo.normalizar();
                double Oi = acos((normal^dir_rayo)/(dir_rayo.modulo() * normal.modulo()));

                double n1 = escena[i_figura]->get_refraccion();
                if(i_figura == figura_actual){
                    n1 = 1;
                }
                const double n = n0 / n1;
                double c1 = -(normal ^ dir_rayo);
                Punto_Vector aux = normal * c1 * 2;
                Punto_Vector r1 = dir_rayo + aux;
                double c2= sqrt(1 - n * n  * (1 - c1 * c1));
                Punto_Vector direccion_refractada = (n * dir_rayo) + (n * (c1 - c2)) * normal;
                direccion_refractada = direccion_refractada.normalizar();
                double Ot = acos((normal^direccion_refractada)/(direccion_refractada.modulo() * normal.modulo()));
                float p_par = (n1*cos(Oi)-n0*cos(Ot))/(n1*cos(Oi)+n0*cos(Ot));
                float p_perp = (n0*cos(Oi)-n1*cos(Ot))/(n0*cos(Oi)+n1*cos(Ot));
                ps = 1/2*(p_par*p_par + p_perp * p_perp);
                pt = 1 - maxps;
                pd = 0;

            }
            else{
                maxpd = escena[i_figura]->get_max_Kd();
                maxps = escena[i_figura]->get_max_Ks();
                maxpt = escena[i_figura]->get_max_Kt();
                //if((pd+ps) > 0.95){
                pd = 0.95 * maxpd / (maxpd + maxps + maxpt);
                ps = 0.95 * maxps / (maxpd + maxps + maxpt);
                pt = 0.95 * maxpt / (maxpd + maxps + maxpt);    
            }


            double num_aleatorio = (double)rand() / RAND_MAX;

             if(num_aleatorio < ps){
                // Si es material especular
                dir_rayo = dir_rayo.normalizar();
                Punto_Vector dir_rayo_inv = Punto_Vector(-dir_rayo.x,-dir_rayo.y,-dir_rayo.z,dir_rayo.valor);
                Punto_Vector direccion_reflejada = (2 * (normal ^ dir_rayo_inv) * normal + dir_rayo);

                r.origen = punto_figura + normal * 0.002;
                r.direccion = direccion_reflejada.normalizar();

                std::tuple<int, int, int> siguiente = funcionL(escena,r,n0,i_figura,false);

                rgb colores_ks = escena[i_figura]->get_colores_ks();

                float red = (get<0>(siguiente) / 255.0) * PI *(colores_ks.get_red()) / (normal^r.direccion) * abs(normal^r.direccion) / ps;
                float green = (get<1>(siguiente) / 255.0) * PI * (colores_ks.get_green()) / (normal^r.direccion) * abs(normal^r.direccion) / ps;
                float blue = (get<2>(siguiente) / 255.0) * PI * (colores_ks.get_blue()) / (normal^r.direccion) * abs(normal^r.direccion) / ps;

                return std::make_tuple(red*255, green*255, blue*255);
            }
            else if(num_aleatorio < pt){
                // Si es material refractado
                double n1 = escena[i_figura]->get_refraccion();
                Punto_Vector cambio = Punto_Vector(-normal.x,-normal.y,-normal.z,0);
                if(i_figura == figura_actual){
                    n1 = 1;
                    cambio = Punto_Vector(normal.x,normal.y,normal.z,0);
                }
                double n = n0 / n1;
                double c1 = -(normal ^ dir_rayo);
                Punto_Vector aux = normal * c1 * 2;
                Punto_Vector r1 = dir_rayo + aux;
                double c2= sqrt(1 - n * n  * (1 - c1 * c1));
                Punto_Vector direccion_refractada = (n * dir_rayo) + (n * (c1 - c2)) * normal;
                r.origen = punto_figura + cambio * 0.02;
                r.direccion = direccion_refractada.normalizar();

                std::tuple<int, int, int> siguiente = funcionL(escena,r,n1,i_figura,false);

                rgb colores_kt = escena[i_figura]->get_colores_kt();

                float red = (get<0>(siguiente) / 255.0) * PI * (colores_kt.get_red()) / (normal^r.direccion) * abs(normal^r.direccion) / pt;
                float green = (get<1>(siguiente) / 255.0) * PI * (colores_kt.get_green()) / (normal^r.direccion) * abs(normal^r.direccion) / pt;
                float blue = (get<2>(siguiente) / 255.0) * PI * (colores_kt.get_blue()) / (normal^r.direccion) * abs(normal^r.direccion) / pt;

                return std::make_tuple(red*255, green*255, blue*255);
            }
            else if( num_aleatorio < pd){
                // Material difuso

                Punto_Vector vector_z,vector_x;
               if(normal.z < 0.){
                    const float a = 1.0f / (1.0f - normal.z);
                    const float b = normal.x * normal.y * a;
                    vector_x = Punto_Vector(1.0f - normal.x * normal.x * a, -b, normal.x, 0);
                    vector_z = Punto_Vector(b, normal.y * normal.y * a - 1.0f, -normal.y, 0);
                }
                else{
                    const float a = 1.0f / (1.0f + normal.z);
                    const float b = -normal.x * normal.y * a;
                    vector_x = Punto_Vector(1.0f - normal.x * normal.x * a, b, -normal.x, 0);
                    vector_z = Punto_Vector(b, 1.0f - normal.y * normal.y * a, -normal.y, 0);
                }
                vector_x = vector_x.normalizar();
                vector_z = vector_z.normalizar();


                Matrix matrizLocal = Matrix(vector_x,normal,vector_z,punto_figura);
                // PARA MATERIAL DIFUSO
                ////////////////////////
                float random1 =  (float) (rand())/ (RAND_MAX);
                float random2 =  (float) (rand())/ (RAND_MAX);

                float zeta = acos(sqrt(1 - random1));
                float fi = 2 * PI * random2;

                Punto_Vector direc_local = Punto_Vector(sin(zeta) * cos(fi),
                                                            sin(zeta) * sin(fi),
                                                            cos(zeta),
                                                            0);
                // Transformamos de coordenadas locales a globales, con la inversa y con eso tenemos el punto y direccion
                Punto_Vector direc_global = matrizLocal*direc_local;

                float valor_w_normal = abs(normal^direc_global);
                r.origen = punto_figura + normal * 0.002;
                r.direccion = direc_global.normalizar();
                ////////////////////////////////////////

                // color de la esfera con la que se ha chocado
                // rgb color = escena[i_figura]->get_colores();
                //cout << "colores vvv: " << color.get_red() << " " << color.get_green() << " " <<  color.get_blue() << endl;


                // el rayo sigue
                std::tuple<int, int, int> siguiente = funcionL(escena,r,n0,i_figura,false);
                // pasamos los colores a [0, 1] para multiplicarlos
                rgb colores_kd = escena[i_figura]->get_colores_kd();

                float red = (get<0>(siguiente) / 255.0) * (colores_kd.get_red()) * valor_w_normal / pd;
                float green = (get<1>(siguiente) / 255.0) * (colores_kd.get_green()) * valor_w_normal / pd;
                float blue = (get<2>(siguiente) / 255.0) * (colores_kd.get_blue()) * valor_w_normal / pd;

                

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
                std::tuple<int, int, int> colorPixel = funcionL(escena,r,1,-1,true);

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
    rgb verde, nada, rojo, blanco, azul, rosa, amarillo,gris;
     verde.set_values(50/255.0, 200/255.0, 50/255.0);
     nada.set_values(0,0,0);
     rojo.set_values(200/255.0, 50/255.0, 50/255.0);
     blanco.set_values(255/255.0, 255/255.0, 255/255.0);
     azul.set_values(0/255,0/255,220/255.0);
     rosa.set_values(255/255.0, 10/255.0, 127/255.0);
     amarillo.set_values(250/255.0, 240/255.0, 10/255.0);
     gris.set_values(0.2, 0.2, 0.2);
    geo.push_back(new Plano(Punto_Vector(-50,0,50,1),Punto_Vector(0,0,1,0),Punto_Vector(0,1,0,0),blanco,nada,nada,0, false,true,13 ));
    geo.push_back(new Plano(Punto_Vector(50,0,50,1),Punto_Vector(0,0,1,0),Punto_Vector(0,1,0,0),blanco,nada,nada,0, false,false,0 ));

    geo.push_back(new Plano(Punto_Vector(0,50,50,1),Punto_Vector(0,0,1,0),Punto_Vector(1,0,0,0),verde,nada,nada,0, false,false,0 ));
    geo.push_back(new Plano(Punto_Vector(0,-50,50,1),Punto_Vector(0,0,-1,0),Punto_Vector(1,0,0,0),rojo,nada,nada,0, false,false,0 ));

    geo.push_back(new Plano(Punto_Vector(0,0,100,1),Punto_Vector(0,1,0,0),Punto_Vector(1,0,0,0),blanco,nada,nada,0, false,false,0 ));
    geo.push_back(new Plano(Punto_Vector(0,0,-100,1),Punto_Vector(0,1,0,0),Punto_Vector(1,0,0,0),rosa,nada,nada,0, false,false,0 ));

    geo.push_back(new Esfera(Punto_Vector(0,0,25,1), 3 , azul,nada,nada,0, false,false,0));

    //geo.push_back(new Esfera(Punto_Vector(10,0,15,1), 3 , azul,nada,nada,0, false,false,0));
    //geo.push_back(new Esfera(Punto_Vector(5,10,16,1), 3, amarillo,blanco,nada,false,0));
    geo.push_back(new Esfera(Punto_Vector(-5,-10,16,1), 3, nada,blanco,nada,0, false,false,0));
    geo.push_back(new Esfera(Punto_Vector(5,10,16,1), 3, nada,nada,blanco,1.45, false,false,0));
    //geo.push_back(new Esfera(Punto_Vector(0,0,20,1), 5, nada,blanco,blanco,1.45, true,false,0));

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
    //rellenar_imagen_esfera(imagen,resolution,geo,cam,1,number_of_rays);
    // gamma
    float gamma = 0.5;
    if (gamma != 1) {
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