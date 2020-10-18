#include <iostream>
#include "punto_y_vector.cc"
#include "matrix.cc"
#include <math.h> // Solo se usará para sin o cos
#define PI 3.14159265
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



    cout << "**************" << endl;
    cout << "* 3: Planetary Station *" << endl;
    cout << "**************" << endl;
    // Introduccion de datos
    double firstAxis[3] = {0,0,0};
    double secondAxis[3] = {0,0,0};
    double thirdAxis[3] = {0,0,0};
    double inclination = 0;
    double azimuth = 0;
    cout << endl << "Enter First Axis, Longitude Tangent Direction: ";
    cin >> firstAxis[0];
    cin >> firstAxis[1];
    cin >> firstAxis[2];
    cout << "Enter Second Axis, Latitude Tangent Direction  ";
    cin >> secondAxis[0];
    cin >> secondAxis[1];
    cin >> secondAxis[2];
    cout << "Enter Third Axis, Surface Normal ";
    cin >> thirdAxis[0];
    cin >> thirdAxis[1];
    cin >> thirdAxis[2];
    cout << "Enter Inclination of the station ";
    cin >> inclination;
    cout << "Enter Azimuth of the station";
    cin >> azimuth;

    double origen2[3] = {0,0,0};
    Matrix CambioBase(firstAxis, secondAxis, thirdAxis, origen2);
    // Realizamos en giro conforme al ángulo Y, que es el vector que va de un polo a otro.
    double vectorIncl_1[4] = {cos(inclination),0,-sin(inclination),0};
    double vectorIncl_2[4] = {0,1,0,0};
    double vectorIncl_3[4] = {sin(inclination),0,cos(inclination),0};
    double vectorIncl_4[4] = {0,0,0,1};
    Matrix matrixIncl(vectorIncl_1,vectorIncl_2,vectorIncl_3,vectorIncl_4);
    // Realizamos en giro conforme al ángulo X, que es el vector que va desde el centro del planeta hasta el limite del mismo y es perpendicular
    // al vector Y
    double vectorAzim_1[4] = {1,0,0,0};
    double vectorAzim_2[4] = {0,cos(azimuth),sin(azimuth),0};
    double vectorAzim_3[4] = {0,-sin(azimuth),cos(azimuth),0};
    double vectorAzim_4[4] = {0,0,0,1};
    Matrix matrixAzim(vectorAzim_1,vectorAzim_2,vectorAzim_3,vectorAzim_4);
    // Creamos un punto en el polo superior del planeta y realizamos los giros, primero en base a Inclinacion y después en base a Azimuth
    Punto_Vector spaceStation(firstAxis[0],firstAxis[1],firstAxis[2],0);
    spaceStation = matrixIncl*spaceStation;
    spaceStation = matrixAzim*spaceStation;

    // Calculamos la inversa conforme a la matriz original del planeta
    Matrix inversaBase = InverseOfMatrix(CambioBase,4);
    // Las coordenadas globales son la inversa por la estacion espacial
    Punto_Vector spaceStationGlobal = inversaBase*spaceStation;
    return 0;
}