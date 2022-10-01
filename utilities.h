#ifndef __UTILITIES__
#define __UTILITIES__

#define NMAX 100
#define KSIZE 3

// using a struct to represent each pixel of the image
// the struct is treating both cases (color or grayscale)
typedef struct {
	double red, green, blue;
	double gray;
} pixel;

// using a struct to represent the required info for a picture
typedef struct {
	pixel **matrix;
	int type;
	int height, width;
	int max_intensity;
	int x1, y1, x2, y2;
} image;

pixel **matrix_alloc(int height, int width);

void free_matrix(pixel **matrix, int height);

void print_matrix(image *picture);

int determine_type(char type[]);

int conversion(char string[]);

void swap(int *a, int *b);

void clamp(double *red, double *green, double *blue);

double **set_apply(int type);

double set_value(int type);

int border_check(image *picture, int i, int j);

void error_command(void);

void select_error(void);

void load_error(char name[]);

void image_error(void);

#endif //__UTILITIES__
