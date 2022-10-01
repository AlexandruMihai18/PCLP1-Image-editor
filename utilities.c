#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utilities.h"

// dynamical allocation of a matrix
pixel **matrix_alloc(int height, int width)
{
	pixel **matrix = malloc(height * sizeof(pixel *));

	// defensive programming
	if (!matrix)
		return NULL;

	for (int i = 0; i < height; i++) {
		matrix[i] = malloc(width * sizeof(pixel));

		// further defensive programming
		if (!matrix[i]) {
			free_matrix(matrix, i);
			return NULL;
		}
	}

	return matrix;
}

// freeing the used resources
void free_matrix(pixel **matrix, int height)
{
	for (int i = 0; i < height; i++)
		free(matrix[i]);
	free(matrix);
}

// function used for local tests
void print_matrix(image *picture)
{
	if (picture->type % 3 != 0) {
		for (int i = 0; i < picture->height; i++) {
			for (int j = 0; j < picture->width; j++)
				printf("%lf ", picture->matrix[i][j].gray);
			printf("\n");
		}
	}

	if (picture->type % 3 == 0) {
		for (int i = 0; i < picture->height; i++) {
			for (int j = 0; j < picture->width; j++) {
				printf("%lf ", picture->matrix[i][j].red);
				printf("%lf ", picture->matrix[i][j].green);
				printf("%lf ", picture->matrix[i][j].blue);
			}
			printf("\n");
		}
	}
}

// converting the string that describes the image type into a number
int determine_type(char type[])
{
	return type[1] - '0';
}

// converting a string to a number
// also checking that the given string is a number
int conversion(char string[])
{
	int number = 0, i = 0;
	int word_length = strlen(string);
	int sign = 1;

	// determining the sign of the number
	if (string[0] == '-') {
		sign = -1;
		i++;
	}

	for (; i < word_length; i++) {
		// the string is not a number
		if ((string[i] - '0' < 0) || (string[i] - '0' > 9))
			return -225200;
		number = number * 10 + string[i] - '0';
	}

	return sign * number;
}

// swap function
void swap(int *a, int *b)
{
	int aux = *a;
	*a = *b;
	*b = aux;
}

// clamp function for apply
void clamp(double *red, double *green, double *blue)
{
	if (*red < 0)
		*red = 0;
	if (*green < 0)
		*green = 0;
	if (*blue < 0)
		*blue = 0;

	if (*red > 255)
		*red = 255;
	if (*green > 255)
		*green = 255;
	if (*blue > 255)
		*blue = 255;
}

// building the kernel matrices for each type of apply
double **set_apply(int type)
{
	double **matrix = malloc(KSIZE * sizeof(double *));
	for (int i = 0; i < KSIZE; i++)
		matrix[i] = malloc(KSIZE * sizeof(double));

	//the matrix for edge
	if (type == 0) {
		for (int i = 0; i < KSIZE; i++) {
			for (int j = 0; j < KSIZE; j++) {
				matrix[i][j] = -1;
				if (i == j && i == 1)
					matrix[i][j] = 8;
			}
		}
		return matrix;
	}

	// the matrix for sharpen
	if (type == 1) {
		for (int i = 0; i < KSIZE; i++) {
			for (int j = 0; j < KSIZE; j++) {
				matrix[i][j] = -1;
				if ((i + j) % 2 == 0)
					matrix[i][j] = 0;
				if (i == 1 && j == 1)
					matrix[i][j] = 5;
			}
		}
		return matrix;
	}

	// the matrix for blur
	if (type == 2) {
		for (int i = 0; i < KSIZE; i++) {
			for (int j = 0; j < KSIZE; j++)
				matrix[i][j] = 1;
		}
		return matrix;
	}

	// the matrix for gaussian blur
	if (type == 3) {
		for (int i = 0; i < KSIZE; i++) {
			for (int j = 0; j < KSIZE; j++) {
				matrix[i][j] = 1;
				if (i == 1)
					matrix[i][j] = 2 * matrix[i][j];
				if (j == 1)
					matrix[i][j] = 2 * matrix[i][j];
			}
		}
		return matrix;
	}

	return matrix;
}

// the value that each pixel will need to be divided
// the value is based on the type of apply
double set_value(int type)
{
	if (type == 2)
		return 9;
	if (type == 3)
		return 16;
	return 1;
}

// function that check if a pixel is on the border
int border_check(image *picture, int i, int j)
{
	if (!(picture->y1 + i))
		return 0;
	if (!(picture->x1 + j))
		return 0;
	if ((picture->y1 + i + 1) == picture->height)
		return 0;
	if ((picture->x1 + j + 1) == picture->width)
		return 0;
	return 1;
}

// error message for invalid command
void error_command(void)
{
	printf("Invalid command\n");
}

// error message for select
void select_error(void)
{
	printf("Invalid set of coordinates\n");
}

// error message for load
void load_error(char name[])
{
	printf("Failed to load %s\n", name);
}

void image_error(void)
{
	printf("No image loaded\n");
}
