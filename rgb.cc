#include <iostream>
#include<string>
using namespace std;

class rgb{
    public:
        void set_values(int R, int G, int B){
            Red = R;
            Green = G;
            Blue = B;
        }
        void set_red(int R){
            Red = R;
        }    
        void set_green(int G){
            Green = G;
        }    
        void set_blue(int B){
            Blue = B;
        }

        string get_values(){
            return to_string(Red) + " " + to_string(Green) + " " + to_string(Blue);
        }
        int get_red(){
            return Red;
        }    
        int get_green(){
            return Green;
        }    
        int get_blue(){
            return Blue;
        }

    protected:
        int Red;
        int Blue;
        int Green;
};