#pragma once
#include <iostream>
#include "rgb.cc"
using namespace std;

class Geometria {
    public:
        Geometria(int R, int G, int B){
            colores.set_values(R, G, B);
        }

        virtual double get_interseccion(Punto_Vector origen_rayo, Punto_Vector dir_rayo) { 
            return 0;
        }
        rgb get_colores(){
            return colores;
        } 
    protected:
        rgb colores;

};