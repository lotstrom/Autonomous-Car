#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>

int main(int argc, char ** argv) {
	//const int vector[5][2] = {{97,233},{35,297},{99,254},{47,311},{99,254}};

	if (argc < 3) {
		std::cout<<"Invalid args"<<std::endl;
		return -1;
	}
	int SIZE = atoi(argv[1]);
	int vector[SIZE][2];

	std::ifstream file(argv[2]);

    for(int row = 0; row < SIZE; ++row)
    {
        std::string line;
        std::getline(file, line);
        std::cout<<line<<std::endl;
        if ( !file.good() )
            break;

        std::stringstream iss(line);

        for (int col = 0; col < 2; ++col)
        {
            std::string val;
            std::getline(iss, val, ',');
            if ( !iss.good() )
                break;

            std::stringstream convertor(val);
            convertor >> vector[row][col];
        }
    }
	std::cout<< (sizeof vector / sizeof vector[0])<<std::endl;
	int sum_x, sum_y ,sum_xy ,sum_x_2 ;
	sum_x=sum_y=sum_xy=sum_x_2=0;
	for (int i=0; i<SIZE; i++)
	{
		//printf("X:%dY:%d\n",vector[i][0],vector[i][1]);
		sum_x+=vector[i][0];
		sum_y+=vector[i][1];
		sum_xy+=vector[i][0]*vector[i][1];
		sum_x_2+=vector[i][0]*vector[i][0];
	}
	//printf("eX:%d\neY:%d\neXY:%d\neX2:%d\n",sum_x,sum_y,sum_xy,sum_x_2);
	long double slope = ((double)(SIZE * sum_xy) - (sum_x*sum_y)) / (double)((SIZE*sum_x_2) - (sum_x*sum_x));
	long double offset = (sum_y - (slope * sum_x)) / SIZE;

	printf("Slope:\t%Lf\nOffset:\t%Lf\n",slope,offset);
	printf("Equation: y = (%Lf) * x + (%Lf)\n", slope,offset);
	return 0;
}