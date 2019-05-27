#include <random>
#include <iostream>
// #include "EasyBMP.h"
#include "matrix.h"





using std::string;
using std::vector;
using std::pair;
using std::make_pair;
using std::cout;
using std::cerr;
using std::endl;
typedef Matrix<float> Image ;


inline double max(double a, double b){
  return a > b ? a : b;
}

inline double min(double a, double b){
  return a < b ? a : b;
}
inline Matrix<double> diamond_square(int rows, int cols){

    //size of grid to generate, note this must be a
    //value 2^n+1
  int cntc = 0;
    // const int map_size = rows;
    const int map_size = rows - 1;
    // const int map_size = 129;
    double seed = 100.0;
    // double seed = rows*2.0;
    double curVertice = 0.0;
    double h = map_size -1;
    double magnitude = 10.0;
    Matrix<double> height_map = Matrix<double>(map_size, map_size);
    int sideLength = map_size - 1;//sideLength 
    int halfSide;
    double valmin = 10000000.0;
    double valmax = -10000000.0;
    double R = 0.8;
    double vertice = 0.0;
    int reduced_map_size = map_size  ;

    //init the hMap
    for(int i = 0; i < map_size; i++){
      for (int j = 0; j < map_size; j++){
        height_map(i,j) = 0;
      }
    }


    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(-seed, seed );

    seed = dist(mt);
    height_map(0,0) = seed;
    seed = dist(mt);
    height_map(0,map_size - 1) = seed;
    seed = dist(mt);
    height_map(map_size - 1,0) = seed;
    seed = dist(mt);
    height_map(map_size - 1,map_size - 1) = seed;

    // cout << height_map(0,0) << " "<< height_map(0,map_size - 1) << " "<< height_map(map_size - 1,0) << " "<< height_map(map_size - 1,map_size - 1) << " \n";

    // int yyy = (map_size -1 )/2;
    // cout << yyy << "PP";
    for (int sideLength = map_size -1; sideLength >=  2;sideLength /= 2)
    {
 std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(-sideLength * R, R * sideLength);

      halfSide = sideLength / 2;
      // cout << halfSide << " <-halfs " <<" sideLength= " << sideLength<< "\n"; 
      for (int x= 0; x < map_size - 1; x += sideLength)
            {
              for (int y = 0; y < map_size -1 ; y += sideLength)
              {
                curVertice = height_map(x, y) + height_map(x , y + sideLength) + height_map(x + sideLength, y) + height_map(x + sideLength, y + sideLength);
                // cout << x <<"#x " << y << "y " << height_map(x, y) << "  "<< height_map(x , y + sideLength) << "  "<< height_map(x + sideLength, y) << "  "<< height_map(x + sideLength, y + sideLength) << "\n";
                curVertice /= 4.0;
                // cout <<curVertice<< " 1! ";
                curVertice = curVertice + dist(mt) ;
                // cout <<curVertice<< " 2! ";
                // cout << "@ ";
                height_map(x+halfSide, y+halfSide) = curVertice;
                valmin = min(valmin, curVertice);
                valmax = max(valmax, curVertice);
                cntc++;
              }
            }
      for (int x= 0; x <= map_size - 1; x += halfSide)
            {
              for (int y = ((x + halfSide)%sideLength); y <= map_size -1 ; y += sideLength)
              {
                // cout << "\n& x" << x << " y " << y<< "\n";
                /*
                      2
                1     *      3
                      4
                */
                // порядок обхода 1-2-3-4
                // cout <<"\n SS\n";
                // cout << x <<"x " << y << "y " << height_map(x, y) << "  ";//<< height_map(x , y + sideLength) << "  "<< height_map(x + sideLength, y) << "  "<< height_map(x + sideLength, y + sideLength);
                int cnt = 0;

      /*          curVertice = 0.0;
                 if((y - halfSide) > 0){ curVertice +=height_map(x, y - halfSide); cnt++;} 
                 if((x - halfSide) > 0){ curVertice +=height_map(x - halfSide, y); cnt++;} 
                 if((y + halfSide) < map_size){ curVertice +=height_map(x, y + halfSide); cnt++;} 
                 if((x + halfSide) < map_size ){ curVertice +=height_map(x+ halfSide, y); cnt++;} 
                 */
// -----------
                 curVertice = (height_map((x - halfSide + reduced_map_size)%(reduced_map_size), y)+
                  height_map((x + halfSide) % reduced_map_size, y) + 
                  height_map(x, (y + halfSide ) % reduced_map_size)+
                  height_map(x, (y - halfSide + reduced_map_size) % reduced_map_size ));
                  curVertice /= 4.0;





                 // cout << "cnt  " << cnt << "\n";
                // curVertice /= static_cast<double>(cnt);
// -----------
                curVertice = curVertice + dist(mt) ;
                height_map(x,y) = curVertice;
                valmin = min(valmin, curVertice);
                valmax = max(valmax, curVertice);
                cntc++;
              }
            }
    }
cout << "Всего итераций " << cntc;
cout << "min height " <<valmin<< "max height " <<valmax;


    for(int i=0; i<= map_size - 1 ; i++) {
        for(int j=0; j<= map_size - 1 ; j++) { 
            vertice = static_cast<double>((height_map(i,j) - valmin) )/( static_cast<double>((valmax - valmin)));
            vertice *= vertice;
            height_map(i,j) = vertice * 120;
            // cout << (height_map(i,j)) << " ";
        }
    }
    return height_map;
}


/*
int main(){
  diamond_square(1,1);
  return 0;
}
*/






/*
    for (int i=0; i<map_size; ++i){
        std::cout << height_map(4,i) << "\n";}

        cout << "+___________+\n";
    for (int i=0; i<map_size; ++i){
        std::cout << height_map(5,i) << "\n";}

*/


