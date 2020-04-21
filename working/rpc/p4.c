#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <rpc/rpc.h>
#include <string.h>
#include "p4.h"

int main(argc, argv)
  int             argc;
  char           *argv[];
{
  CLIENT         *cl;    /* a client handle */
  char           *value;
  int             key;

  if (!(cl = clnt_create(argv[1], PFOURPROG, PFOURVERS, "tcp"))) {
    /*
     * CLIENT handle couldn't be created, server not there.
     */
    clnt_pcreateerror(argv[1]);
    exit(1);
  }

  if (strcmp(argv[2], "-r") == 0) {
    key = SUMSQRT_RANGE;
  } else if (strcmp(argv[2], "-u") == 0) {
    key = UPDATE_LIST;
  } else {
    perror("key error");
    exit(1);
  }

  switch (key) {
  case SUMSQRT_RANGE:
    if (argc < 5) {
        fputs("Error: SUMSQRT_RANGE requires a lower and upper bound!\n", stderr);
    }
    struct sumsqrt params;
    params.lower = atoi(argv[3]);
    params.upper = atoi(argv[4]);
    printf("%f\n", *sumsqrt_range_1(&params, cl));
    break;
  case UPDATE_LIST:
    if (argc < 4) {
        fputs("Error: UPDATE_LIST requires at least one value!\n", stderr);
    }
    // construct the list
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

    // update the list and print the results
    llist * rlist = update_list_1(head, cl);
    while (rlist != NULL) { 
        printf("%.1f ", rlist->value);
        rlist = rlist->next;
    } printf("\n");
    break;
  default:
    fprintf(stderr, "%s: unknown key\n", argv[0]);
    exit(1);
  }
}
