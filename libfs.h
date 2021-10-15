#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#define TAM_CONTENT 32750
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
	INDEX index; // ponteiro para si proprio
	INDEX father; // ponteiro para o pai
	INDEX prev;	// ponteiro para o cluster anterior do arquivo
	INDEX next; // ponteiro para o proximo cluster do arquivo
	char content[TAM_CONTENT];
} CLUSTER;
/* bits de flags: R x x x x x D C 
 * R = Root (flag especial para identificar cluster root)
 * x = don't care
 * x = don't care
 * x = don't care
 * x = don't care
 * x = don't care
 * D = !(Empty Dir/File) (0 = empty / 1 = not empty)
 * C = !(Empty cluster) (0 = empty / 1 = not empty)
 */