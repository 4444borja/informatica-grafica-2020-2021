
#include <iostream>
#include "rgb.cc"
using namespace std;

class geometria {
    public:
        void set_color (int R, int G, int B){
            colores.set_values(R, G, B);
        }
        void set_color (rgb los_colores){
            colores = los_colores;
        }

        rgb get_colores(){
            return colores;
        } 
    protected:
        rgb colores;

};

// Derived classes
class esfera : public geometria {
   public:
        void set_values(Punto_Vector centro, double radio){
            centro_esfera = centro;
            radio_esfera = radio;
        }
        void set_radio(double radio){
            radio_esfera = radio;
        }
        void set_centro(Punto_Vector centro){
            centro_esfera = centro;
        }
        
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
                float x1 = (-b+sqrt(delta))/2*a;
                float x2 = (-b-sqrt(delta))/2*a;
                if(x1 < x2){
                    interseccion = -((double)x1);
                }
                else{
                    interseccion = -((double)x2);
                }
            }
            else if(delta == 0){
                interseccion = -(-b/(2*a));
            }
            else{
                interseccion = -1;
            }
            return interseccion;
        }




    protected:
        Punto_Vector centro_esfera;
        double radio_esfera;
};