#pragma once
#include <iostream>
#include "rgb.cc"
using namespace std;

class Geometria {
    public:
        Geometria(rgb Kd, rgb Ks, rgb Kt, bool light, int p_light, float i_ref, bool dielectric){
            kd.set_values(Kd.get_red(), Kd.get_green(), Kd.get_blue());
            ks.set_values(Ks.get_red(), Ks.get_green(), Ks.get_blue());
            kt.set_values(Kt.get_red(), Kt.get_green(), Kt.get_blue());
            ind_ref = i_ref;
            is_light = light;
            power_light = p_light;
            is_dielectric = dielectric;
        }

        virtual Punto_Vector get_centro(){
            cout << "SE HA INTENTADO LLAMAR A get_centro EN UN OBJETO DE TIPO GEOMETRIA" << endl;
            exit(1);
        }
        virtual Punto_Vector get_normal() {
            cout << "SE HA INTENTADO LLAMAR A get_normal EN UN OBJETO DE TIPO GEOMETRIA" << endl;
            exit(1);
        }

        virtual bool es_denom_neg(Punto_Vector dir_rayo) {
            cout << "SE HA INTENTADO LLAMAR A es_denom_neg EN UN OBJETO DE TIPO GEOMETRIA" << endl;
            exit(1);
        }

        virtual bool get_interseccion(Punto_Vector origen_rayo, Punto_Vector dir_rayo, float &distancia, float &t_value) { 
            return 0;
        }

        virtual double distancia_de_refraccion(){
            return 0;
        }
        
        rgb get_colores_kd(){
            return kd;
        } 
        rgb get_colores_ks(){
            return ks;
        }

        rgb get_colores_kt(){
            return kt;
        }
        float get_refraccion(){
            return ind_ref;
        }
        
        bool es_luz(){
            return is_light;
        }

        int get_p_light(){
            return power_light;
        }

        bool es_dielectrico(){
            return is_dielectric;
        }
        double get_max_Kd(){
            if(kd.get_red() > kd.get_blue() && kd.get_red() > kd.get_green()){
                return kd.get_red();
            }
            else if(kd.get_green() > kd.get_red() && kd.get_green() > kd.get_blue()){
                return kd.get_green();
            }
            else{
                return kd.get_blue();
            }
        }

        double get_max_Ks(){
            if(ks.get_red() > ks.get_blue() && ks.get_red() > ks.get_green()){
                return ks.get_red();
            }
            else if(ks.get_green() > ks.get_red() && ks.get_green() > ks.get_blue()){
                return ks.get_green();
            }
            else{
                return ks.get_blue();
            }
        }

        double get_max_Kt(){
            if(kt.get_red() > kt.get_blue() && kt.get_red() > kt.get_green()){
                return kt.get_red();
            }
            else if(kt.get_green() > kt.get_red() && kt.get_green() > kt.get_blue()){
                return kt.get_green();
            }
            else{
                return kt.get_blue();
            }
        }
    protected:
        rgb kd;
        rgb ks;
        rgb kt;
        bool is_light;
        int power_light;
        float ind_ref;
        bool is_dielectric;
};