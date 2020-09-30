#include <iostream>
using namespace std;

struct Punto_Vector
{
    double x, y, z;
    int valor;

    Punto_Vector (double x, double y, double z, int valor) {
        if (valor == 0 || valor == 1) {
            this -> x = x;
            this -> y = y;
            this -> z = z;
            this -> valor = valor;
        }
        else{
            cerr << "El cuarto argumento solo admite 0 (direcciones) y 1 (puntos)" << endl;
            exit(1);
        }
    }

    double modulo (Punto_Vector p) {
        if (p.valor == 1) {
            // es un punto
            cerr << "No se puede calcular el modulo de un punto" << endl;
            exit(1);
        }
        return sqrt(pow(p.x,2) + pow(p.y,2) + pow(p.z,2));
    }
};

Punto_Vector operator+(const Punto_Vector &p1, const Punto_Vector &p2)
{
    if (p1.valor == 1 && p2.valor == 1){
        // si ambos son puntos
        cerr << "Sumar dos puntos no tiene sentido geometrico" << endl;
        exit(1);
    }
    int valor = 1;
    if (p1.valor == 0 && p2.valor == 0) {
        // si ambos son direcciones
        valor = 0;
    }
    Punto_Vector p(p1.x + p2.x, p1.y + p2.y, p1.z + p2.z, valor);
    return p;
}

Punto_Vector operator-(const Punto_Vector &p1, const Punto_Vector &p2)
{
    if (p1.valor == 1 && p2.valor == 1){
        // si ambos son puntos
        cerr << "Restar dos puntos no tiene sentido geometrico" << endl;
        exit(1);
    }
    int valor = 1;
    if (p1.valor == 1 && p2.valor == 0) {
        // p1 punto y p2 direccion
        cerr << "No se puede restar un punto menos una direccion" << endl;
        exit(1);
    }
    Punto_Vector p(p1.x - p2.x, p1.y - p2.y, p1.z - p2.z, 0);
    return p;
}

Punto_Vector operator*(const Punto_Vector &p1, const double &s)
{
    if (p1.valor == 1){
        // si es punto
        cerr << "No se puede multiplicar un punto por un escalar" << endl;
        exit(1);
    }
    Punto_Vector p(p1.x * s, p1.y * s, p1.z * s, 1);
    return p;
}

Punto_Vector operator/(const Punto_Vector &p1, const double &s)
{
    if (p1.valor == 0){
        // si es direccion
        cerr << "No se puede dividir una direccion por un escalar" << endl;
        exit(1);
    }
    Punto_Vector p(p1.x * s, p1.y * s, p1.z * s, 1);
    return p;
}

double operator·(const Punto_Vector &p1, const Punto_Vector &p2)
{
    if (p1.valor == 1 || p2.valor == 1){
        // si alguno es punto
        cerr << "Dot product con puntos no tiene sentido geometrico" << endl;
        exit(1);
    }

    return p1.x*p2.x + p1.y*p2.y + p1.z*p2.z;
}

Punto_Vector operatorx(const Punto_Vector &p1, const Punto_Vector &p2)
{
    if (p1.valor == 1 || p2.valor == 1){
        // si alguno es punto
        cerr << "Producto vectorial con puntos no tiene sentido geometrico" << endl;
        exit(1);
    }
    double x = (p1.y*p2.z - p1.z*p2.y);
    double y = (p1.z*p2.x - p1.x*p2.z);
    double z = (p1.x*p2.y - p1.y*p2.x);
    Punto_Vector p( x, y, z, 0);
    return p;
}

int main() {
    Punto_Vector A(2,3,4,1);
    Punto_Vector B(1,2,3,0);

    Punto_Vector C = A+B;
    Punto_Vector D = A*5;

    cout << A.x << " " << A.y << " " << A.z << " " << A.valor << endl;
    cout << B.x << " " << B.y << " " << B.z << " " << B.valor << endl;
    cout << C.x << " " << C.y << " " << C.z << " " << C.valor << endl;
    cout << D.x << " " << D.y << " " << D.z << " " << D.valor << endl;

    return 0;
}