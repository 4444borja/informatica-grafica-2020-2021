#include <iostream>
#include "punto_y_vector.cc"

using namespace std;

int main(){
    cout << "**************" << endl;
    cout << "* 2: Planets *" << endl;
    cout << "**************" << endl;

    int centeraux[3] = {0,0,0};
    int axisaux[3] = {0,0,0};
    int refcityaux[3] = {0,0,0};
    cout << endl << "Enter center: ";
    cin >> centeraux[0];
    cin >> centeraux[1];
    cin >> centeraux[2];
    cout << "Enter axis: ";
    cin >> axisaux[0];
    cin >> axisaux[1];
    cin >> axisaux[2];
    cout << "Enter reference city: ";
    cin >> refcityaux[0];
    cin >> refcityaux[1];
    cin >> refcityaux[2];

    Punto_Vector center(centeraux[0],centeraux[1],centeraux[2],1);
    Punto_Vector axis(axisaux[0],axisaux[1],axisaux[2],0);
    Punto_Vector refcity(refcityaux[0],refcityaux[1],refcityaux[2],1);

    Punto_Vector resta = refcity - center;
    resta.imprimir();
    double modulo_resta = resta.modulo();
    cout << "Modulo " << modulo_resta << endl;
    axis.imprimir();
    double modulo_axis = axis.modulo();
    cout << "Modulo " << modulo_axis << endl;

    if(modulo_resta == modulo_axis/2){
        cout << "Vectores introducidos correctamente" << endl;
    }
    else{
        cout << "Error en los parámetros del planeta" << endl;
    }

    return 0;
}