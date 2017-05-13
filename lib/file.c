#include <stdio.h>
#include <stdlib.h>

/* readFile: Read from a file and fill a buffer with its content.
 * params:
 *    filename: Filename to open
 *    result:   A char array pointer to put the text in
 * returns:
 *  The length of the text
 *  -1 if an error occured
 */
int readFile(const char* filename, char **result){
  int file_len = 0;

  FILE* file = fopen(filename, "r"); // Input file

  if (!file){
    printf("[E] Can't open file.");
    return 1;
  }

  // Get the file length
  // Ref: http://stackoverflow.com/a/238607/2558252
  fseek(file, 0, SEEK_END);
  file_len = ftell(file);
  rewind(file);

  // Allocate the desired number of bytes
  (*result) = malloc(file_len + 1);

  if (!(*result)) {
    printf("[E] Couldn't allocate %d bytes of memory for the file.\n", file_len);
    return -1;
  }

  // Try to read the entire buffer
  if(1 != fread((*result), file_len, 1, file)){
    printf("[E] Couldn't read from the file.\n");
    return -1;
  }

  // Close the file
  fclose(file);

  return file_len;
}

