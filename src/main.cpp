/****************************************************************************
  FileName  [ main.cpp ]
  Synopsis  [ Main function of the router. ]
  Author    [ Fu-Yu Chuang ]
  Date      [ 2017.6.9 ]
****************************************************************************/
#include <iostream>
#include <fstream>
#include "router.h"
using namespace std;

int main(int argc, char** argv)
{
    fstream input, output;

    if (argc == 3) {
        input.open(argv[1], ios::in);
        output.open(argv[2], ios::out);
        if (!input) {
            cerr << "Cannot open the input file \"" << argv[1]
                 << "\". The program will be terminated..." << endl;
            exit(1);
        }
        if (!output) {
            cerr << "Cannot open the output file \"" << argv[2]
                 << "\". The program will be terminated..." << endl;
            exit(1);
        }
    }
    else {
        cerr << "Usage: ./Router <input file> <output file> " << endl;
        exit(1);
    }

    Router* router = new Router(input);
    router->route();
    // router->reportPin();
    // router->reportEdge();

    return 0;
}
