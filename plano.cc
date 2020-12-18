#pragma once

#include "geometria.cc"
#include <iostream>
using namespace std;

class Plano : public Geometria {
   public:
        Plano(Punto_Vector normal, double distancia, int R, int G, int B):
            Geometria(R, G, B), distancia_origen(distancia), normal_plano (normal) { }

        Punto_Vector get_normal(){
            return normal_plano;
        }
        double get_distancia(){
            return distancia_origen;
        }

        double get_interseccion(Punto_Vector origen_rayo, Punto_Vector dir_rayo) { 
            double interseccion;
            double paralelo = dir_rayo ^ normal_plano;
            if (paralelo == 0){ // Es paralelo
                interseccion = -1;
            }
            else{
                interseccion = -(distancia_origen+(origen_rayo^normal_plano))/paralelo;
            }
            return interseccion;
        }

        Punto_Vector get_centro(){
            cout << "SE HA INVOCADO LA FUNCIÓN get_centro EN UN OBJETO DE TIPO PLANO" << endl;
            exit(1);
        }

    protected:
        Punto_Vector normal_plano;
        double distancia_origen;
};