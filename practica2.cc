#include <iostream>
#include <fstream>
#include <istream>
#include <string>
#include <cstring>
#include <sstream>
#include <cstdlib>
#include <vector>

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
                MAX = stof(aux);
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
    cout << formato << endl;
    cout << width << endl;
    cout << height << endl;
    cout << res_color << endl;

    vector<float> imagen (height*width*3);

    for (int i = 0; i < height; i++) {
        getline(fichero,cadena);
        //cout << cadena << endl;
        std::istringstream iss(cadena);
        float auxiliar_s;
        for (int j = 0; j < width; j++) {
            iss >> auxiliar_s;
            imagen[i*width*3 + j*3] = (MAX * auxiliar_s) / res_color;
            iss >> auxiliar_s;
            imagen[i*width*3 + j*3 + 1] = (MAX * auxiliar_s) / res_color;
            iss >> auxiliar_s;
            imagen[i*width*3 + j*3 + 2] = (MAX * auxiliar_s) / res_color;
        }
    }


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