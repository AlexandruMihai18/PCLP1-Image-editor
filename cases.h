#ifndef __CASES__
#define __CASES__

#include "utilities.h"

void case_load(image * picture, int *ok, char *token);

void case_select(image *picture, char *token);

void case_rotate(image *picture, char *token);

void case_crop(image *picture, char *token);

void case_apply(image *picture, char *token);

void case_save(image *picture, char *token);

int case_exit(image *picture, int ok, char *token);

#endif //__CASES__
