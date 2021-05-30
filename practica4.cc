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
#include "punto_luz.cc"

#define PI 3.14159265358979

using namespace std;

// Clase para definir un rayo
class Ray{
    public:
    // Un rayo tiene tanto un origen como una direccion
    Punto_Vector origen;
    Punto_Vector direccion;

        // Constructor
        Ray (Punto_Vector _o, Punto_Vector _d) {
            origen = _o;
            direccion = _d;
        }
};

// Clase para definir una cámara
class Camera{
    // Una camara tiene tanto un vector u,l y f y un origen
    Punto_Vector origen, u, l, f;

    public:
        // Constructor
        Camera (Punto_Vector _c, Punto_Vector _u, Punto_Vector _l, Punto_Vector _f) {
            origen = _c;
            u = _u.normalizar();
            l = _l.normalizar();
            f = _f;
            srand(time(NULL));
        }
        
        // Devuelve el rayo centrado de una resolución concreta segun la dimension del pixel
        Ray get_center_ray(const int x, const int y, const int resolucion) {
            double dim_pixel = 2.0/(double)resolucion;
            double new_x = -1 + dim_pixel / 2 + x * dim_pixel;
            double new_y = 1 - dim_pixel / 2 - y * dim_pixel;
            return Ray(origen, Punto_Vector(new_x,new_y,1,0));
        }

        // Devuelve un rayo aleatorio en una resolucion dada segun la dimension del pixel
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

std::tuple<double,double,double> fr(Geometria* figura, Punto_Vector wi, Punto_Vector wo, Ray r, Punto_Vector normal, Punto_Vector punto_figura){

    // Calculamos direccion reflejada
    Punto_Vector dir_rayo_inv = Punto_Vector(-wi.x,-wi.y,-wi.z,wi.valor);
    Punto_Vector direccion_reflejada = (2 * (normal ^ dir_rayo_inv) * normal + wi);
    
    // Calculamos direccion refractada
    double n1 = figura->get_refraccion();
    double n = 1 / n1;
    double c1 = abs(normal ^ wi);
    Punto_Vector aux = normal * c1 * 2;
    Punto_Vector r1 = wi + aux;
    double c2= sqrt(1 - n * n  * (1 - c1 * c1));
    Punto_Vector direccion_refractada = (n * wi) + (n * (c1 - c2)) * normal;
    
    // Calculamos el punto dentro de la esfera y la direccion refractada
    Punto_Vector cambio = Punto_Vector(-normal.x,-normal.y,-normal.z,0); // Negamos la normal para que sea hacia adentro
    Punto_Vector punto_refract = punto_figura + cambio * 0.02;
    direccion_refractada = direccion_refractada.normalizar();
    // Sacamos el valor de t del nuevo punto y el nuevo punto
    float new_t_value,distancia;
    //bool refracta = escena[i_figura]->get_interseccion(punto_refract,direccion_refractada, distancia, new_t_value);
    punto_refract = punto_refract + new_t_value*direccion_refractada;

    Punto_Vector normal_n;
    if (dynamic_cast<Plano*>(figura) == nullptr){
        Punto_Vector centro_figura = figura->get_centro();
    
        normal_n = punto_refract - centro_figura; 
        normal_n = normal.normalizar();
        // Calculamos las ecuaciones de refraccion otra vez, para sacar la direccion de salida
        n = n1 / 1;
        c1 = abs(normal_n ^ wi);
        aux = normal_n * c1 * 2;
        r1 = wi + aux;
        c2= sqrt(1 - n * n  * (1 - c1 * c1));
        direccion_refractada = (n * wi) + (n * (c1 - c2)) * normal_n;
    }
    else{
        direccion_refractada = Punto_Vector(0,0,0,0);
    }
    int constante_wr = 0;
    int constante_wt = 0;
    if(wo.equal(direccion_reflejada)){
        constante_wr = 1;
    }

    if(wo.equal(direccion_refractada)){
        constante_wt = 1;
    }
    // Calculamos la BSDF del material
    double valor_red = 0;
    double valor_green = 0;
    double valor_blue = 0;
    valor_red = figura->get_colores_kd().get_red() / PI + (figura->get_colores_ks().get_red() * constante_wr) / (normal^r.direccion) + (figura->get_colores_kt().get_red() * constante_wt / (normal^r.direccion));
    valor_green = figura->get_colores_kd().get_green() / PI + (figura->get_colores_ks().get_green() * constante_wr) / (normal^r.direccion) + (figura->get_colores_kt().get_green() * constante_wt / (normal^r.direccion));
    valor_blue = figura->get_colores_kd().get_blue() / PI + (figura->get_colores_ks().get_blue() * constante_wr) / (normal^r.direccion) + (figura->get_colores_kt().get_blue() * constante_wt / (normal^r.direccion));
    return std::make_tuple(valor_red, valor_green, valor_blue);
}
// Ray tracing
// Devuelve una tupla RGB
std::tuple<int,int,int> funcionL(vector<Geometria*> escena, Ray r, vector<Punto_Luz*> luces){
    
    // Definimos tanto el origen como la direccion del rayo
    Punto_Vector origen_rayo = r.origen;
    Punto_Vector dir_rayo = r.direccion;
    int i_figura = 0;
    float distancia_min = numeric_limits<float>::max();
    float t_valor_min;
    Punto_Vector normal;
    // Comprobamos cual de todos los objetos de la escena es el mas cercano
    for(int i = 0; i < escena.size() ; i++){
        float t_valor, distancia;
        bool choca = escena[i]->get_interseccion(origen_rayo,dir_rayo, distancia, t_valor);
        // Si choca comprobamos que la distancia es menor a la menor de todas las distancias
        if(choca){
            if(distancia < distancia_min){
                // Guardamos tanto la distancia minima, como con que figura choca como el valor de t
                distancia_min = distancia;
                i_figura = i;
                t_valor_min = t_valor;
            }
        }
    }
    if(t_valor_min != numeric_limits<float>::max()){
        if (escena[i_figura]->es_luz() ) {
            // ha intersectado con una figura que es luz, entonces devolvemos la luz
                rgb light = escena[i_figura]->get_colores_kd();
                if(luces.size() > 0){
                    // Luz para next event estimation
                    return std::make_tuple(255 *light.get_red()*escena[i_figura]->get_p_light(), 
                                   255 * light.get_green()*escena[i_figura]->get_p_light(), 
                                   255 * light.get_blue()*escena[i_figura]->get_p_light());  

                }
                else{
                    // Luz si no hay luces puntuales
                    return std::make_tuple(255 *light.get_red()*escena[i_figura]->get_p_light(), 
                                   255 * light.get_green()*escena[i_figura]->get_p_light(), 
                                   255 * light.get_blue()*escena[i_figura]->get_p_light());  
                }
        }
        else {
            // NO HA INTERSECTADO CON LUZ, PERO SI CON OTRO OBJETO

            // Punto en coordenadas globales
            Punto_Vector punto_figura = origen_rayo + t_valor_min*dir_rayo;

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


            // NEXT EVENT STIMATION
            Punto_Vector punto_luz;
            float distancia_a_luz;
            Punto_Vector normal_nee;
            Punto_Vector dir_rayo_nee;
            int luz_random;
            double valor_luz = 0;
            if(luces.size() > 0){
                for(int i = 0; i < luces.size(); i++){
                    punto_luz = luces[i]->get_punto();
                    distancia_a_luz = sqrt( pow(punto_figura_luz.x - punto_luz.x,2) +
                                           pow(punto_figura_luz.y - punto_luz.y,2) +
                                           pow(punto_figura_luz.z - punto_luz.z,2));
                    valor_luz = luces[i]->get_power / (distancia_a_luz*distancia_a_luz);
                }
            }

            // Calculamos el valor del material
            double maxpd, maxps, maxpt, pd, ps,pt;
            if(escena[i_figura]->es_dielectrico()){ // Material dielectrico
                // Calculamos las ecuaciones de fresnel
                dir_rayo = dir_rayo.normalizar();
                double Oi = acos((normal^dir_rayo)/(dir_rayo.modulo() * normal.modulo()));

                double n1 = escena[i_figura]->get_refraccion();
                const double n = 1 / n1;
                double c1 = -(normal ^ dir_rayo);
                Punto_Vector aux = normal * c1 * 2;
                Punto_Vector r1 = dir_rayo + aux;
                double c2= sqrt(1 - n * n  * (1 - c1 * c1));
                Punto_Vector direccion_refractada = (n * dir_rayo) + (n * (c1 - c2)) * normal;
                direccion_refractada = direccion_refractada.normalizar();
                double Ot = acos((normal^direccion_refractada)/(direccion_refractada.modulo() * normal.modulo()));
                float p_par = (n1*cos(Oi)-1*cos(Ot))/(n1*cos(Oi)+1*cos(Ot));
                float p_perp = (1*cos(Oi)-n1*cos(Ot))/(1*cos(Oi)+n1*cos(Ot));
                ps = (p_par*p_par + p_perp * p_perp)/2;
                pt = 1 - ps;
                pd = 0;

            }
            else{
                // Calculamos una probabilidad en base a los valores Kd,Ks y Kt de las figuras
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
                std::tuple<double, double, double> bsdf_material = fr(escena[i_figura],dir_rayo,direccion_reflejada,r,normal,punto_figura);
                r.origen = punto_figura + normal * 0.002;
                r.direccion = direccion_reflejada.normalizar();

                std::tuple<int, int, int> siguiente = funcionL(escena,r,luces);

                rgb colores_ks = escena[i_figura]->get_colores_ks();
                rgb colores_kd = escena[i_figura]->get_colores_kd();
                float red,green,blue;
                if(luces.size() > 0){ // Next event estimation
                    if(escena[i_figura]->es_dielectrico()){
                        red = valor_luz * (get<0>(siguiente) / 255.0) * PI * ps / (normal^r.direccion) * abs(normal^r.direccion) / ps;
                        green = valor_luz * (get<1>(siguiente) / 255.0) * PI * ps / (normal^r.direccion) * abs(normal^r.direccion) / ps;
                        blue = valor_luz * (get<2>(siguiente) / 255.0) * PI * ps / (normal^r.direccion) * abs(normal^r.direccion) / ps;
                    }
                    else{
                        red = valor_luz * (get<0>(siguiente) / 255.0) * PI * get<0>(bsdf_material) * abs(normal^r.direccion) / ps;
                        green = valor_luz * (get<1>(siguiente) / 255.0) * PI * get<1>(bsdf_material) * abs(normal^r.direccion) / ps;
                        blue = valor_luz * (get<2>(siguiente) / 255.0) * PI * get<2>(bsdf_material)  * abs(normal^r.direccion) / ps;
                    }
                }
                else{
                    if(escena[i_figura]->es_dielectrico()){
                        red = (get<0>(siguiente) / 255.0) * PI * ps / (normal^r.direccion) * abs(normal^r.direccion) / ps;
                        green = (get<1>(siguiente) / 255.0) * PI * ps / (normal^r.direccion) * abs(normal^r.direccion) / ps;
                        blue = (get<2>(siguiente) / 255.0) * PI * ps / (normal^r.direccion) * abs(normal^r.direccion) / ps;
                    }
                    else{
                        red = (get<0>(siguiente) / 255.0) * PI * get<0>(bsdf_material) * abs(normal^r.direccion) / ps;
                        green = (get<1>(siguiente) / 255.0) * PI * get<1>(bsdf_material) * abs(normal^r.direccion) / ps;
                        blue = (get<2>(siguiente) / 255.0) * PI * get<2>(bsdf_material)  * abs(normal^r.direccion) / ps;
                    }
                }
                return std::make_tuple(red*255, green*255, blue*255);
            }
            else if(num_aleatorio < pt){
                // Si es material refractado realizamos las ecuaciones para sacar la direccion refractada
                double n1 = escena[i_figura]->get_refraccion();
                double n = 1 / n1;
                double c1 = abs(normal ^ dir_rayo);
                Punto_Vector aux = normal * c1 * 2;
                Punto_Vector r1 = dir_rayo + aux;
                double c2= sqrt(1 - n * n  * (1 - c1 * c1));
                Punto_Vector direccion_refractada = (n * dir_rayo) + (n * (c1 - c2)) * normal;
                
                // Calculamos el punto dentro de la esfera y la direccion refractada
                Punto_Vector cambio = Punto_Vector(-normal.x,-normal.y,-normal.z,0); // Negamos la normal para que sea hacia adentro
                Punto_Vector punto_refract = punto_figura + cambio * 0.02;
                direccion_refractada = direccion_refractada.normalizar();
                // Sacamos el valor de t del nuevo punto y el nuevo punto
                float new_t_value,distancia;
                bool refracta = escena[i_figura]->get_interseccion(punto_refract,direccion_refractada, distancia, new_t_value);
                punto_refract = punto_refract + new_t_value*direccion_refractada;

                Punto_Vector centro_figura = escena[i_figura]->get_centro();
                normal = punto_refract - centro_figura; 
                normal = normal.normalizar();

                // Calculamos las ecuaciones de refraccion otra vez, para sacar la direccion de salida
                n = n1 / 1;
                c1 = abs(normal ^ dir_rayo);
                aux = normal * c1 * 2;
                r1 = dir_rayo + aux;
                c2= sqrt(1 - n * n  * (1 - c1 * c1));
                direccion_refractada = (n * dir_rayo) + (n * (c1 - c2)) * normal;


                std::tuple<double, double, double> bsdf_material = fr(escena[i_figura],dir_rayo,direccion_refractada,r,normal,punto_figura);


                r.origen = punto_refract + normal * 0.002;
                r.direccion = direccion_refractada.normalizar();

                std::tuple<int, int, int> siguiente = funcionL(escena,r,luces);

                rgb colores_kt = escena[i_figura]->get_colores_kt();
                float red,green,blue;
                if(luces.size() > 0){
                    if(escena[i_figura]->es_dielectrico()){
                        red = valor_luz * (get<0>(siguiente) / 255.0) * PI * (colores_kt.get_red()) / (normal^r.direccion) * abs(normal^r.direccion) / pt;
                        green = valor_luz * (get<1>(siguiente) / 255.0) * PI * (colores_kt.get_green()) / (normal^r.direccion) * abs(normal^r.direccion) / pt;
                        blue = valor_luz * (get<2>(siguiente) / 255.0) * PI * (colores_kt.get_blue()) / (normal^r.direccion) * abs(normal^r.direccion) / pt;
                    }
                    else{
                        red = valor_luz * (get<0>(siguiente) / 255.0) * PI * get<0>(bsdf_material) * abs(normal^r.direccion) / pt;
                        green = valor_luz * (get<1>(siguiente) / 255.0) * PI * get<1>(bsdf_material) * abs(normal^r.direccion) / pt;
                        blue = valor_luz * (get<2>(siguiente) / 255.0) * PI * get<2>(bsdf_material)  * abs(normal^r.direccion) / pt;
                    }
                }
                else{
                    if(escena[i_figura]->es_dielectrico()){
                        red = (get<0>(siguiente) / 255.0) * PI * pt / (normal^r.direccion) * abs(normal^r.direccion) / pt;
                        green = (get<1>(siguiente) / 255.0) * PI * pt / (normal^r.direccion) * abs(normal^r.direccion) / pt;
                        blue = (get<2>(siguiente) / 255.0) * PI * pt / (normal^r.direccion) * abs(normal^r.direccion) / pt;
                    }
                    else{
                        red = (get<0>(siguiente) / 255.0) * PI * get<0>(bsdf_material) * abs(normal^r.direccion) / pt;
                        green = (get<1>(siguiente) / 255.0) * PI * get<1>(bsdf_material) * abs(normal^r.direccion) / pt;
                        blue = (get<2>(siguiente) / 255.0) * PI * get<2>(bsdf_material)  * abs(normal^r.direccion) / pt;
                    }
                }


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
                

                Matrix matrizLocal = Matrix(vector_x,normal,vector_z,punto_figura);
                // PARA MATERIAL DIFUSO
                ////////////////////////
                float random1 =  (float) (rand())/ (RAND_MAX);
                float random2 =  (float) (rand())/ (RAND_MAX);

                float zeta = acos(sqrt(1 - random1));
                float fi = 2 * PI * random2;

                Punto_Vector direc_local = Punto_Vector(sin(zeta) * cos(fi),
                                                            cos(zeta),
                                                            sin(zeta) * sin(fi),
                                                            0);
                // Transformamos de coordenadas locales a globales, con la inversa y con eso tenemos el punto y direccion
                Punto_Vector direc_global = matrizLocal*direc_local;


                Punto_Vector aux = Punto_Vector(0,0,0,0);
                std::tuple<double, double, double> bsdf_material = fr(escena[i_figura],dir_rayo,aux,r,normal,punto_figura);

                float valor_w_normal = abs(normal^direc_global);
                r.origen = punto_figura + normal * 0.002;
                r.direccion = direc_global.normalizar();
                ////////////////////////////////////////
                
                int figura_actual = i_figura;
                // el rayo sigue
                std::tuple<int, int, int> siguiente = funcionL(escena,r,luces);
                // pasamos los colores a [0, 1] para multiplicarlos
                rgb colores_kd = escena[i_figura]->get_colores_kd();
                float red,green,blue;
                if(luces.size() > 0){
                    red = valor_luz *  (get<0>(siguiente) / 255.0) * get<0>(bsdf_material) * PI * valor_w_normal / pd;
                    green = valor_luz * (get<1>(siguiente) / 255.0) * get<0>(bsdf_material) * PI * valor_w_normal / pd;
                    blue = valor_luz * (get<2>(siguiente) / 255.0) * get<0>(bsdf_material) * PI * valor_w_normal / pd;
                }
                else{
                    red = (get<0>(siguiente) / 255.0) * get<0>(bsdf_material) * PI * valor_w_normal / pd;
                    green = (get<1>(siguiente) / 255.0) * get<1>(bsdf_material) * PI * valor_w_normal / pd;
                    blue = (get<2>(siguiente) / 255.0) * get<2>(bsdf_material) * PI * valor_w_normal / pd;
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


// Funcion que rellena la imagen con los rayos correspondientes
void rellenar_imagen_esfera(vector<float> &imagen, const int resolution,  vector<Geometria*> escena, Camera cam, int thread, int number_of_rays, vector<Punto_Luz*> luces){
    // Dividimos la escena en 8 trozos iguales
    int j1, j2;
    j1 = (thread - 1) * (resolution / 8);
    j2 = thread * (resolution / 8);

    if (thread < 1 || thread > 8) {
        std::cout << "Error en los threads" << endl;
        exit(1);
    }
    // Empezamos a rellenar la escena
    for (int i = resolution-1; i >= 0; i--){
        for (int j = j1; j < j2; j++){

            // Disparamos un rayo desde la camara
            Ray r = cam.get_center_ray(i,j,resolution);
            r.direccion = r.direccion.normalizar();
            double radiancia_total = 0;

            std::tuple<int, int, int> totalPixel = std::make_tuple(0, 0, 0);
            // Por cada numero de rayos nos quedamos con su color
            for (int i = 0; i < number_of_rays; i++){
                std::tuple<int, int, int> colorPixel = funcionL(escena,r,luces);

                get<0>(totalPixel) = get<0>(totalPixel) + get<0>(colorPixel);
                get<1>(totalPixel) = get<1>(totalPixel) + get<1>(colorPixel);
                get<2>(totalPixel) = get<2>(totalPixel) + get<2>(colorPixel);
            }

            // Sacamos la media de color de los rayos
            get<0>(totalPixel) = get<0>(totalPixel) / number_of_rays;
            get<1>(totalPixel) = get<1>(totalPixel) / number_of_rays;
            get<2>(totalPixel) = get<2>(totalPixel) / number_of_rays;

            // Y la guardamos en la matriz de imagenes
            imagen[i*resolution*3 + j*3] = get<0>(totalPixel);
            imagen[i*resolution*3 + j*3 + 1] = get<1>(totalPixel);
            imagen[i*resolution*3 + j*3 + 2] = get<2>(totalPixel);
        }
    }
}


int main(int argc, char **argv) {

    if(argc < 4) {
        cout << "Argument ppp or argument resolution or argument image.ppm missing" << endl;
        exit(1);
    }

    int number_of_rays = atoi(argv[1]);

    const int resolution = atoi(argv[2]);

    vector<float> imagen (resolution * resolution * 3);

    // Escribimos en el fichero ppm los datos correspondientes
    ofstream out;
    out.open(argv[3]);
    out << "P3" << endl;
    out << resolution << " " << resolution << endl;
    out << 255 << endl;
    
    // Definimos nuestra escena
    std::vector<Geometria*> geo;
    std::vector<Punto_Luz*> vector_luces;
    rgb verde, nada, rojo, blanco, azul, rosa, amarillo,gris;
    verde.set_values(50/255.0, 200/255.0, 50/255.0);
    nada.set_values(0,0,0);
    rojo.set_values(200/255.0, 50/255.0, 50/255.0);
    blanco.set_values(255/255.0, 255/255.0, 255/255.0);
    azul.set_values(0/255,0/255,220/255.0);
    rosa.set_values(255/255.0, 10/255.0, 127/255.0);
    amarillo.set_values(250/255.0, 240/255.0, 10/255.0);
    gris.set_values(0.2, 0.2, 0.2);
    int tipo_escena = 0;
    if(tipo_escena == 0){
        // Escena sin next event estimation
        geo.push_back(new Plano(Punto_Vector(-50,0,50,1),Punto_Vector(0,0,1,0),Punto_Vector(0,1,0,0),blanco,nada,nada,0, false,true,4 ));
        geo.push_back(new Plano(Punto_Vector(50,0,50,1),Punto_Vector(0,0,1,0),Punto_Vector(0,1,0,0),gris,nada,nada,0, false,false,0 ));

        geo.push_back(new Plano(Punto_Vector(0,50,50,1),Punto_Vector(0,0,1,0),Punto_Vector(1,0,0,0),verde,nada,nada,0, false,false,0 ));
        geo.push_back(new Plano(Punto_Vector(0,-50,50,1),Punto_Vector(0,0,-1,0),Punto_Vector(1,0,0,0),rojo,nada,nada,0, false,false,0 ));

        geo.push_back(new Plano(Punto_Vector(0,0,100,1),Punto_Vector(0,1,0,0),Punto_Vector(1,0,0,0),gris,nada,nada,0, false,false,0 ));
        geo.push_back(new Plano(Punto_Vector(0,0,-100,1),Punto_Vector(0,1,0,0),Punto_Vector(1,0,0,0),gris,nada,nada,0, false,false,0 ));

        geo.push_back(new Esfera(Punto_Vector(40,30,60,1), 10, nada,nada,blanco,1.35, false,false,0));
        geo.push_back(new Esfera(Punto_Vector(40,-20,60,1), 10, nada,blanco,nada,1.35, false,false,0));
    }
    else{
        // Escena con next event estimation
        geo.push_back(new Plano(Punto_Vector(-50,0,50,1),Punto_Vector(0,0,1,0),Punto_Vector(0,1,0,0),gris,nada,nada,0, false,true,4 ));
        geo.push_back(new Plano(Punto_Vector(50,0,50,1),Punto_Vector(0,0,1,0),Punto_Vector(0,1,0,0),gris,nada,nada,0, false,false,0 ));

        geo.push_back(new Plano(Punto_Vector(0,50,50,1),Punto_Vector(0,0,1,0),Punto_Vector(1,0,0,0),verde,nada,nada,0, false,false,0 ));
        geo.push_back(new Plano(Punto_Vector(0,-50,50,1),Punto_Vector(0,0,-1,0),Punto_Vector(1,0,0,0),rojo,nada,nada,0, false,false,0 ));

        geo.push_back(new Plano(Punto_Vector(0,0,100,1),Punto_Vector(0,1,0,0),Punto_Vector(1,0,0,0),gris,nada,nada,0, false,false,0 ));
        geo.push_back(new Plano(Punto_Vector(0,0,-100,1),Punto_Vector(0,1,0,0),Punto_Vector(1,0,0,0),gris,nada,nada,0, false,false,0 ));

        geo.push_back(new Esfera(Punto_Vector(40,20,60,1), 10, blanco,blanco,nada,0, false,false,0));
        geo.push_back(new Esfera(Punto_Vector(40,-20,60,1), 10, nada,blanco,blanco,1.35, true,false,0));

        vector_luces.push_back(new Punto_Luz(Punto_Vector(-30,0,50,1),blanco,1000));
    }

    // Definimos la cámara
    Camera cam = Camera(Punto_Vector(0,0,0,1),
                        Punto_Vector(0,1,0,0),
                        Punto_Vector(1,0,0,0),
                        Punto_Vector(0,0,1,0));

    chrono::steady_clock::time_point begin = chrono::steady_clock::now();

    // generar threads
    std::thread threads[8];
    for (int t = 1; t <= 8; t++) {
        threads[t-1] = std::thread(rellenar_imagen_esfera, std::ref(imagen), resolution, geo, cam, t, number_of_rays, vector_luces);
    }
    for (int t = 1; t <= 8; t++) {
        threads[t-1].join();
    }
    // gamma
    float gamma = 0.5;
    if (gamma != 1) {
        for (int i = 0; i < resolution; i++) {
            for (int j = 0; j < resolution; j++) {
                for( int k = 0 ; k < 3 ; k++){
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