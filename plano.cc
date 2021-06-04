#pragma once

#include "geometria.cc"
#include <iostream>
using namespace std;

class Plano : public Geometria {
   public:
        Plano(Punto_Vector punto, Punto_Vector v, Punto_Vector u, rgb Kd, rgb Ks, rgb Kt, float i_ref, bool dielectric, bool is_light, int p_light) :
        Geometria(Kd, Ks, Kt, is_light, p_light, i_ref, dielectric)
        {
            vector_u = u.normalizar();
            vector_v = v.normalizar();
            punto_plano = punto;
            normal_plano = operatorx(vector_u, vector_v); 
            normal_plano = normal_plano.normalizar();

        }

        Punto_Vector get_normal(){
            return normal_plano;
        }


        bool get_interseccion(Punto_Vector origen_rayo, Punto_Vector dir_rayo, float &distancia, float &t_value) { 
            float denom = normal_plano ^ dir_rayo ;
            if (fabs(denom) > 0.000001f ){ // No es paralelo
                Punto_Vector p_origen = punto_plano - origen_rayo;
                t_value = (p_origen ^ normal_plano) / denom;
                distancia = t_value*dir_rayo.modulo();
                return (t_value >= 0);
            }
            return false;
        }

        bool es_denom_neg(Punto_Vector dir_rayo){
            float denom = normal_plano ^ dir_rayo;
            return denom < 0;
        }

        Punto_Vector get_centro(){
            cout << "SE HA INVOCADO LA FUNCIÓN get_centro EN UN OBJETO DE TIPO PLANO" << endl;
            exit(1);
        }

        double distancia_de_refraccion(){
            return 0.02;
        }

    protected:
        Punto_Vector punto_plano;
        Punto_Vector vector_v;
        Punto_Vector vector_u;
        Punto_Vector normal_plano;
};