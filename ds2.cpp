#include <random>
#include <iostream>
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
Matrix<double> diamond_square(int rows, int cols){


  int cntc = 0;
     int map_size = rows;
  if(rows % 2 == 0) {
    map_size = rows;
  }
  else {
    map_size = rows - 1;

  }
    // const int map_size = 129;
    double seed = 70.0;
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
    // seed = dist(mt);
    height_map(0,map_size % map_size ) = seed;
    // seed = dist(mt);
    height_map(map_size % map_size, 0) = seed;
    // seed = dist(mt);
    height_map(map_size % map_size ,map_size % map_size) = seed;

    // cout << height_map(0,0) << " "<< height_map(0,map_size - 1) << " "<< height_map(map_size - 1,0) << " "<< height_map(map_size - 1,map_size - 1) << " \n";

    // int yyy = (map_size -1 )/2;
    // cout << yyy << "PP";
    for (int sideLength = map_size ; sideLength >=  2;sideLength /= 2)
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
                curVertice = height_map(x % map_size, y % map_size) + height_map(x % map_size , (y + sideLength) % map_size) + height_map((x + sideLength) % map_size, y % map_size) + height_map((x + sideLength) % map_size, (y + sideLength) % map_size);
                // cout << x <<"#x " << y << "y " << height_map(x, y) << "  "<< height_map(x , y + sideLength) << "  "<< height_map(x + sideLength, y) << "  "<< height_map(x + sideLength, y + sideLength) << "\n";
                curVertice /= 4.0;
                // cout <<curVertice<< " 1! ";
                curVertice = curVertice + dist(mt) ;
                // cout <<curVertice<< " 2! ";
                // cout << "@ ";
                height_map((x+halfSide) % map_size, (y+halfSide) % map_size) = curVertice;
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

 
                 curVertice = (height_map((x - halfSide + reduced_map_size)%(reduced_map_size), y % map_size)+
                  height_map((x + halfSide) % reduced_map_size, y % map_size) + 
                  height_map(x % map_size, (y + halfSide ) % reduced_map_size)+
                  height_map(x % map_size, (y - halfSide + reduced_map_size) % reduced_map_size ));
                  curVertice /= 4.0;





                 // cout << "cnt  " << cnt << "\n";
                // curVertice /= static_cast<double>(cnt);
                curVertice = curVertice + dist(mt) ;
                height_map(x % map_size,y % map_size) = curVertice;
                valmin = min(valmin, curVertice);
                valmax = max(valmax, curVertice);
                cntc++;
              }
            }
    }
// cout << "Всего итераций " << cntc;
// cout << "min height " <<valmin<< "max height " <<valmax;

int sign = 1;
int maxv = 0;
int minv = 0;
int maxx = 0;
int maxz = 0;
    for(int i=0; i<= map_size -1 ; i++) {
        for(int j=0; j<= map_size-1 ; j++) { 
            sign = 1.0;
            vertice = static_cast<double>((height_map(i % map_size,j % map_size) - valmin) )/( static_cast<double>((valmax - valmin)));
            if (vertice < 0){
              sign = -1.0;
            }
            vertice *= (vertice * sign);
            vertice *= 120;
            height_map(i % map_size,j % map_size) = vertice;
            if(height_map(i % map_size,j % map_size) >= maxv){
              maxv = height_map(i % map_size, j % map_size);
              maxx = i % map_size;
              maxz = j % map_size;
            }
            // maxv = max(maxv, height_map(i % map_size,j % map_size));
            minv = min(minv, height_map(i % map_size,j % map_size));
        }
    } 
            // cout << "minv " << minv << "  maxv " << maxv << "\n";

            int level = 10;
    for(int i=0; i<= 1 ; i++) {
      for(int j=0; j<= map_size-1 ; j++) {
        height_map(i,j) = height_map(map_size -1 -i ,  j);
        // height_map(map_size -1 - j,i) = height_map(map_size -1 - j, map_size -1 - i);
       }
    }
       for(int j=0; j<=1 ; j++) {
      for(int i=0; i<= map_size-1 ; i++) {
        height_map(i,j) = height_map(i,map_size -1 - j);
       }
     }
/*
      for(int i=0; i<= map_size -1 ; i++) {
        for(int j=0; j<= map_size-1 ; j++) { 
          if(height_map(i,j) < 20) {
            height_map(i,j) = 19;
          }
        }
      }
*/

            return height_map;

}



