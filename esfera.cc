#pragma once

#include "geometria.cc"
#include <iostream>
using namespace std;

class Esfera : public Geometria {
   public:
        Esfera(Punto_Vector centro, double radio, rgb Kd, rgb Ks, bool is_light) :
            Geometria(Kd, Ks, is_light), centro_esfera(centro), radio_esfera (radio) { }
        
        int get_radio(){
            return radio_esfera;
        }
        Punto_Vector get_centro(){
            return centro_esfera;
        }
        bool solveQuadratic(const float &a, const float &b, const float &c, float &x0, float &x1) 
        { 
            float discr = b * b - 4 * a * c; 
            if (discr < 0) return false; 
            else if (discr == 0) x0 = x1 = - 0.5 * b / a; 
            else { 
                float q = (b > 0) ? 
                    -0.5 * (b + sqrt(discr)) : 
                    -0.5 * (b - sqrt(discr)); 
                x0 = q / a; 
                x1 = c / q; 
            } 
            if (x0 > x1) std::swap(x0, x1); 
        
            return true; 
        }

        float get_interseccion(Punto_Vector origen_rayo, Punto_Vector dir_rayo, Punto_Vector &normal) { 
            float t;
            float a = pow(dir_rayo.modulo(), 2);
            float b = 2 * (dir_rayo ^ (origen_rayo - centro_esfera));
            float c = pow((origen_rayo - centro_esfera).modulo(), 2) - pow(radio_esfera, 2);
            float x0,x1;
            if(!solveQuadratic(a,b,c,x0,x1)) return -1;
            if(x0 > x1) swap(x0,x1);
            if(x0 < 0){
                x0 = x1;
                if(x0 < 0) return -1;
            }
            t = x0;
            normal = (origen_rayo + dir_rayo*t) - centro_esfera;
            return t;
        }


        Punto_Vector get_normal(){
            cout << "SE HA INVOCADO LA FUNCIÓN get_normal EN UN OBJETO DE TIPO ESFERA" << endl;
            exit(1);
        }




    protected:
        Punto_Vector centro_esfera;
        double radio_esfera;
};