#include <iostream>
#include <fstream>
#include <istream>
#include <string>
#include <cstring>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <cmath>

using namespace std;

int main() {

    string cadena;

    fstream fichero;
    fichero.open("imagen.ppm");

    string formato;
    int width, height;
    int res_color;
    float MAX = 1;

    int i = 0;
    while(!fichero.eof()) {
        getline(fichero,cadena);

        if (cadena.at(0)=='#'){
            if(cadena.substr(1,3) == "MAX"){
                string aux = cadena.substr(5,cadena.length());
                //MAX = stof(aux);
                std::istringstream iss(aux);
                iss >> MAX;
            }
            continue;
        }

        if (i == 0) {
            formato = cadena;
            i++;
        } 
        else if (i == 1) {
            std::istringstream iss(cadena);
            iss >> width;
            iss >> height;
            i++;
        } 
        else if (i == 2) {
            std::istringstream iss(cadena);
            iss >> res_color;
            break;
        }
    }
    /*cout << formato << endl;
    cout << width << endl;
    cout << height << endl;
    cout << res_color << endl;
    cout << MAX << endl;*/

    vector<float> imagen (height*width*3);
    float max = 0, min = res_color;

    for (int i = 0; i < height; i++) {
        getline(fichero,cadena);
        //cout << cadena << endl;
        std::istringstream iss(cadena);
        float auxiliar_s;
        for (int j = 0; j < width; j++) {
            iss >> auxiliar_s;
            imagen[i*width*3 + j*3] = (MAX * auxiliar_s) / res_color;
            if ((MAX * auxiliar_s) / res_color < min) {
                min = (MAX * auxiliar_s) / res_color;
            } else if ((MAX * auxiliar_s) / res_color > max){
                max = (MAX * auxiliar_s) / res_color;
            }
            iss >> auxiliar_s;
            imagen[i*width*3 + j*3 + 1] = (MAX * auxiliar_s) / res_color;
            if ((MAX * auxiliar_s) / res_color < min) {
                min = (MAX * auxiliar_s) / res_color;
            } else if ((MAX * auxiliar_s) / res_color > max){
                max = (MAX * auxiliar_s) / res_color;
            }
            iss >> auxiliar_s;
            imagen[i*width*3 + j*3 + 2] = (MAX * auxiliar_s) / res_color;
            if ((MAX * auxiliar_s) / res_color < min) {
                min = (MAX * auxiliar_s) / res_color;
            } else if ((MAX * auxiliar_s) / res_color > max){
                max = (MAX * auxiliar_s) / res_color;
            }
        }
    }

    // TODO: tone mapping //////////////////

    // clamping
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (imagen[i*width*3 + j*3] > 1) {
                imagen[i*width*3 + j*3] = 1;
            }
            if (imagen[i*width*3 + j*3 + 1] > 1) {
                imagen[i*width*3 + j*3 + 1] = 1;
            }
            if (imagen[i*width*3 + j*3 + 2] > 1) {
                imagen[i*width*3 + j*3 + 2] = 1;
            }
        }
    }

    //eq
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            imagen[i*width*3 + j*3] = imagen[i*width*3 + j*3] / max * 1;
            imagen[i*width*3 + j*3 + 1] = imagen[i*width*3 + j*3 + 1] / max * 1;
            imagen[i*width*3 + j*3 + 2] = imagen[i*width*3 + j*3 + 2] / max * 1;
        }
    }

    //eq and clamp
    float V = 1;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (imagen[i*width*3 + j*3] > V) {
                imagen[i*width*3 + j*3] = V;
            } else {
                imagen[i*width*3 + j*3] = imagen[i*width*3 + j*3] / V * 1;
            }
            if (imagen[i*width*3 + j*3 + 1] > V) {
                imagen[i*width*3 + j*3 + 1] = V;
            } else {
                imagen[i*width*3 + j*3 + 1] = imagen[i*width*3 + j*3 + 1] / V * 1;
            }
            if (imagen[i*width*3 + j*3 + 2] > V) {
                imagen[i*width*3 + j*3 + 2] = V;
            } else {
                imagen[i*width*3 + j*3 + 2] = imagen[i*width*3 + j*3 + 2] / V * 1;
            }
        }
    }

    // gamma
    float G = 1;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            imagen[i*width*3 + j*3] = imagen[i*width*3 + j*3] / max * 1;
            imagen[i*width*3 + j*3] = pow(imagen[i*width*3 + j*3] / max , (1/G)) * max;

            imagen[i*width*3 + j*3 + 1] = imagen[i*width*3 + j*3 + 1] / max * 1;
            imagen[i*width*3 + j*3 + 1] = pow(imagen[i*width*3 + j*3 + 1] / max , (1/G)) * max;

            imagen[i*width*3 + j*3 + 2] = imagen[i*width*3 + j*3 + 2] / max * 1;
            imagen[i*width*3 + j*3 + 2] = pow(imagen[i*width*3 + j*3 + 2] / max , (1/G)) * max;
        }
    }

    // gamma y clamp
    float G = 1;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (imagen[i*width*3 + j*3] > V) {
                imagen[i*width*3 + j*3] = V;
            } else {
                imagen[i*width*3 + j*3] = imagen[i*width*3 + j*3] / V * 1;
                imagen[i*width*3 + j*3] = pow(imagen[i*width*3 + j*3] / V , (1/G)) * V;
            }

            if (imagen[i*width*3 + j*3 + 1] > V) {
                imagen[i*width*3 + j*3 + 1] = V;
            } else {
                imagen[i*width*3 + j*3 + 1] = imagen[i*width*3 + j*3 + 1] / V * 1;
                imagen[i*width*3 + j*3 + 1] = pow(imagen[i*width*3 + j*3 + 1] / V , (1/G)) * V;
            }

            if (imagen[i*width*3 + j*3 + 2] > V) {
                imagen[i*width*3 + j*3 + 2] = V;
            } else {
                imagen[i*width*3 + j*3 + 2] = imagen[i*width*3 + j*3 + 2] / V * 1;
                imagen[i*width*3 + j*3 + 2] = pow(imagen[i*width*3 + j*3 + 2] / V , (1/G)) * V;
            }
            
        }
    }


    ////////////////////////////////////////

    ofstream out;
    out.open("practica_2.ppm");
    out << formato << endl;
    out << width << " " << height << endl;
    out << res_color << endl;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            
            out << imagen[i*width*3 + j*3] * res_color / MAX << " " 
            << imagen[i*width*3 + j*3 + 1] * res_color / MAX << " "
            << imagen[i*width*3 + j*3 + 2] * res_color / MAX << " ";
        }
        out << endl;
    }

    fichero.close();

    return 0;
}