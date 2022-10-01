#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utilities.h"
#include "functions.h"
#include "cases.h"

int main(void)
{
	// dynamical allocation of a struct
	image *current_picture = malloc(sizeof(image));
	current_picture->matrix = (pixel **)NULL;

	char string[NMAX];
	int ok = 0;

	while (225200) {
		fgets(string, NMAX, stdin);

		char *token = strtok(string, "\n ");

		// assesing the case
		if (strcmp(token, "LOAD") == 0) {
			case_load(current_picture, &ok, token);
		} else if (strcmp(string, "SELECT") == 0) {
			case_select(current_picture, token);
		} else if (strcmp(string, "ROTATE") == 0) {
			case_rotate(current_picture, token);
		} else if (strcmp(string, "CROP") == 0) {
			case_crop(current_picture, token);
		} else if (strcmp(string, "APPLY") == 0) {
			case_apply(current_picture, token);
		} else if (strcmp(string, "SAVE") == 0) {
			case_save(current_picture, token);
		} else if (strcmp(string, "EXIT") == 0) {
			if (case_exit(current_picture, ok, token))
				break;
		} else {
			error_command();
		}
	}
	// freeing the used resources
	if (current_picture->matrix != (pixel **)NULL)
		free_matrix(current_picture->matrix, current_picture->height);
	free(current_picture);

	return 0;
}
