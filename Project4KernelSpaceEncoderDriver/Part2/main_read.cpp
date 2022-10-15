#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
using namespace std;
#define INPUT 0
#define OUTPUT 1
void export_pin(int pin, int dir) { // 0 in, 1 out
    char out[50];
    int len = sprintf(out, "%d", pin);
    std::ofstream fOut ("/sys/class/gpio/export");
    fOut.write(out, len);
    fOut.close();

    char file_path[50];
    sprintf(file_path, "/sys/class/gpio/gpio%d/direction", pin);
    FILE *fp = fopen (file_path, "a");
    if(!dir) {
    fputs("in", fp);
    }
    else {
    fputs("out", fp);
    }
    fclose(fp);
}

int main() {
    // cat /sys/class/gpio/gpio27/value > /sys/class/gpio/gpio17/value
    export_pin(17, INPUT);
    export_pin(27, OUTPUT);
    std::ifstream pin_in ("/sys/class/gpio/gpio17/value");
    std::ofstream pin_out ("/sys/class/gpio/gpio27/value"); // probably should be iostream to read state of output pin
    char LOW[1] = {'0'};
    char HIGH[1] = {'1'};
    while(1) {
        std::cout<<pin_in.get()-'0'<<"\n";
        if(!pin_out.write(HIGH, 1)) {
            cout<<"write failed??";
        }
        pin_in.seekg(0, fstream::beg);
        pin_out.seekp(0, fstream::beg);
        sleep(1);
        std::cout<<pin_in.get()-'0'<<"\n";
        pin_in.seekg(0, fstream::beg);
        pin_out.write(LOW, 1);
        pin_out.seekp(0, fstream::beg);
        sleep(1);

    }
    return 0;
}