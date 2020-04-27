//server code that runs the 2 arithmetic functions

//change log: git created 04/07/20
//change log: dr.Sang sample modified 04/13/20
//change log: updated sqrt and list functions 04/21/20
//change log: refactored 04/26/20

//libraries
	#include <stdlib.h>
	#include <string.h>
	#include <math.h>
	#include <rpc/rpc.h>
	#include "rcalc.h"

//prototypes
	static double * rsqrt;

//return sum of square roots
	double * sumsqrt_range_1_svc(sumsqrt *params, struct svc_req *rqp) {
	//allocate the data size required in the heap
		if (!rsqrt) {
			rsqrt = malloc(sizeof(double));
		} *rsqrt = 0;
	//check to make sure upper value is larger than lower value
		if (params->lower > params->upper) {
			return rsqrt;
		}
	//calculate the sum of square roots
		for (int i = params->lower; i <= params->upper; i++) {
			*rsqrt += sqrt(i);
		}
	//return results
		return rsqrt;
	}

//return F * F / 10.0
	llist * update_list_1_svc(llist *head, struct svc_req *rqp) {
	//set the current node to the head
		llist * current = head;
	//scan through list to run operation on each item
		while (current != NULL) {
			current->value = current->value * current->value / 10.0;
			current = current->next;
		}
	//return results
		return head;
	}