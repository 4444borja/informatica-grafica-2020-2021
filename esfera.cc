#pragma once

#include "geometria.cc"
#include <iostream>
using namespace std;

class Esfera : public Geometria {
   public:
        Esfera(Punto_Vector centro, double radio, int R, int G, int B) :
            Geometria(R, G, B), centro_esfera(centro), radio_esfera (radio) { }
        
        int get_radio(){
            return radio_esfera;
        }
        Punto_Vector get_centro(){
            return centro_esfera;
        }

        double get_interseccion(Punto_Vector origen_rayo, Punto_Vector dir_rayo) { 
            double interseccion;
            double a = pow(dir_rayo.modulo(), 2);
            double b = dir_rayo ^ (centro_esfera - origen_rayo) * 2;
            double c = pow((centro_esfera - origen_rayo).modulo(), 2) - pow(radio_esfera, 2);
            double delta = b*b - 4*a*c;
            if (delta > 0){
                float x1 = abs((-b+sqrt(delta))/2*a);
                float x2 = abs((-b-sqrt(delta))/2*a);
                if(x1 < x2){
                    interseccion = ((double)x1);
                }
                else{
                    interseccion = ((double)x2);
                }
            }
            else if(delta == 0){
                interseccion = abs((-b/(2*a)));
            }
            else{
                interseccion = -1;
            }
            return interseccion;
        }


        Punto_Vector get_normal(){
            cout << "SE HA INVOCADO LA FUNCIÓN get_normal EN UN OBJETO DE TIPO ESFERA" << endl;
            exit(1);
        }




    protected:
        Punto_Vector centro_esfera;
        double radio_esfera;
};