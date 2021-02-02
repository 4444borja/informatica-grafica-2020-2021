#pragma once

#include "geometria.cc"
#include <iostream>
using namespace std;

class Plano_Limit: public Plano {
   public:
        Plano_Limit(Punto_Vector punto, Punto_Vector v, Punto_Vector u, rgb Kd, rgb Ks, rgb Kt, double i_ref, bool dielectric, bool is_light, int p_light) :
        Plano(Punto_Vector punto, Punto_Vector v, Punto_Vector u, rgb Kd, rgb Ks, rgb Kt, double i_ref, bool dielectric, bool is_light, int p_light)
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


        bool get_interseccion_limit(Punto_Vector origen_rayo, Punto_Vector dir_rayo, float &distancia, float &t_value) { 
            if(Plane::get_interseccion(origen_rayo, dir_rayo, distancia,t_value)){

                Punto_vector punto_inter = origen_rayo + dir_rayo*t_value;
                Punto_Vector vector_aux_1 = inter - p;

                double cos_vec_1 = vector_v ^ vector_aux_1 / (vector_v_mod * vector_aux_1.modulo())
                double cos_vec_2 = vector_u ^ vector_aux_2 / (vector_u_mod * vector_aux_2.modulo())
                return true;
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