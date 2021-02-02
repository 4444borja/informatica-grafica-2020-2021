#pragma once

#include "geometria.cc"
#include <iostream>
using namespace std;

class Punto_Luz{
   public:
        Punto_Luz(Punto_Vector punto_origen, rgb luz_origen){
            luz.set_values(luz_origen.get_red(), luz_origen.get_green(), luz_origen.get_blue());
            punto = punto_origen;
         }
        
        Punto_Vector get_punto(){
            return punto;
        }
        rgb get_luz(){
            return luz;
        }
       
    protected:
        Punto_Vector punto;
        rgb luz;
};