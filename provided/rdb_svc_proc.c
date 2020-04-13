#include <stdio.h>
#include <string.h>
#include <rpc/rpc.h>
#include "rdb.h"

FILE           *fp = NULL;
static record  *pR = NULL;

int 
readRecord()
{
  char            buf[MAX_STR];

  if (!pR) {
    pR = (record *) malloc(sizeof(record));
    pR->firstName = (char *) malloc(MAX_STR);
    pR->middleInitial = (char *) malloc(MAX_STR);
    pR->lastName = (char *) malloc(MAX_STR);
    pR->location = (char *) malloc(MAX_STR);
  }
  if (!fgets(buf, MAX_STR - 1, fp)) return (0);
  if (sscanf(buf, "%s%s%s%d%s", pR->firstName, pR->middleInitial,
	     pR->lastName, &(pR->phone), pR->location) != 5) return (0);
  return (1);
}

record         *
firstname_key_1_svc(char **name, struct svc_req *rqp)
{
  return ((record *) pR);
}

record         *
lastname_key_1_svc(char **name, struct svc_req *rqp)
{
  if (!(fp = fopen(DATABASE, "r")))
    return ((record *) NULL);

  while (readRecord())
    if (!strcmp(pR->lastName, *name)) break;
  if (feof(fp)) {
    fclose(fp);
    return ((record *) NULL);
  }
  fclose(fp);
  return ((record *) pR);
}

record         *
phone_key_1_svc(int *phone, struct svc_req *rqp)
{
  return ((record *) pR);
}

record         *
location_key_1_svc(char **name, struct svc_req *rqp)
{
  return ((record *) pR);
}

int            *
add_record_1_svc(record *r, struct svc_req *rqp)
{
  static int             status;
  return ((int *) &status);
}
