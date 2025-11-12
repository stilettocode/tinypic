#include <iostream>
#include "Image.hpp"
#include "Matrix.hpp"
#include "processing.hpp"
#include <fstream>
#include <string>

//written by Ian Kim 

using namespace std;

int main(int argc, char *argv[]) {
  if (!(argc == 4 || argc == 5 || argc == 3)) {
    cout << "Usage: resize.exe IN_FILENAME OUT_FILENAME WIDTH [HEIGHT]\n"
     << "WIDTH and HEIGHT must be less than or equal to original" << endl;
    return 1;
  }

  if (argc == 3) {
    string file = argv[1];
    string outfile = argv[2];

    ifstream fin(file);
    if (!fin) {
        cout << "Error opening file: " << file << endl;
        return 2;
    }

    Image img;
    Image_init(&img, fin);

    Image dst;
    crop_square_centered_at_max_energy(&img, &dst);

    ofstream fout(outfile);
    Image_print(&dst, fout);
  }

  if (argc == 5) {
    string file = argv[1];
    string outfile = argv[2];

    ifstream fin(file);
    if (!fin) {
        cout << "Error opening file: " << file << endl;
        return 2;
    }

    Image img;
    Image_init(&img, fin);

    int qWidth = stoi(argv[3]);
    int qHeight = stoi(argv[4]);

    if (!(qWidth > 0) || !(qWidth <= Image_width(&img)) || !(qHeight > 0) || !(qHeight <= Image_height(&img))) {
            cout << "Usage: resize.exe IN_FILENAME OUT_FILENAME WIDTH [HEIGHT]\n"
            << "WIDTH and HEIGHT must be less than or equal to original" << endl;
            return 3;
        }

    seam_carve(&img, qWidth, qHeight);

    ofstream fout(outfile);
    Image_print(&img, fout);
    } else if (argc == 4) {
        string file = argv[1];
        string outfile = argv[2];

        ifstream fin(file);
        if (!fin) {
            cout << "Error opening file: " << file << endl;
            return 4;
        }

        Image img;
        Image_init(&img, fin);

        int qWidth = stoi(argv[3]);

        if (!(qWidth > 0) || !(qWidth <= Image_width(&img))) {
            cout << "Usage: resize.exe IN_FILENAME OUT_FILENAME WIDTH [HEIGHT]\n"
            << "WIDTH and HEIGHT must be less than or equal to original" << endl;
            return 5;
        }

        seam_carve_width(&img, qWidth);

        ofstream fout(outfile);
        Image_print(&img, fout);
    }
}