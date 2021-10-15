#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include "libfs.h"

void createFile(void)
{
	FILE *file;
	METADATA metadata;
	INDEX index; // indice de ponteiros
	CLUSTER root;
	CLUSTER empty_cluster;
	int i;

	strcpy(root.filename, "ROOT");
	
	strcpy(root.extension, "DIR");
	root.flags = 0x81; // R + C
	root.index = 0;
	root.next = 0;
	root.prev = 0;
	// father irrelevante por ter flag root
	
	for (i = 0; i < TAM_FILENAME; i++) {
		empty_cluster.filename[i] = 0;
	}
	for (i = 0; i < TAM_EXT; i++) {
		empty_cluster.extension[i] = 0;
	}
	for (i = 0; i < TAM_CONTENT; i++) {
		root.content[i] = 0;
		empty_cluster.content[i] = 0;
	}
	empty_cluster.flags = 0;
	empty_cluster.father = 0;
	empty_cluster.index = 0;
	empty_cluster.next = 0;
	empty_cluster.prev = 0;
	
	metadata.index_size = 0x0100; // 256
	metadata.cluster_size = 0x8000; // 32KB
	metadata.index_start = 0x0004;
	metadata.first_cluster = 0x0108;
	
	if (! (file = fopen("LIGHTFS.BIN", "wb"))) {
		printf("File creation error\n");
		printf("Exiting...\n");
		exit(1);
	} else {
		if ( fwrite(&metadata, sizeof(METADATA), 1, file) != 1) {
			printf("File write error\n");
			printf("Exiting...\n");
			exit(1);
		}
		for(i = 0; i < 256; i++) {
			index = (char)i;
			if (fwrite(&index, sizeof(INDEX), 1, file) != 1) {
				printf("File write error\n");
				printf("Exiting...\n");
				exit(1);
			}
		}
		// write root directory
		if (fwrite(&root, sizeof(CLUSTER), 1, file) != 1) {
			printf("File write error\n");
			printf("Exiting...\n");
			exit(1);
		}
		// the other clusters
		for (i = 1; i < 256; i++) {
			if (fwrite(&empty_cluster, sizeof(CLUSTER), 1, file) != 1) {
				printf("File write error\n");
				printf("Exiting...\n");
				exit(1);
			}
		}
	}
	fclose(file);
}

int main(void)
{
	char key = 0;
	printf("Criar novo arquivo? (Y/N) ");
	while (key != 'Y' && key != 'y' && key != 'N' && key != 'n') {
		fflush(stdin);
		key = getch();
	}
	printf("%c\n", key);
	if (key == 'Y' || key == 'y') {
		remove("LIGHTFS.BIN");
		createFile();
		printf("Arquivo criado com sucesso.\n");
	} else {
		printf("Nenhuma operacao foi realizada.\n");
	}
	return 0;
}