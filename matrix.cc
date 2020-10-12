
#include<iostream>
#include "punto_y_vector.cc"
using namespace std;


// Declaración:
struct Matrix
{

    float la_matriz[4][4];

    Matrix(float valores[16]){
        for(int i = 0 ;i < 4 ; i++){
            for(int j = 0; j < 4 ; j++){
                la_matriz[i][j] = valores[i*4+j];
            }
        }
    }
    void imprimirMatrix(){  
        for (int i = 0; i < 4; i++) { 
            for (int j = 0; j < 4; j++) { 
                printf("%.3f  ", la_matriz[i][j]);     
            } 
            printf("\n"); 
        }  
    }
};


Matrix operator+(const Matrix &m1 , const Matrix &m2){
    float lista_res[16];
    for(int i = 0 ;i < 4 ; i++){
        for(int j = 0; j < 4 ; j++){
            lista_res[i*4+j] = m1.la_matriz[i][j] + m2.la_matriz[i][j];
        }
    }
    Matrix res(lista_res);
    return res;
}

Matrix operator-(const Matrix &m1 , const Matrix &m2){
    float lista_res[16];
    for(int i = 0 ;i < 4 ; i++){
        for(int j = 0; j < 4 ; j++){
            lista_res[i*4+j] = m1.la_matriz[i][j] - m2.la_matriz[i][j];
        }
    }
    Matrix res(lista_res);
    return res;
}

Matrix operator*(const Matrix &m1 , const float &v1){
    float lista_res[16];
    for(int i = 0 ;i < 4 ; i++){
        for(int j = 0; j < 4 ; j++){
            lista_res[i*4+j] = m1.la_matriz[i][j] * v1;
        }
    }
    Matrix res(lista_res);
    return res;
}
Matrix operator*(const float &v1, const Matrix &m1){
    float lista_res[16];
    for(int i = 0 ;i < 4 ; i++){
        for(int j = 0; j < 4 ; j++){
            lista_res[i*4+j] = m1.la_matriz[i][j] * v1;
        }
    }
    Matrix res(lista_res);
    return res;
}

Matrix operator*(const Matrix &m1 , const Matrix &m2){
    float lista_res[16];
    int extra = 0;
    for (int i = 0; i < 4 ; i++){
        for(int j = 0; j < 4 ; j++){
           lista_res[j+extra] = m1.la_matriz[i][0] * m2.la_matriz[0][j] + m1.la_matriz[i][1] * m2.la_matriz[1][j] + m1.la_matriz[i][2] * m2.la_matriz[2][j] + m1.la_matriz[i][3] * m2.la_matriz[3][j];
        }
        extra = extra + 4;
    }
    Matrix res(lista_res);
    return res;
}

Punto_Vector operator*(const Matrix &m1, const Punto_Vector &v1){
    float lista_res[4];
    for(int i = 0 ;i < 4 ; i++){
        lista_res[i] = m1.la_matriz[i][0] * v1.x + m1.la_matriz[i][1] * v1.y + m1.la_matriz[i][2] * v1.z + m1.la_matriz[i][3] * v1.valor;
    }
    Punto_Vector res(lista_res[0],lista_res[1],lista_res[2],lista_res[3]);
    return res;
}

void PrintMatrix(float ar[][8], int n, int m) 
{ 
    for (int i = 0; i < n; i++) { 
        for (int j = 0; j < m; j++) { 
            cout << ar[i][j] << "  "; 
        } 
        printf("\n"); 
    } 
    return; 
}

void PrintInverse(float ar[][8], int n, int m) 
{ 
    for (int i = 0; i < n; i++) { 
        for (int j = n; j < m; j++) { 
            printf("%.3f  ", ar[i][j]); 
        } 
        printf("\n"); 
    } 
    return; 
}

Matrix InverseOfMatrix(const Matrix &m1, int order) 
{ 
    // Matrix Declaration. 
  
    float temp; 
    float tempMatrix [4][8];
    for(int i = 0 ;i < 4 ; i++){
        for(int j = 0; j < 4 ; j++){
            tempMatrix [i][j] = m1.la_matriz[i][j];
        }
    }

    printf("=== Matrix ===\n"); 
    PrintMatrix( tempMatrix, order, order);
  
    // Create the augmented matrix 
    // Add the identity matrix 
    // of order at the end of original matrix. 
    for (int i = 0; i < 4; i++) { 
  
        for (int j = 0; j < 2 * 4; j++) { 
            // Add '1' at the diagonal places of 
            // the matrix to create a identity matirx 
            if(j > 3){
                if (j == (i + 4)) 
                    tempMatrix[i][j] = 1; 
                else
                    tempMatrix[i][j] = 0;
            }
        } 
    } 
  
    // Print matrix after interchange operations. 
    printf("\n=== Augmented Matrix ===\n"); 
    PrintMatrix(tempMatrix, order, order * 2); 
  
    // Replace a row by sum of itself and a 
    // constant multiple of another row of the matrix 
    for (int i = 0; i < order; i++) { 
  
        for (int j = 0; j < order; j++) { 
  
            if (j != i) { 
  
                temp = tempMatrix[j][i] / tempMatrix[i][i]; 
                for (int k = 0; k < 2 * order; k++) { 
  
                    tempMatrix[j][k] -= tempMatrix[i][k] * temp; 
                } 
                printf("\n=== Matrix Actual ===\n"); 
                PrintMatrix(tempMatrix, order, order * 2); 

            } 
        } 
    } 
  
    // Multiply each row by a nonzero integer. 
    // Divide row element by the diagonal element 
    for (int i = 0; i < order; i++) { 
  
        temp = tempMatrix[i][i]; 
        for (int j = 0; j < 2 * order; j++) { 
            tempMatrix[i][j] = tempMatrix[i][j] / temp; 
        } 
    } 
  
    // print the resultant Inverse matrix. 
    printf("\n=== Inverse Matrix ===\n"); 
    PrintInverse(tempMatrix, order, 2 * order); 
    float valor_res[16];
    for (int i = 0; i < 4; i++) { 
        for (int j = 4; j < 8; j++) { 
            valor_res[i*4+j-4] = tempMatrix[i][j];
        } 
    } 
    Matrix res(valor_res);
    return res;
} 

/*int main() {
    float valores[16] = { 2, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0};
    Matrix A(valores);
    Matrix B(valores);
    A.imprimirMatrix();
    B.imprimirMatrix();
    Matrix C = A+B;
    C.imprimirMatrix();
    Matrix D = A-B;
    D.imprimirMatrix();

    Matrix E = A*B;
    E.imprimirMatrix();

    float valores2[16] = { 1, -2, 2, 2, 0, 4, -2, 1, 1, -2, 4, 0, 1, -1, 2, 2};
    int orden = 4;
    Matrix ej(valores2);
    Matrix F = InverseOfMatrix(ej,orden);
    F.imprimirMatrix();

    Matrix G = A*3;
    G.imprimirMatrix();

    Punto_Vector ej2(2,3,4,1);
    Punto_Vector res = A*ej2;
    res.imprimir();
    return 0;
} */