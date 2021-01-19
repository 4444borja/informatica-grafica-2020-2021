#pragma once

#include "geometria.cc"
#include <iostream>
using namespace std;

class Plano : public Geometria {
   public:
        Plano(Punto_Vector normal, double distancia, rgb Kd, rgb Ks, bool is_light) :
        Geometria(Kd, Ks, is_light)
        {
            distancia_origen = distancia; 
            normal_plano = normal.normalizar();
            punto_plano = Punto_Vector(normal_plano.x,normal_plano.y, normal_plano.z,1);
         }

        Punto_Vector get_normal(){
            return normal_plano;
        }
        double get_distancia(){
            return distancia_origen;
        }

        float get_interseccion(Punto_Vector origen_rayo, Punto_Vector dir_rayo, Punto_Vector &normal) { 
            float interseccion;
            float denom = normal_plano ^ dir_rayo ;
            if (fabs(denom) < 1e-6 ){ // Es paralelo
                interseccion = -1;
            }
            else{
                Punto_Vector p_origen = punto_plano - origen_rayo;
                interseccion = (p_origen ^ normal_plano) / denom;
                if(denom < 0) normal = normal_plano;
                else normal = Punto_Vector( -normal_plano.x, -normal_plano.y, -normal_plano.z, 0);
            }
            return interseccion;
        }

        Punto_Vector get_centro(){
            cout << "SE HA INVOCADO LA FUNCIÓN get_centro EN UN OBJETO DE TIPO PLANO" << endl;
            exit(1);
        }

    protected:
        Punto_Vector punto_plano;
        double distancia_origen;
        Punto_Vector normal_plano;
};