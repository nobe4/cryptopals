#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../lib/file.h"

#define BLOCK_LENGTH 16

int main(int argc, char *argv[]){
	// File input and line iterator
	char *input = NULL,
			 *line = NULL;

	// Various variables to handle the best score and the current blocks number
	int current_score = 0,
			best_score = 0,
			best_line = 0,
			current_line = 0,
			line_length = 0,
			blocks_number = 0;

	int i, // First block to compare
			j; // Second block to compare

	// Read the file
	readFile("input.txt", &input);

	// Read each line, separated by a newline
	while ((line = strsep(&input, "\n")) ) {
		current_score = 0;

		line_length = strlen(line);

		blocks_number = line_length / BLOCK_LENGTH;

		// Loop through all 2 chars combinations on the line
		for(i = 0; i < blocks_number - 1; i ++){
			for(j = i + 1; j < blocks_number; j ++){

				// If the two next chars are the same as the ones in the next block, the
				// resulting ASCII chars will be equal, we can increase the score.
				if (line[i * BLOCK_LENGTH    ] == line[j * BLOCK_LENGTH    ] &&
						line[i * BLOCK_LENGTH + 1] == line[j * BLOCK_LENGTH + 1]) {
					current_score ++;
				}
			}
		}

		// Keep the best
		if (current_score > best_score){
			best_score = current_score;
			best_line = current_line;
		}

		current_line ++;
	}

	printf("Best line %d with a score of %d\n", best_line, best_score);

	if (input) free(input);
	if (line) free(line);

	return 0;
}
