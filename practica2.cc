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

    int i = 0;
    while(!fichero.eof()) {
        getline(fichero,cadena);

        if (cadena.at(0)=='#') continue;

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

    vector<int> imagen (height*width*3);

    for (int i = 0; i < height; i++) {
        getline(fichero,cadena);
        //cout << cadena << endl;
        std::istringstream iss(cadena);
        for (int j = 0; j < width; j++) {
            iss >> imagen[i*width*3 + j*3];
            iss >> imagen[i*width*3 + j*3 + 1];
            iss >> imagen[i*width*3 + j*3 + 2];
        }
    }

    fichero.close();

    return 0;
}