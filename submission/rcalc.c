//client code that takes input from the user

//change log: git created 04/07/20
//change log: dr.Sang sample modified 04/13/20
//change log: updated rpc code 04/21/20
//change log: refactored 04/26/20

//libraries
	#include <stdlib.h>
	#include <stdio.h>
	#include <ctype.h>
	#include <rpc/rpc.h>
	#include <string.h>
	#include "rcalc.h"

//main
	int main(argc, argv)
	  int             argc;
	  char           *argv[];
	{
	  CLIENT         *cl;   
	  char           *value;
	  int             key;
	//error check for client handle if server is not there
	  if (!(cl = clnt_create(argv[1], PFOURPROG, PFOURVERS, "tcp"))) {
		clnt_pcreateerror(argv[1]);
		exit(1);
	  }
	//check for -r command
	  if (strcmp(argv[2], "-r") == 0) {
		key = SUMSQRT_RANGE;
	  } 
	//check for -u command
		else if (strcmp(argv[2], "-u") == 0) {
		key = UPDATE_LIST;
	  } 
	//check for everything else
		else {
		perror("key error");
		exit(1);
	  }
	//switch commands based on the provided input
	  switch (key) {
	  //run sum of square roots
		  case SUMSQRT_RANGE:
		  	//check for proper # of arguments
				if (argc < 5) {
					fputs("Error: SUMSQRT_RANGE requires a lower and upper bound!\n", stderr);
				}
		  	//build the struct from rcalc.x
				struct sumsqrt params;
		  	//convert the input to integers using atoi
				params.lower = atoi(argv[3]);
				params.upper = atoi(argv[4]);
		  	//run the sumsqrt_range() function and display results
				printf("%f\n", *sumsqrt_range_1(&params, cl));
		  	//exit out of switch statement
				break;
	  //run update list
		  case UPDATE_LIST:
		  	//check for proper # of arguments
				if (argc < 4) {
					fputs("Error: UPDATE_LIST requires at least one value!\n", stderr);
				}
			//construct the list with the provided numbers
				llist * head = malloc(sizeof(struct llist));
				head->value = strtod(argv[3], NULL);
				head->next = NULL;
				llist * current = head;
				for (int i = 4; i < argc; i++) {
					llist * newnode = malloc(sizeof(struct llist));
					newnode->value = strtod(argv[i], NULL);
					newnode->next = NULL;
					current->next = newnode;
					current = newnode;
				}
			//update the list and print the results
				llist * rlist = update_list_1(head, cl);
				while (rlist != NULL) { 
					printf("%.1f ", rlist->value);
					rlist = rlist->next;
				} printf("\n");
		  	//exit out of switch statement
				break;
	  //everything else
		  default:
			fprintf(stderr, "%s: unknown key\n", argv[0]);
			exit(1);
		  }
	}