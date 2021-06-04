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

double clamp(double value, double high, double low){
    if(value > high){
        return high;
    }
    if(value < low){
        return low;
    }
    return value;
}
// Ray tracing
// Devuelve una tupla RGB
Punto_Vector refraccion(const Punto_Vector I, const Punto_Vector Normal, const float ind_refrac){
    double cos = I^Normal;
    float cosi = clamp(cos, 1, -1); 
    float etai = 1, etat = ind_refrac; 
    Punto_Vector n = Normal; 
    if (cosi < 0) { cosi = -cosi; } else { swap(etai, etat); n= Punto_Vector(-Normal.x,-Normal.y,-Normal.z,Normal.valor); } 
    float eta = etai / etat; 
    float k = 1 - eta * eta * (1 - cosi * cosi); 
    return k < 0 ? Punto_Vector(0,0,0,0) : eta * I + (eta * cosi - sqrtf(k)) * n;

}

float fresnel(const Punto_Vector I, const Punto_Vector Normal, const float ind_refrac){
    double cos = I^Normal;
    float cosi = clamp(cos, 1, -1);  
    float etai = 1, etat = ind_refrac;
    if (cosi > 0) {swap(etai, etat); } 
    // Calculamos sint utilizando la ley de snell
    float sint = etai / etat * sqrtf(max(0.f, 1 - cosi * cosi)); 
    // Total internal reflection
    if (sint >= 1) { 
        return 1; 
    } 
    else { 
        float cost = sqrtf(max(0.f, 1 - sint * sint)); 
        cosi = fabsf(cosi); 
        float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost)); 
        float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost)); 
        return (Rs * Rs + Rp * Rp) / 2; 
    } 
    // As a consequence of the conservation of energy, transmittance is given by:
    // kt = 1 - kr;
}
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
            // Ha intersectado con una figura que emite luz. Multiplicamos el color de la figura con la potencia del objeto
            rgb light = escena[i_figura]->get_colores_kd();
            return std::make_tuple(light.get_red()*escena[i_figura]->get_p_light(), 
                            light.get_green()*escena[i_figura]->get_p_light(), 
                            light.get_blue()*escena[i_figura]->get_p_light());  
        }
        else {
            // Ha intersectado con otro objeto que no emite luz. Calculamos la luz directa y luz indirecta si es el caso
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
            rgb kd, ks, kt;
            if(escena[i_figura]->es_dielectrico()){

                float fresnel_kr = fresnel(dir_rayo,normal,escena[i_figura]->get_refraccion());
                float fresnel_kt = 1 - fresnel_kr;
                kd.set_values(0,0,0);
                ks.set_values(fresnel_kr,fresnel_kr,fresnel_kr);
                kt.set_values(fresnel_kt,fresnel_kt,fresnel_kt);
            }
            else{
                kd = escena[i_figura]->get_colores_kd();
                ks = escena[i_figura]->get_colores_ks();
                kt = escena[i_figura]->get_colores_kt();
            }

            // Next event estimation, calculamos la luz indirecta de nuestra escena

            rgb luz_indirecta;
            luz_indirecta.set_values(0,0,0);
            if(luces.size() > 0){
                Punto_Vector hit_point = punto_figura + normal * 0.02;
                for(int i = 0; i < luces.size(); i++){
                    Punto_Vector light_point = luces[i]->get_punto();
                    Punto_Vector light_dir = light_point - hit_point;
                    light_dir = light_dir.normalizar(); 
                    float distancia_luz = sqrt( pow(hit_point.x - light_point.x,2) +
                                                            pow(hit_point.y - light_point.y,2) +
                                                            pow(hit_point.z - light_point.z,2));
                    for(int j = 0; j < escena.size() ; j++){
                        float t_valor, distancia;
                        bool choca = escena[j]->get_interseccion(hit_point,light_dir, distancia, t_valor);
                        // Si choca comprobamos que la distancia es menor a la menor de todas las distancias
                        if(choca){
                            if(distancia_luz < distancia){
                                luz_indirecta.set_red(luz_indirecta.get_red() + luces[i]->get_luz().get_red() * luces[i]->get_power() * kd.get_red() * max((normal^light_dir),0.0) / (distancia_luz*distancia_luz*PI));
                                luz_indirecta.set_green(luz_indirecta.get_green() + luces[i]->get_luz().get_green() * luces[i]->get_power() * kd.get_green() * max((normal^light_dir),0.0) / (distancia_luz*distancia_luz*PI));
                                luz_indirecta.set_blue(luz_indirecta.get_blue() + luces[i]->get_luz().get_blue() * luces[i]->get_power() * kd.get_blue() * max((normal^light_dir),0.0) / (distancia_luz*distancia_luz*PI));
                            }
                        }
                    }
                }
            }
            // Calculamos el valor del material
            double pd = kd.get_max();
            double ps = ks.get_max();
            double pt = kt.get_max();
            if((pd + ps + pt) > 0.95 ){ // Si los valores suman mas de 0.95, los normalizamos para que haya una probabilidad de absorcion
                pd = 0.95 * pd / (pd + ps + pt);
                ps = 0.95 * ps / (pd + ps + pt);
                pt = 0.95 * pt / (pd + ps + pt);  
            }

            double num_aleatorio = (double)rand() / RAND_MAX;

            if(num_aleatorio < ps){
                // Si es material especular
                dir_rayo = dir_rayo.normalizar();
                Punto_Vector dir_rayo_inv = Punto_Vector(-dir_rayo.x,-dir_rayo.y,-dir_rayo.z,dir_rayo.valor);
                Punto_Vector direccion_reflejada = (2 * (normal ^ dir_rayo_inv) * normal + dir_rayo);

                r.origen = punto_figura + normal * 0.002;
                r.direccion = direccion_reflejada.normalizar();

                std::tuple<int, int, int> siguiente = funcionL(escena,r,luces);

                float red,green,blue;
                red = get<0>(siguiente) * ks.get_red() / ps;
                green = get<1>(siguiente) * ks.get_green() / ps;
                blue = get<2>(siguiente) * ks.get_blue() / ps;

                red = luz_indirecta.get_red() + red;
                green = luz_indirecta.get_green() + green;
                blue = luz_indirecta.get_blue() + blue;
                return std::make_tuple(red, green, blue);
            }
            else if(num_aleatorio < (ps + pt)){
                double cos = r.direccion^normal;
                bool fuera = cos < 0;
                if(!fuera){
                    r.origen = punto_figura + normal * 0.02;
                }
                else{
                    Punto_Vector cambio = Punto_Vector(-normal.x,-normal.y,-normal.z,0); // Negamos la normal para que sea hacia adentro
                    r.origen = punto_figura + cambio * 0.02;
                }
                r.direccion = refraccion(dir_rayo, normal, escena[i_figura]->get_refraccion());
                r.direccion = r.direccion.normalizar();

                std::tuple<int, int, int> siguiente = funcionL(escena,r,luces);

                float red,green,blue;

                red = get<0>(siguiente) * kt.get_red() / pt;
                green = get<1>(siguiente) * kt.get_green() / pt;
                blue = get<2>(siguiente) * kt.get_blue() / pt;

                red = luz_indirecta.get_red() + red;
                green = luz_indirecta.get_green() + green;
                blue = luz_indirecta.get_blue() + blue;
                return std::make_tuple(red, green, blue);
            }
            else if( num_aleatorio < (ps+pt+pd)){
            

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

                float valor_w_normal = abs(normal^direc_global);
                r.origen = punto_figura + normal * 0.002;
                r.direccion = direc_global.normalizar();
            
                // el rayo sigue
                std::tuple<int, int, int> siguiente = funcionL(escena,r,luces);

                float red,green,blue;
                red = get<0>(siguiente) * kd.get_red() / pd;
                green = get<1>(siguiente) * kd.get_green() / pd;
                blue = get<2>(siguiente) * kd.get_blue() / pd;

                red = luz_indirecta.get_red() + red;
                green = luz_indirecta.get_green() + green;
                blue = luz_indirecta.get_blue() + blue;
                return std::make_tuple(red, green, blue);
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

                get<0>(totalPixel) = get<0>(totalPixel) + get<0>(colorPixel) * 255;
                get<1>(totalPixel) = get<1>(totalPixel) + get<1>(colorPixel) * 255;
                get<2>(totalPixel) = get<2>(totalPixel) + get<2>(colorPixel) * 255;
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

        geo.push_back(new Esfera(Punto_Vector(40,-40,60,1), 10, azul,azul,nada,1.5, false,false,0));
    }
    else{
        // Escena sin next event estimation
        geo.push_back(new Plano(Punto_Vector(-50,0,50,1),Punto_Vector(0,0,1,0),Punto_Vector(0,1,0,0),gris,nada,nada,0, false,false,4 ));
        geo.push_back(new Plano(Punto_Vector(50,0,50,1),Punto_Vector(0,0,1,0),Punto_Vector(0,1,0,0),gris,nada,nada,0, false,false,0 ));

        geo.push_back(new Plano(Punto_Vector(0,50,50,1),Punto_Vector(0,0,1,0),Punto_Vector(1,0,0,0),verde,nada,nada,0, false,false,0 ));
        geo.push_back(new Plano(Punto_Vector(0,-50,50,1),Punto_Vector(0,0,-1,0),Punto_Vector(1,0,0,0),rojo,nada,nada,0, false,false,0 ));

        geo.push_back(new Plano(Punto_Vector(0,0,100,1),Punto_Vector(0,1,0,0),Punto_Vector(1,0,0,0),gris,nada,nada,0, false,false,0 ));
        geo.push_back(new Plano(Punto_Vector(0,0,-100,1),Punto_Vector(0,1,0,0),Punto_Vector(1,0,0,0),gris,nada,nada,0, false,false,0 ));

        geo.push_back(new Esfera(Punto_Vector(40,-40,60,1), 10, nada,nada,blanco,1.5, false,false,0));

        vector_luces.push_back(new Punto_Luz(Punto_Vector(0,0,50,1),blanco,10000));
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
                out << (int)(imagen[i*resolution*3 + j*3 + k]) << " " ;
            }
        }
        out << endl;
    }

    chrono::steady_clock::time_point end = chrono::steady_clock::now();

    std::cout << "Tiempo: " << (chrono::duration_cast<std::chrono::microseconds>(end - begin).count())/1000000.0 << " segundos" << std::endl;
    return 0;
}