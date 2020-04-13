#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <rpc/rpc.h>
#include "p4.h"

static double * rsqrt;

double * sumsqrt_range_1_svc(sumsqrt *params, struct svc_req *rqp) 
{
    if (!rsqrt) {
        rsqrt = malloc(sizeof(double));
    } *rsqrt = 0;
    if (params->lower > params->upper) {
        return rsqrt;
    }
    for (int i = params->lower; i <= params->upper; i++) {
        *rsqrt += sqrt(i);
    }
    return rsqrt;
}

llist * update_list_1_svc(llist *head, struct svc_req *rqp) {
    llist * current = head;
    while (current != NULL) {
        current->value = current->value * current->value / 10.0;
        current = current->next;
    }
    return head;
}
