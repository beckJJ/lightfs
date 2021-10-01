#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#define TAM_CONTENT 32753
#define TAM_FILENAME 9		// 8 + \0
#define TAM_EXT 4 			// 3 + \0

typedef struct {
	unsigned short index_size; 
	unsigned short cluster_size; 
	unsigned short index_start;
	unsigned short first_cluster;
} METADATA;

typedef unsigned char INDEX;

typedef struct {
	char filename[TAM_FILENAME]; // 8.3 filename
	char extension[TAM_EXT];
	unsigned char flags;	
	INDEX father; // ponteiro para o pai
	char content[TAM_CONTENT];
} CLUSTER;
/* bits de flags: R x x x x x x E 
 * x = don't care
 * R = Root (flag especial para identificar root)
 * E = !Empty (0 = empty / 1 = not empty)
 */