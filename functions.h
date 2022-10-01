#ifndef __FUNCTIONS__
#define __FUNCTIONS__

#include "utilities.h"

void read_matrix(FILE * file, image * picture);

void load(image *picture, int *ok, char *token);

void selection(image *picture, char *token);

void total_rotation(image *picture, int angle);

void square_rotation(image *picture, int angle);

pixel **crop(image *picture);

int type_of_apply(char string[]);

void apl_krnl(pixel ***aux, image *picture, int i, int j, int type);

void apply(image *picture, char *token);

void include_matrix(int type, FILE *file, image *picture);

void save(image *picture, char *token);

#endif //__FUNCTIONS__
