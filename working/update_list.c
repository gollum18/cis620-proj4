//this is the update_list() service that can be inserted inside the server file when ready
//to compile this sample, use  "gcc update_list.c"

//libraries
	#include <stdio.h>
	#include <stdlib.h>

//start function
	int main(int argc, char *argv[]){
	//get the arguments
		int array[argc - 1]; //-1 for size because 1st argument is executable name
	//compute sum
		for(int i = 1; i < argc; i++){
			double input = atoi(argv[i]);
			double result = input * input / 10.0;
			printf("%.1f ", result);
		}
	//tab
		printf("\n");
	//return success
		return 0;
}
