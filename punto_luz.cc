#pragma once

#include "geometria.cc"
#include <iostream>
using namespace std;

class Punto_Luz{
   public:
        Punto_Luz(Punto_Vector punto_origen, rgb luz_origen, int luz_power){
            luz.set_values(luz_origen.get_red(), luz_origen.get_green(), luz_origen.get_blue());
            punto = punto_origen;
            power = luz_power;
         }
        
        Punto_Vector get_punto(){
            return punto;
        }
        rgb get_luz(){
            return luz;
        }

        int get_power(){
            return power;
        }
       
    protected:
        Punto_Vector punto;
        rgb luz;
        int power;
};