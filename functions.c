#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "utilities.h"
#include "functions.h"

// function to read the image matrix
void read_matrix(FILE *file, image *picture)
{
	// ignoring the new line terminator
	fseek(file, 1, SEEK_CUR);

	// using an auxiliary value in order to read the matrix
	unsigned char aux;

	if (picture->type == 1 || picture->type == 2) {
		for (int i = 0; i < picture->height; i++) {
			for (int j = 0; j < picture->width; j++) {
				fscanf(file, "%hhu", &aux);
				// casting the auxiliary value to a double for better accuracy
				picture->matrix[i][j].gray = (double)aux;
			}
		}
	}

	if (picture->type == 3) {
		for (int i = 0; i < picture->height; i++) {
			for (int j = 0; j < picture->width; j++) {
				fscanf(file, "%hhu", &aux);
				picture->matrix[i][j].red = (double)aux;
				fscanf(file, "%hhu", &aux);
				picture->matrix[i][j].green = (double)aux;
				fscanf(file, "%hhu", &aux);
				picture->matrix[i][j].blue = (double)aux;
			}
		}
	}

	if (picture->type == 4 || picture->type == 5) {
		for (int i = 0; i < picture->height; i++) {
			for (int j = 0; j < picture->width; j++) {
				// binary reading the matrix
				fread(&aux, sizeof(unsigned char), 1, file);
				picture->matrix[i][j].gray = (double)aux;
			}
		}
	}

	if (picture->type == 6) {
		for (int i = 0; i < picture->height; i++) {
			for (int j = 0; j < picture->width; j++) {
				fread(&aux, sizeof(unsigned char), 1, file);
				picture->matrix[i][j].red = (double)aux;
				fread(&aux, sizeof(unsigned char), 1, file);
				picture->matrix[i][j].green = (double)aux;
				fread(&aux, sizeof(unsigned char), 1, file);
				picture->matrix[i][j].blue = (double)aux;
			}
		}
	}
}

// function that loads a new image
void load(image *picture, int *ok, char *token)
{
	// determining the name of the file
	char name[NMAX];
	strcpy(name, token);

	// checking if there is more than one argument to the load function
	token = strtok(NULL, "\n ");
	if (token) {
		error_command();
		return;
	}

	// freeing the existent matrix
	if (picture->matrix != (pixel **)NULL)
		free_matrix(picture->matrix, picture->height);

	// opening the file
	FILE *file = fopen(name, "rb");
	// the opening failed
	if (!file) {
		load_error(name);
		picture->matrix = (pixel **)NULL;
		return;
	}

	char type[NMAX];
	fscanf(file, "%s", type);

	// determining the type of the image
	picture->type = determine_type(type);
	// determining the width and height of the matrix
	fscanf(file, "%d%d", &picture->width, &picture->height);

	// determining the maximum intensity of the pixels
	if (picture->type % 3 != 1)
		fscanf(file, "%d", &picture->max_intensity);

	pixel **cpy_matrix;
	cpy_matrix = matrix_alloc(picture->height, picture->width);

	// defensive programming
	if (!cpy_matrix) {
		load_error(name);
		fclose(file);
		return;
	}

	picture->matrix = cpy_matrix;
	read_matrix(file, picture);

	picture->x1 = 0;
	picture->y1 = 0;
	picture->x2 = picture->width;
	picture->y2 = picture->height;

	fclose(file);
	printf("Loaded %s\n", name);

	// ok value certifies that an image has been loaded successfully
	*ok = 1;
}

// function for the selection
void selection(image *picture, char *token)
{
	if (picture->matrix == (pixel **)NULL) {
		image_error();
		return;
	}

	// fixing the values for the select ALL case
	if (strcmp(token, "ALL") == 0) {
		picture->x1 = 0;
		picture->y1 = 0;
		picture->x2 = picture->width;
		picture->y2 = picture->height;
		printf("Selected ALL\n");
		return;
	}

	// using an auxiliary array for the value that will be read
	int coord[4];
	coord[0] = conversion(token);

	for (int i = 1; i < 4; i++) {
		token = strtok(NULL, "\n ");
		// the string does not contain enough arguments
		if (!token) {
			error_command();
			return;
		}

		// converting the string to a numeric value
		coord[i] = conversion(token);

		// using a very sugestive flag for failed performance
		if (coord[i] == -225200) {
			error_command();
			return;
		}
	}

	// checking that values are inside the matrix
	for (int i = 0; i < 2; i++) {
		if (coord[2 * i] < 0 || coord[2 * i] > picture->width) {
			select_error();
			return;
		}
		if (coord[2 * i + 1] < 0 || coord[2 * i + 1] > picture->height) {
			select_error();
			return;
		}
	}

	if (coord[0] == coord[2] || coord[1] == coord[3]) {
		select_error();
		return;
	}

	// swapping the coord accordingly to the axes
	if (coord[0] > coord[2])
		swap(&coord[0], &coord[2]);

	if (coord[1] > coord[3])
		swap(&coord[1], &coord[3]);

	picture->x1 = coord[0];
	picture->y1 = coord[1];
	picture->x2 = coord[2];
	picture->y2 = coord[3];

	printf("Selected ");

	for (int i = 0; i < 4; i++)
		printf("%d ", coord[i]);
	printf("\n");
}

// full rotation of the matrix using a recursive subprogram
void total_rotation(image *picture, int angle)
{
	if (angle == 0)
		return;

	// assessing a posivite angle value
	angle = (angle + 360) % 360;

	pixel **aux = matrix_alloc(picture->width, picture->height);

	int height = picture->height;
	int width = picture->width;

	// full rotation of the matrix using an auxiliary matrix
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			aux[j][height - i - 1].gray = picture->matrix[i][j].gray;
			aux[j][height - i - 1].red = picture->matrix[i][j].red;
			aux[j][height - i - 1].green = picture->matrix[i][j].green;
			aux[j][height - i - 1].blue = picture->matrix[i][j].blue;
		}
	}

	free_matrix(picture->matrix, picture->height);

	picture->matrix = aux;

	// establishing the new sizes of the matrix
	picture->height = width;
	picture->width = height;
	picture->x2 = height;
	picture->y2 = width;

	total_rotation(picture, angle - 90);
}

// square rotation of a fixed selection
void square_rotation(image *picture, int angle)
{
	if (angle == 0)
		return;

	angle = (angle + 360) % 360;

	pixel **aux = matrix_alloc(picture->y2, picture->x2);

	int y = picture->y2 - picture->x2;
	int x = picture->x1 + picture->y2;

	// the square rotation using an auxiliary matrix
	for (int i = picture->y1; i < picture->y2; i++) {
		for (int j = picture->x1; j < picture->x2; j++) {
			aux[y + j][x - i - 1].gray = picture->matrix[i][j].gray;
			aux[y + j][x - i - 1].red = picture->matrix[i][j].red;
			aux[y + j][x - i - 1].green = picture->matrix[i][j].green;
			aux[y + j][x - i - 1].blue = picture->matrix[i][j].blue;
		}
	}

	for (int i = picture->y1; i < picture->y2; i++) {
		for (int j = picture->x1; j < picture->x2; j++) {
			picture->matrix[i][j].gray = aux[i][j].gray;
			picture->matrix[i][j].red = aux[i][j].red;
			picture->matrix[i][j].green = aux[i][j].green;
			picture->matrix[i][j].blue = aux[i][j].blue;
		}
	}

	free_matrix(aux, picture->y2);

	square_rotation(picture, angle - 90);
}

// the cropping algorithm of an image
pixel **crop(image *picture)
{
	int width = picture->x2 - picture->x1;
	int height = picture->y2 - picture->y1;

	pixel **matrix = matrix_alloc(height, width);
	if (!matrix)
		return NULL;

	int y1 = picture->y1;
	int x1 = picture->x1;

	// using a new matrix as the cropped image
	for (int i = 0 ; i < height; i++) {
		for (int j = 0; j < width; j++) {
			matrix[i][j].red = picture->matrix[i + y1][j + x1].red;
			matrix[i][j].green = picture->matrix[i + y1][j + x1].green;
			matrix[i][j].blue = picture->matrix[i + y1][j + x1].blue;
			matrix[i][j].gray = picture->matrix[i + y1][j + x1].gray;
		}
	}

	return matrix;
}

// function to determine the type of apply
int type_of_apply(char string[])
{
	if (strcmp(string, "EDGE") == 0) {
		printf("APPLY EDGE done\n");
		return 0;
	}
	if (strcmp(string, "SHARPEN") == 0) {
		printf("APPLY SHARPEN done\n");
		return 1;
	}
	if (strcmp(string, "BLUR") == 0) {
		printf("APPLY BLUR done\n");
		return 2;
	}
	if (strcmp(string, "GAUSSIAN_BLUR") == 0) {
		printf("APPLY GAUSSIAN_BLUR done\n");
		return 3;
	}

	return -1;
}

// function that establishes the new value of a pixel
void apl_krnl(pixel ***aux, image *picture, int i, int j, int type)
{
	// building the kernel matrix
	double **krnl = set_apply(type);

	// the value that each pixel needs to be divind based on the type of apply
	double value = set_value(type);

	int y1 = picture->y1;
	int x1 = picture->x1;

	double red = 0, green = 0, blue = 0;

	// assessing the value to each pixel based on the apply pattern
	for (int k = -1; k < 2; k++) {
		for (int l = -1; l < 2; l++) {
			red += picture->matrix[y1 + i + k][x1 + j + l].red *
				   krnl[1 + l][1 + k];
			green += picture->matrix[y1 + i + k][x1 + j + l].green *
					 krnl[1 + l][1 + k];
			blue += picture->matrix[y1 + i + k][x1 + j + l].blue *
					krnl[1 + l][1 + k];
		}
	}
	// dividing by their respective value parameter
	red = (double)red / value;
	green = (double)green / value;
	blue = (double)blue / value;
	clamp(&red, &green, &blue);

	// copying the values in the auxiliary matrix
	(*aux)[i][j].red = red;
	(*aux)[i][j].green = green;
	(*aux)[i][j].blue = blue;

	// freeing the used resources - the kernel matrix
	for (int i = 0; i < KSIZE; i++)
		free(krnl[i]);
	free(krnl);
}

// function to apply a specific filter
void apply(image *picture, char *token)
{
	if (picture->matrix == (pixel **)NULL) {
		image_error();
		return;
	}

	token = strtok(NULL, "\n ");
	if (!token) {
		error_command();
		return;
	}

	// the image has gray pixels
	if (picture->type % 3 != 0) {
		printf("Easy, Charlie Chaplin\n");
		return;
	}

	int type = type_of_apply(token);

	if (type == -1) {
		printf("APPLY parameter invalid\n");
		return;
	}

	int height = picture->y2 - picture->y1;
	int width = picture->x2 - picture->x1;

	// using an auxiliary matrix for the apply action
	pixel **aux = matrix_alloc(height, width);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (border_check(picture, i, j))
				apl_krnl(&aux, picture, i, j, type);
		}
	}

	int y1 = picture->y1;
	int x1 = picture->x1;

	// over-writing the existent matrix
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (border_check(picture, i, j)) {
				picture->matrix[y1 + i][x1 + j].red = aux[i][j].red;
				picture->matrix[y1 + i][x1 + j].green = aux[i][j].green;
				picture->matrix[y1 + i][x1 + j].blue = aux[i][j].blue;
			}
		}
	}
	// freeing the used resources
	free_matrix(aux, height);
}

// function that transfers the image in an output file
void include_matrix(int type, FILE *file, image *picture)
{
	// using an auxiliary value to convert back to unsigned char
	unsigned char aux;

	if (type == 1) {
		if (picture->type % 3 != 0) {
			for (int i = 0; i < picture->height; i++) {
				for (int j = 0; j < picture->width; j++) {
					// rounding the result for better accuracy
					aux = (unsigned char)round(picture->matrix[i][j].gray);
					fprintf(file, "%d ", aux);
				}
				fprintf(file, "\n");
			}
		}

		if (picture->type % 3 == 0) {
			for (int i = 0; i < picture->height; i++) {
				for (int j = 0; j < picture->width; j++) {
					aux = (unsigned char)round(picture->matrix[i][j].red);
					fprintf(file, "%d ", aux);
					aux = (unsigned char)round(picture->matrix[i][j].green);
					fprintf(file, "%d ", aux);
					aux = (unsigned char)round(picture->matrix[i][j].blue);
					fprintf(file, "%d ", aux);
				}
				fprintf(file, "\n");
			}
		}
	}

	if (type == 0) {
		if (picture->type % 3 != 0) {
			for (int i = 0; i < picture->height; i++) {
				for (int j = 0; j < picture->width; j++) {
					aux = (unsigned char)round(picture->matrix[i][j].gray);
					fwrite(&aux, sizeof(unsigned char), 1, file);
				}
			}
		}

		if (picture->type % 3 == 0) {
			for (int i = 0; i < picture->height; i++) {
				for (int j = 0; j < picture->width; j++) {
					aux = (unsigned char)round(picture->matrix[i][j].red);
					fwrite(&aux, sizeof(unsigned char), 1, file);
					aux = (unsigned char)round(picture->matrix[i][j].green);
					fwrite(&aux, sizeof(unsigned char), 1, file);
					aux = (unsigned char)round(picture->matrix[i][j].blue);
					fwrite(&aux, sizeof(unsigned char), 1, file);
				}
			}
		}
	}
}

// function to save the matrix in an output file
void save(image *picture, char *token)
{
	if (picture->matrix == (pixel **)NULL) {
		image_error();
		return;
	}

	// the name of the output file
	char name[NMAX];
	strcpy(name, token);

	token = strtok(NULL, "\n ");

	// using a type variable to establish a certain save format
	int type = 0;

	if (token && strcmp(token, "ascii") == 0)
		type = 1;

	FILE *file = fopen(name, "wb");
	// opening the file has failed
	if (!file) {
		printf("Cannot open the file\n");
		return;
	}

	// converting an ascii picture into a binary picture
	if (picture->type <= 3 && type == 0)
		picture->type = picture->type + 3;

	// converting a binary picture into an ascii picture
	if (picture->type > 3 && type == 1)
		picture->type = picture->type - 3;

	// printing the required info in the output file
	fprintf(file, "P%d\n", picture->type);
	fprintf(file, "%d %d\n", picture->width, picture->height);

	if (picture->type % 3 !=  1)
		fprintf(file, "%d\n", picture->max_intensity);

	include_matrix(type, file, picture);
	printf("Saved %s\n", name);

	fclose(file);
}
