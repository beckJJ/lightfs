#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include "libfs.h"

char isroot (CLUSTER cluster)
{
	return ((cluster.flags & 0x80) && 1);
}

unsigned long int findAbsAdd(char point, METADATA metadata)
{
	// AbsAdd = first_cluster + point * cluster_size
	return (metadata.first_cluster + (point * metadata.cluster_size));
}

void dir_func(CLUSTER cluster, METADATA metadata)
{
	int i;
	unsigned long int absAdd;
	FILE *lightfs;
	char nome[TAM_FILENAME] = { 0 };
	char ext[TAM_EXT] = { 0 };

	if (!(lightfs = fopen("LIGHTFS.BIN","r"))) {
		printf("File open error\n");
		return;
	} else {
		// se o cluster estiver vazio
		if (!(cluster.flags & 0x01)) {
			printf("<vazio>\n");
		} else { // percorrer lista de ponteiros até chegar em um vazio
			i = 0;
			while (cluster.content[i]) {
				// encontrar o endereco do primeiro ponteiro de cluster
				absAdd = findAbsAdd(cluster.content[i], metadata);
				// encontrar cluster no arquivo
				fseek (lightfs, absAdd, SEEK_SET);
				// copiar nome e extensao do arquivo para a memoria
				fread (nome, TAM_FILENAME, 1, lightfs);
				fread (ext, TAM_EXT, 1, lightfs);
				// imprimir nome do cluster
				printf("%s.%s\n", nome, ext);
				i++;
			}
		}
	}
	fclose(lightfs);
}

int main(void)
{
	METADATA metadata;
	FILE *lightfs;
	CLUSTER root;
	unsigned long int root_add;
	//printf("%d", sizeof(CLUSTER));
	// abrir arquivo
	if (!(lightfs = fopen("LIGHTFS.BIN","r+"))) {
		printf("File open error\n");
		exit(1);
	}
	// ler metadata do arquivo
	if (!fread(&metadata, sizeof(METADATA), 1, lightfs)) {
		printf("File read error\n");
		exit(1);
	}
	// copiar cluster root para a memoria
	root_add = findAbsAdd(0, metadata); // root = cluster 0
	fseek(lightfs, root_add, SEEK_SET);
	if (!fread(&root, sizeof(CLUSTER), 1, lightfs)) {
		printf("File read error\n");
		exit(1);
	}
	fclose(lightfs);
	printf("%s.%s\n", root.filename, root.extension);
	dir_func(root, metadata);
	return 0;
}