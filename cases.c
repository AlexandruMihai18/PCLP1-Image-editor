#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utilities.h"
#include "functions.h"
#include "cases.h"

// function for the load case
void case_load(image *picture, int *ok, char *token)
{
	// checking if there are no more parameters
	token = strtok(NULL, "\n ");
	if (!token) {
		error_command();
		return;
	}

	load(picture, ok, token);
}

// function for the select case
void case_select(image *picture, char *token)
{
	token = strtok(NULL, "\n ");
	if (!token) {
		error_command();
		return;
	}

	selection(picture, token);
}

// function for the rotate case
void case_rotate(image *picture, char *token)
{
	token = strtok(NULL, "\n ");
	if (!token) {
		error_command();
		return;
	}

	int angle;

	// converting a string to the angle value
	angle = conversion(token);

	token = strtok(NULL, "\n ");
	if (token) {
		error_command();
		return;
	}

	// checking if there is an image loaded
	if (picture->matrix == (pixel **)NULL) {
		image_error();
		return;
	}

	// checking if the angle is supported
	if (angle % 90 != 0) {
		printf("Unsupported rotation angle\n");
		return;
	}

	// checking if the entire matrix is rotated
	if (picture->x2 == picture->width && picture->x1 == 0 &&
		picture->y2 == picture->height && picture->y1 == 0)
		total_rotation(picture, angle);
	// checking if only a square selection is rotated
	else if ((picture->x2 - picture->x1) == (picture->y2 - picture->y1)) {
		square_rotation(picture, angle);
	} else {
		// error message for encountering a non square selection
		printf("The selection must be square\n");
		return;
	}

	printf("Rotated %d\n", angle);
}

// function for the crop case
void case_crop(image *picture, char *token)
{
	token = strtok(NULL, "\n ");
	if (token) {
		error_command();
		return;
	}

	if (picture->matrix == (pixel **)NULL) {
		image_error();
		return;
	}

	// building the cropped image
	pixel **cropped_image;
	cropped_image = crop(picture);

	// malloc failed
	if (!cropped_image)
		return;

	free_matrix(picture->matrix, picture->height);

	picture->matrix = cropped_image;
	printf("Image cropped\n");

	// establishing new sizes for the cropped image
	int new_width = picture->x2 - picture->x1;
	int new_height = picture->y2 - picture->y1;
	picture->height = new_height;
	picture->width = new_width;

	picture->x1 = 0;
	picture->y1 = 0;
	picture->x2 = new_width;
	picture->y2 = new_height;
}

// function for the apply case
void case_apply(image *picture, char *token)
{
	apply(picture, token);
}

// function for the save case
void case_save(image *picture, char *token)
{
	token = strtok(NULL, "\n ");
	if (!token) {
		error_command();
		return;
	}

	save(picture, token);
}

// function for the exit case
int case_exit(image *picture, int ok, char *token)
{
	token = strtok(NULL, "\n ");
	if (token) {
		error_command();
		return 0;
	}

	if (picture->matrix == (pixel **)NULL)
		image_error();

	return ok;
}
