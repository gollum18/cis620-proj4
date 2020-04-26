/*
 * p4.x: remote protocol for project 4.
 */

struct sumsqrt {
    int lower;
    int upper;
};

struct llist {
    double value;
    struct llist * next;
};

/* program definition, no union or typdef definitions needed */
program PFOURPROG { /* could manage multiple servers */
	version PFOURVERS {
		double SUMSQRT_RANGE(sumsqrt) = 1;
        llist UPDATE_LIST(llist) = 2;
	} = 1;
} = 0x20000001;  /* program number ranges established by ONC */
