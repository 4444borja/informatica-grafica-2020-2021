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

vector<float> tone_mapping(int opcion, vector<float> imagen, int width, int height, float min, float max, float V, float gamma, double MAX_R){

    switch (opcion){
        case 1:
            // clamping
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    for (int k = 0 ; k < 3 ; k ++) {
                        if (imagen[i*width*3 + j*3 + k] > MAX_R) {
                            imagen[i*width*3 + j*3 + k] = MAX_R;
                        }
                    }
                }
            }
            break;
        case 2:
            //eq
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    for (int k = 0 ; k < 3 ; k++){
                        imagen[i*width*3 + j*3 + k] = (imagen[i*width*3 + j*3 + k] - min) / max * MAX_R;
                    }
                }
            }
            break;
        case 3:
            //eq and clamp
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    for ( int k = 0 ; k < 3 ; k++){
                        if (imagen[i*width*3 + j*3 + k] > V) {
                            //clamping
                            imagen[i*width*3 + j*3 + k] = V;
                        } else {
                            //eq
                            imagen[i*width*3 + j*3 + k] = (imagen[i*width*3 + j*3 + k] - min) / V * MAX_R;
                        }
                    }
                }
            }
            break;
        case 4:
            // gamma
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    for( int k = 0 ; k < 3 ; k++){
                        // eq
                        imagen[i*width*3 + j*3 + k] = (imagen[i*width*3 + j*3 + k] - min) / max * MAX_R;
                        // gamma
                         imagen[i*width*3 + j*3 + k] = pow(imagen[i*width*3 + j*3 + k] / max , gamma) * max;
                    }
                }
            }
            break;
        case 5:
            // gamma y clamp
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    for (int k = 0; k < 3; k++){
                        if (imagen[i*width*3 + j*3 + k] > V) {
                            // clamping
                            imagen[i*width*3 + j*3 + k] = V;
                        } else {
                            // eq
                            imagen[i*width*3 + j*3 + k] = (imagen[i*width*3 + j*3 + k] - min) / V * MAX_R;
                            // gamma
                            imagen[i*width*3 + j*3 + k] = pow(imagen[i*width*3 + j*3 + k] / V , gamma) * V;
                        }
                    }  
                }
            }
            break;
        default:
            cout << "Opciones:" << endl;
            cout << "\t1: Clamping" << endl;
            cout << "\t2: EQ" << endl;
            cout << "\t3: Clamping y EQ (NECESARIO ARGUMENTO V)" << endl;
            cout << "\t4: Curva Gamma (NECESARIO ARGUMENTO gamma)" << endl;
            cout << "\t5: Clamping y curva Gamma (NESESARIOS ARGUMENTOS V y gamma)" << endl; 
    }

    return imagen;
}

int main() {

    string cadena;

    fstream fichero;
    //fichero.open("imagen.ppm");
    fichero.open("mpi_atrium_3.ppm");

    string formato;
    int width, height;
    int res_color;
    double MAX = 1;

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
            for( int k = 0 ; k < 3 ; k++){
                iss >> auxiliar_s;
                imagen[i*width*3 + j*3 + k] = (MAX * auxiliar_s) / res_color;
                if ((MAX * auxiliar_s) / res_color < min) {
                    min = (MAX * auxiliar_s) / res_color;
                }
                else if ((MAX * auxiliar_s) / res_color > max){
                    max = (MAX * auxiliar_s) / res_color;
                }
            }
        }
    }

    // tone mapping //////////////////
    cout << "Opciones:" << endl;
            cout << "\t1: Clamping" << endl;
            cout << "\t2: EQ" << endl;
            cout << "\t3: Clamping y EQ (NECESARIO ARGUMENTO V)" << endl;
            cout << "\t4: Curva Gamma (NECESARIO ARGUMENTO gamma)" << endl;
            cout << "\t5: Clamping y curva Gamma (NESESARIOS ARGUMENTOS V y gamma)" << endl; 

    
    imagen = tone_mapping(5, imagen, width, height, min, max, 10,2,MAX);
    


    ////////////////////////////////////////

    ofstream out;
    out.open("practica_2_3_2.ppm");
    out << formato << endl;
    out << width << " " << height << endl;
    out << res_color << endl;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            for (int k = 0 ; k < 3 ; k++){
                out << (int)(imagen[i*width*3 + j*3 + k] * res_color / MAX) << " " ;
            }
        }
        out << endl;
    }

    fichero.close();

    return 0;
}