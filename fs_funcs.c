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

unsigned long int findAbsAdd(INDEX point, METADATA metadata)
{
	// AbsAdd = first_cluster + point * cluster_size
	return (long)(metadata.first_cluster + (point * metadata.cluster_size));
}

CLUSTER busca_cluster(INDEX point, METADATA metadata, FILE *arq)
{
	CLUSTER cluster;
	unsigned long int absAdd = findAbsAdd(point, metadata);

	fseek(arq, absAdd, SEEK_SET);
	// copiar cluster do arquivo para a memoria
	fread(&cluster, sizeof(CLUSTER), 1, arq);

	return cluster;
}

void nome_cluster (INDEX point, METADATA metadata, FILE *arq, char n[TAM_FILENAME])
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
				sub = busca_cluster(cluster.content[i], metadata, lightfs);

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

void rm_func (CLUSTER *father, METADATA metadata, INDEX point)
{
	FILE *lightfs;
	unsigned long int address;
	CLUSTER cluster;
	int i;

	if (!(lightfs = fopen("LIGHTFS.BIN", "r+"))) {
		printf("File open error\n");
		return;
	}
	// receber cluster do ponteiro
	cluster = busca_cluster(point, metadata, lightfs);
	// testar se o cluster e root
	if (cluster.flags & 0x80) {
		printf("Erro: Nao e permitido remover root\n");
		return;
	}
	// testar se o cluster ja esta vazio
	if (!(cluster.flags & 0x01)) {
		printf("Erro: Cluster vazio\n");
		return;
	}
	// limpar ponteiro no father
	i = 0;
	while (father->content[i] != cluster.index && i < TAM_CONTENT) {
		i++;
	}
	if (father->content[i] == cluster.index) {
		father->content[i] = 0;
		do {
			father->content[i] = father->content[i+1];
			i++;
		} while (father->content[i]);
		if (father->content[0] == 0) {
			father->flags &= 0xFD; // limpar nas flags
		}
		// escrever novo father no arquivo
		address = findAbsAdd(father->index, metadata);
		fseek(lightfs, address, SEEK_SET);
		fwrite(father, sizeof(CLUSTER), 1, lightfs);
	} else {
		printf("Erro: Cluster nao encontrado\n");
		return;
	}
	// pegar endereco do cluster
	address = findAbsAdd(cluster.index, metadata);
	fseek(lightfs, address, SEEK_SET);
	// limpar cluster
	for (i = 0; i < TAM_FILENAME; i++) {
		cluster.filename[i] = 0;
	}
	for (i = 0; i < TAM_EXT; i++) {
		cluster.extension[i] = 0;
	}
	for (i = 0; i < TAM_CONTENT; i++) {
		cluster.content[i] = 0;
	}
	cluster.flags = 0;
	cluster.father = 0;
	cluster.index = 0;
	// escrever cluster limpo
	fwrite(&cluster, sizeof(CLUSTER), 1, lightfs);

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

	printf("\nAntes de criar dirs:\n");
	dir_func(root, metadata);
	
	mkdir_func(&root, metadata, "teste");
	mkdir_func(&root, metadata, "teste2");
	mkdir_func(&root, metadata, "asdfasfd");
	mkdir_func(&root, metadata, "qwertyuiop");
	printf("\nDepois de criar dirs:\n");
	dir_func(root, metadata);

	printf("\nDepois de remover cluster TESTE:\n");
	rm_func(&root, metadata, 1);
	dir_func(root, metadata);

	printf("\nDepois de tentar remover root:\n");
	rm_func(&root, metadata, 0);

	return 0;
}