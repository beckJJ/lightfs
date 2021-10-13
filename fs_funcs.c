#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>
#include "libfs.h"
#include "libfuncs.h"

unsigned long int findAbsAdd(INDEX point, METADATA metadata)
{
	/* AbsAdd = first_cluster + point * cluster_size */
	return (long)(metadata.first_cluster + (point * metadata.cluster_size));
}

CLUSTER busca_cluster(INDEX point, METADATA metadata, FILE *arq)
{
	CLUSTER cluster;
	unsigned long int absAdd = findAbsAdd(point, metadata);

	fseek(arq, absAdd, SEEK_SET);
	/* copiar cluster do arquivo para a memoria */
	fread(&cluster, sizeof(CLUSTER), 1, arq);

	return cluster;
}

void nome_cluster(INDEX point, METADATA metadata, FILE *arq, char n[])
{ /* recebe um ponteiro e coloca o nome do cluster em n */
	CLUSTER cluster;

	cluster = busca_cluster(point, metadata, arq);

	strcpy(n, cluster.filename);
}

void ext_cluster(INDEX point, METADATA metadata, FILE *arq, char n[])
{ /* recebe um ponteiro e coloca a extensao do cluster em n */
	CLUSTER cluster;

	cluster = busca_cluster(point, metadata, arq);

	strcpy(n, cluster.extension);
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
		/* se o dir estiver vazio */
		if (!(cluster.flags & 0x02)) { /* verificar D */
			printf("<vazio>\n");
		} else { /* percorrer lista de ponteiros até chegar em um vazio */
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

void mk_func(CLUSTER *cluster, METADATA metadata, char nome[], char ext[])
{ /* mesma funcao para mkfile e mkdir, "everything is a file" */
	INDEX i, j;
	unsigned long int absAdd;
	FILE *lightfs;
	CLUSTER new;
	char nome_upper[TAM_FILENAME] = { 0 };
	char nome2[TAM_FILENAME] = { 0 };
	char ext_upper[TAM_EXT] = { 0 };
	char ext2[TAM_EXT] = { 0 };
	
	if (strlen(nome) > TAM_FILENAME) {
		printf("Erro: nome com mais de 8 caracteres\n");
		return;
	}

	if (strlen(ext) > TAM_EXT) {
		printf("Erro: extensao com mais de 3 caracteres\n");
		return;
	}
	if (strlen(ext) < TAM_EXT-1) {
		printf("Erro: extensao com menos de 3 caracteres\n");
		return;
	}
	/* nome em maiusculas */
	for (i = 0; i < strlen(nome); i++) {
		nome_upper[i] = toupper(nome[i]);
	}
	/* ext em maiusculas */
	for (i = 0; i < strlen(ext); i++) {
		ext_upper[i] = toupper(ext[i]);
	}
	
	if (!(lightfs = fopen("LIGHTFS.BIN","r+"))) {
		printf("File open error\n");
		return;
	} else {
		/* procurar primeiro cluster livre */
		i = 0;
		do {
			absAdd = findAbsAdd(i, metadata);
			fseek(lightfs, absAdd, SEEK_SET);
			fread(&new, sizeof(CLUSTER), 1, lightfs);
			i++;
		} while ((new.flags & 0x01) && !feof(lightfs));
		i--;

		if (!(new.flags & 0x01)) { /* se encontrou cluster livre */
			/* chegar no primeiro bloco vazio do pai */
			for (j = 0; cluster->content[j] != 0; j++) {
				/* comparar se tem um arquivo com o mesmo nome e extensao */
				nome_cluster(cluster->content[j], metadata, lightfs, nome2);
				ext_cluster(cluster->content[j], metadata, lightfs, ext2);
				if (!strcmp(nome_upper, nome2) && !strcmp(ext_upper, ext2)) {
					printf("Erro: ja existe um arquivo com esse nome e extensao\n");
					fclose(lightfs);
					return;
				}
			}

			strcpy(new.filename, nome_upper);
			strcpy(new.extension, ext_upper);
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

		} else { /* se nao encontrou */
			printf("Sistema de arquivos cheio!\n");
		}
	}
	fclose(lightfs);
}
// todo: rm_aux que recebe father, nome e ext e chama rm_func
void rm_func(CLUSTER *father, METADATA metadata, INDEX point)
{
	FILE *lightfs;
	unsigned long int address;
	CLUSTER cluster;
	int i;

	if (!(lightfs = fopen("LIGHTFS.BIN", "r+"))) {
		printf("File open error\n");
		return;
	}
	/* receber cluster do ponteiro */
	cluster = busca_cluster(point, metadata, lightfs);
	/* testar se o cluster e root */
	if (cluster.flags & 0x80) {
		printf("Erro: Nao e permitido remover root\n");
		fclose(lightfs);
		return;
	}
	/* testar se o cluster ja esta vazio */
	if (!(cluster.flags & 0x01)) {
		printf("Erro: Cluster vazio\n");
		fclose(lightfs);
		return;
	}
	/* limpar ponteiro no father */
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
			father->flags &= 0xFD; /* limpar nas flags */
		}
		/* escrever novo father no arquivo */
		address = findAbsAdd(father->index, metadata);
		fseek(lightfs, address, SEEK_SET);
		fwrite(father, sizeof(CLUSTER), 1, lightfs);
	} else {
		printf("Erro: Cluster nao encontrado\n");
		fclose(lightfs);
		return;
	}
	/* pegar endereco do cluster */
	address = findAbsAdd(cluster.index, metadata);
	fseek(lightfs, address, SEEK_SET);
	/* limpar cluster */
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
	/* escrever cluster limpo */
	fwrite(&cluster, sizeof(CLUSTER), 1, lightfs);

	fclose(lightfs);
}

void rename_func(CLUSTER father, METADATA metadata, INDEX point,
													   char nome[], char ext[])
{
	char nome_upper[TAM_FILENAME] = { 0 };
	char nome2[TAM_FILENAME] = { 0 };
	char ext_upper[TAM_EXT] = { 0 };
	char ext2[TAM_EXT] = { 0 };
	INDEX i, j;
	CLUSTER cluster;
	unsigned long int address;
	FILE *lightfs;

	if (strlen(nome) > TAM_FILENAME) {
		printf("Erro: nome com mais de 8 caracteres\n");
		return;
	}
	if (strlen(ext) > TAM_EXT) {
		printf("Erro: extensao com mais de 3 caracteres\n");
		return;
	}

	if (!(lightfs = fopen("LIGHTFS.BIN","r+"))) {
		printf("File open error\n");
		return;
	}
	/* colocar nome e ext em maiusculas */
	for (i = 0; i < strlen(nome); i++) {
		nome_upper[i] = toupper(nome[i]);
	}
	for (i = 0; i < strlen(ext); i++) {
		ext_upper[i] = toupper(ext[i]);
	}
	/* comparar se tem um cluster com o mesmo nome e ext */
	for (j = 0; father.content[j] != 0; j++) {
		nome_cluster(father.content[j], metadata, lightfs, nome2);
		ext_cluster(father.content[j], metadata, lightfs, ext2);
		
		if (!(strcmp(nome_upper, nome2)) && !(strcmp(ext_upper, ext2))) {
			printf("Erro: ja existe um arquivo com esse nome e extensao\n");
			fclose(lightfs);
			return;
		}
	}
	/* ler cluster do arquivo */
	cluster = busca_cluster(point, metadata, lightfs);
	/* testar se o cluster e root */
	if (cluster.flags & 0x80) {
		printf("Erro: Nao e permitido renomear root\n");
		fclose(lightfs);
		return;
	}
	/* testar se o cluster esta vazio */
	if (!(cluster.flags & 0x01)) {
		printf("Erro: Cluster vazio\n");
		fclose(lightfs);
		return;
	}
	/* verificar se a extensao e .DIR */
	if (!(strcmp(cluster.extension, "DIR")) && strcmp(ext_upper, "DIR")) {
		printf("Erro: Nao e possivel trocar extensao de diretorio\n");
		fclose(lightfs);
		return;
	}
	/* modificar nome e extensao */
	strcpy(cluster.filename, nome_upper);
	strcpy(cluster.extension, ext_upper);
	/* pegar endereco do cluster */
	address = findAbsAdd(cluster.index, metadata);
	fseek(lightfs, address, SEEK_SET);
	/* escrever cluster */
	fwrite(&cluster, sizeof(CLUSTER), 1, lightfs);
	fclose(lightfs);
}

void edit_func(METADATA metadata, INDEX point, char content[])
{
	CLUSTER cluster;
	unsigned long int address;
	FILE *lightfs;

	if (strlen(content) > TAM_CONTENT) {
		printf("Erro: Arquivo muito grande.\n");
		return;
	}	

	/* abrir arquivo */
	if (!(lightfs = fopen("LIGHTFS.BIN","r+"))) {
		printf("File open error\n");
		return;
	}
	/* carregar cluster na memoria */
	cluster = busca_cluster(point, metadata, lightfs);
	/* verificar se e dir */
	if (strcmp(cluster.extension, "DIR") == 0) {
		printf("Erro: Nao e possivel modificar conteudo de diretorio\n");
		return;
	}
	/* copiar conteudo e modificar flags */
	strcpy(cluster.content, content);
	cluster.flags |= 0x2;
	/* pegar endereco do cluster */
	address = findAbsAdd(cluster.index, metadata);
	fseek(lightfs, address, SEEK_SET);
	/* escrever cluster */
	fwrite(&cluster, sizeof(CLUSTER), 1, lightfs);
	fclose(lightfs);
}
/*
int main(void)
{
	METADATA metadata;
	FILE *lightfs;
	CLUSTER root;
	unsigned long int root_add;

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
	
	mk_func(&root, metadata, "teste", "dir");
	mk_func(&root, metadata, "teste2", "dir");
	mk_func(&root, metadata, "asdfasfd", "dir");
	mk_func(&root, metadata, "qwertyuiop", "dir");

	printf("\nDepois de criar dirs:\n");
	dir_func(root, metadata);

	printf("\nDepois de remover TESTE.DIR:\n");
	rm_func(&root, metadata, 1);
	dir_func(root, metadata);

	printf("\nDepois de tentar remover root:\n");
	rm_func(&root, metadata, 0);

	printf("\nDepois de renomear 'TESTE2.DIR' para 'PASTA1.DIR':\n");
	rename_func(root, metadata, 2, "pasta1", "dir");
	dir_func(root, metadata);

	printf("\nDepois de tentar renomear root:\n");
	rename_func(root, metadata, 0, "root2", "dir");

	printf("\nDepois de tentar renomear 'PASTA1.DIR' para 'PASTA1.TXT':\n");
	rename_func(root, metadata, 2, "pasta1", "txt");
	dir_func(root, metadata);
	
	printf("\nDepois de criar FILE.TXT:\n");
	mk_func(&root, metadata, "file", "txt");
	dir_func(root, metadata);

	printf("\nDepois de renomear FILE.TXT para FILE2.TXT:\n");
	rename_func(root, metadata, 1, "file2", "txt");
	dir_func(root, metadata);
	
	edit_func(metadata, 1, "Testando escrita");
	edit_func(metadata, 0, "Testando escrita");

	return 0;
} */