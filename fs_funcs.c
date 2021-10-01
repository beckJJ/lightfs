#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>
#include "libfs.h"

char isroot (CLUSTER cluster)
{
	return ((cluster.flags & 0x80) && 1);
}

unsigned long int findAbsAdd(char point, METADATA metadata)
{
	// AbsAdd = first_cluster + point * cluster_size
	return (long)(metadata.first_cluster + (point * metadata.cluster_size));
}

CLUSTER busca_cluster(char point, METADATA metadata, FILE *arq)
{
	CLUSTER cluster;
	unsigned long int absAdd = findAbsAdd(point, metadata);

	fseek(arq, absAdd, SEEK_SET);
	// copiar cluster do arquivo para a memoria
	fread(&cluster, sizeof(CLUSTER), 1, arq);

	return cluster;
}

void nome_cluster (char point, METADATA metadata, FILE *arq, char n[TAM_FILENAME])
{ //recebe um ponteiro e coloca o nome do cluster em n
	CLUSTER cluster;

	cluster = busca_cluster(point, metadata, arq);

	strcpy(n, cluster.filename);
}

void dir_func(CLUSTER cluster, METADATA metadata)
{
	int i;
	unsigned long int absAdd;
	FILE *lightfs;
	CLUSTER sub;

	if (!(lightfs = fopen("LIGHTFS.BIN","r"))) {
		printf("File open error\n");
		return;
	} else {
		if (!(cluster.flags & 0x01)) {
			printf("Empty cluster\n");
		}
		// se o dir estiver vazio
		if (!(cluster.flags & 0x02)) { // verificar D
			printf("<vazio>\n");
		} else { // percorrer lista de ponteiros até chegar em um vazio
			i = 0;
			while (cluster.content[i]) {
				// encontrar o endereco do primeiro ponteiro de cluster
				//absAdd = findAbsAdd(cluster.content[i], metadata);
				// encontrar cluster no arquivo
				//fseek(lightfs, absAdd, SEEK_SET);
				// copiar cluster do arquivo para a memoria
				sub = busca_cluster(cluster.content[i], metadata, lightfs);
				//fread(&sub, sizeof(CLUSTER), 1, lightfs);
				// imprimir nome do cluster
				printf("%s.%s\n", sub.filename, sub.extension);
				i++;
			}
		}
	}
	fclose(lightfs);
}

void mkdir_func(CLUSTER *cluster, METADATA metadata, char nome[])
{
	INDEX i, j;
	unsigned long int absAdd;
	FILE *lightfs;
	CLUSTER new;
	char nome_upper[TAM_FILENAME] = { 0 };
	char nome2[TAM_FILENAME] = { 0 };
	
	if (strlen(nome) > TAM_FILENAME) {
		printf("Erro: nome do diretorio com mais de 8 caracteres\n");
		return;
	}
	
	for (i = 0; i < strlen(nome); i++) {
		nome_upper[i] = toupper(nome[i]);
	}
	
	if (!(lightfs = fopen("LIGHTFS.BIN","r+"))) {
		printf("File open error\n");
		return;
	} else {
		// procurar primeiro cluster livre
		i = 0;
		do {
			absAdd = findAbsAdd(i, metadata);
			fseek(lightfs, absAdd, SEEK_SET);
			fread(&new, sizeof(CLUSTER), 1, lightfs);
			i++;
		} while ((new.flags & 0x01) && !feof(lightfs));
		i--;

		if (!(new.flags & 0x01)) { // se encontrou cluster livre
			//chegar no primeiro bloco vazio do pai
			for (j = 0; cluster->content[j] != 0; j++) {
				// comparar se tem um dir com o mesmo nome
				nome_cluster(cluster->content[j], metadata, lightfs, nome2);
				if (strcmp(nome_upper, nome2) == 0) {
					printf("Erro: ja existe um diretorio com esse nome\n");
					fclose(lightfs);
					return;
				}
			}

			strcpy(new.filename, nome_upper);
			strcpy(new.extension, "DIR");
			new.flags = 0x01;
			new.index = i;
			new.father = cluster->index;

			absAdd = findAbsAdd(i, metadata);
			fseek(lightfs, absAdd, SEEK_SET);
			fwrite(&new, sizeof(CLUSTER), 1, lightfs);

			cluster->content[j] = new.index;
			cluster->flags |= 0x02;
			absAdd = findAbsAdd(cluster->index, metadata);
			fseek(lightfs, absAdd, SEEK_SET);
			fwrite(cluster, sizeof(CLUSTER), 1, lightfs);

		} else { // se nao encontrou
			printf("Sistema de arquivos cheio!\n");
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
	if (!(lightfs = fopen("LIGHTFS.BIN","r"))) {
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
	printf("Antes de criar dir:\n");
	dir_func(root, metadata);
	printf("Depois de criar dir:\n");
	mkdir_func(&root, metadata, "teste");
	mkdir_func(&root, metadata, "teste2");
	mkdir_func(&root, metadata, "asdfasfd");
	mkdir_func(&root, metadata, "qwertyuiop");
	dir_func(root, metadata);

	return 0;
}