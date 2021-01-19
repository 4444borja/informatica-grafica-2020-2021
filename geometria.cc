#pragma once
#include <iostream>
#include "rgb.cc"
using namespace std;

class Geometria {
    public:
        Geometria(rgb Kd, rgb Ks, bool light){
            kd.set_values(Kd.get_red(), Kd.get_green(), Kd.get_blue());
            ks.set_values(Ks.get_red(), Ks.get_green(), Ks.get_blue());
            is_light = light;
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

        virtual float get_interseccion(Punto_Vector origen_rayo, Punto_Vector dir_rayo, Punto_Vector &normal) { 
            return 0;
        }
        rgb get_colores_kd(){
            return kd;
        } 
        rgb get_colores_ks(){
            return ks;
        }

        bool es_luz(){
            return is_light;
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
    protected:
        rgb kd;
        rgb ks;
        bool is_light;

};