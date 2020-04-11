//this is the sumsqrt_range() service that can be inserted inside the server file when ready
//to compile this sample, need to use -lm to link the math library so "gcc sumsqrt_range.c -lm"

//libraries
	#include <stdio.h>
	#include <stdlib.h>
	#include <math.h>

//start function
	int main(int argc, char *argv[]){
	//check arguments(this will need to change when moved because client takes 4 args)
		if(argc != 3){//1st arg is executable callout
			printf("\n2 numbers are required!\n");
			return 1;
		}

	//get the arguments
		int first = atoi(argv[1]); //1st numerical argument
		int second = atoi(argv[2]); //2nd numerical argument
		double result = 0;

	//compute sum of sqrt
		for(int i = first; i < second + 1; i++){
			result += sqrt(i);
		}

	//return results
		printf("%.6f\n", result);
		return 0;
}
