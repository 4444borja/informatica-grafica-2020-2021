#include <iostream>
#include<string>
using namespace std;

class rgb{
    public:
        void set_values(double R, double G, double B){
            Red = R;
            Green = G;
            Blue = B;
        }
        void set_red(double R){
            Red = R;
        }    
        void set_green(double G){
            Green = G;
        }    
        void set_blue(double B){
            Blue = B;
        }

        string get_values(){
            return to_string(Red) + " " + to_string(Green) + " " + to_string(Blue);
        }
        double get_red(){
            return Red;
        }    
        double get_green(){
            return Green;
        }    
        double get_blue(){
            return Blue;
        }

    protected:
        double Red;
        double Blue;
        double Green;
};