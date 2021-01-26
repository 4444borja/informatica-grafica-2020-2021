#pragma once

#include "geometria.cc"
#include <iostream>
using namespace std;

class Plano : public Geometria {
   public:
        Plano(Punto_Vector punto, Punto_Vector v, Punto_Vector u, rgb Kd, rgb Ks, rgb Kt, double i_ref, bool dielectric, bool is_light, int p_light) :
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


        float get_interseccion(Punto_Vector origen_rayo, Punto_Vector dir_rayo, Punto_Vector& normal) { 
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

        bool es_denom_neg(Punto_Vector dir_rayo){
            float denom = normal_plano ^ dir_rayo;
            if(denom < 0){
                return true;
            }
            else{
                return false;
            }
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