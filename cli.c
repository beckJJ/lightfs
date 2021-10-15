// para compilar:
// gcc cli.c fs_funcs.c -o cli.o
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <string.h>
#include <ctype.h>
#include "libfs.h"
#include "libfuncs.h"

#define ENTER 13
#define MAX_INPUT 200

void print_interface(void)
{
	char key = 0;
	char *input;
	system("cls");
	printf("Arquitetura e Organizacao de Computadores II\n");
	printf("Trabalho 2 - 2021/1\n");
	printf("Bruno Grohs Vergara\t\t- 324256\n");
	printf("Joao Pedro Ferreira Pereira\t- 324521\n");
	printf("Pedro Company Beck\t\t- 324055\n");
	printf("\nAperte ENTER para comecar\n");

	while (key != ENTER) {
		fflush(stdin);
		key = getch();
	}
	system("cls");
}

void print_help(void)
{
	printf("CD\t- Troca de diretorio\n");
	printf("CLS\t- Limpa a tela\n");
	printf("DIR\t- Mostra o conteudo do diretorio\n");
	printf("EDIT\t- Edita o conteudo de um arquivo\n");
	printf("HELP\t- Mostra essa pagina de ajuda\n");
	printf("MKDIR\t- Cria um diretorio\n");
	printf("MKFILE\t- Cria um arquivo\n");
	printf("MOVE\t- Move um arquivo ou diretorio\n");
	printf("RENAME\t- Renomeia um arquivo ou diretorio\n");
	printf("RM\t- Remove um arquivo ou diretorio\n");
	printf("\nEXIT\t- Termina a execucao\n");
}

void recebe_input(char input[], CLUSTER *cluster, METADATA metadata)
{
	char input_lower[MAX_INPUT] = { 0 };
	char *comando;
	char *filename; 
	char *ext; 
	int i, j;
	INDEX x;

	gets(input_lower);
	if (strlen(input_lower) == 0) {
		return;
	}
	if (strlen(input_lower) >= MAX_INPUT) {
		printf("Erro: Input com mais de 200 caracteres\n");
		return;
		//exit(1);
	}
	for (i = 0; i < strlen(input_lower); i++) {
		input[i] = toupper(input_lower[i]);
	}
	if (strlen(input_lower) < MAX_INPUT) { 
		for (; i < MAX_INPUT; i++) {
			input[i] = 0;
		}
	}
	comando = strtok(input, " ");

	if (strcmp(comando, "CD") == 0) {
		printf(" %s 0", comando);
		return;
	}
	if (strcmp(comando, "DIR") == 0) {
		printf("Conteudo do diretorio %s.%s:\n", cluster->filename, cluster->extension);
		dir_func(*cluster, metadata);
		return;
	}
	if (strcmp(comando, "RM") == 0) {
		comando = strtok(NULL, ".");
		if (comando == NULL) {
			printf("Erro: Arquivo nao informado\n");
			return;
		}
		filename = malloc(strlen(comando));
		strcpy(filename, comando);

		comando = strtok(NULL, " ");
		if (comando == NULL) {
			printf("Erro: Extensao nao informada\n");
			return;
		}

		ext = malloc(strlen(comando));
		strcpy(ext, comando);
		// ignorar o resto
		while (comando != NULL) {
			comando = strtok(NULL, " ");
		}
		if (rm_aux(cluster, metadata, filename, ext)) {
			printf("Arquivo %s.%s removido com sucesso.\n", filename, ext);
		}
		return;
	}
	if (strcmp(comando, "MKDIR") == 0) {
		comando = strtok(NULL, ".");
		if (comando == NULL) {
			printf("Erro: Nome nao informado\n");
			return;
		}
		filename = malloc(strlen(comando));
		strcpy(filename, comando);


		// ignorar o resto
		while (comando != NULL) {
			comando = strtok(NULL, " ");
		}
		if (mk_func(cluster, metadata, filename, "DIR")) {
			printf("Criado diretorio %s.DIR com sucesso.\n", filename);
		}
		return;
	}
	if (strcmp(comando, "MKFILE") == 0) {
		comando = strtok(NULL, ".");
		if (comando == NULL) {
			printf("Erro: Nome nao informado\n");
			return;
		}
		filename = malloc(strlen(comando));
		strcpy(filename, comando);

		comando = strtok(NULL, " ");
		if (comando == NULL) {
			printf("Erro: Extensao nao informada\n");
			return;
		}

		ext = malloc(strlen(comando));
		strcpy(ext, comando);
		// ignorar o resto
		while (comando != NULL) {
			comando = strtok(NULL, " ");
		}

		if (mk_func(cluster, metadata, filename, ext)) {
			printf("Criado arquivo %s.%s com sucesso.\n", filename, ext);
		}
		return;
	}
	if (strcmp(comando, "EDIT") == 0) {
		printf(" %s 5", comando);
		return;
	}
	if (strcmp(comando, "MOVE") == 0) {
		printf(" %s 6", comando);
		return;
	}
	if (strcmp(comando, "RENAME") == 0) {
		printf(" %s 7", comando);
		return;
	}
	if (strcmp(comando, "HELP") == 0) {
		print_help();
		return;
	}
	if (strcmp(comando, "CLS") == 0) {
		system("cls");
		return;
	}
	if (strcmp(comando, "EXIT") == 0) {
		system("cls");
		exit(0);
	}
	else {
		printf("Comando nao encontrado\n");
		return;
	}
}

int main(void)
{
	char input[MAX_INPUT] = { 0 };
	METADATA metadata;
	FILE *lightfs;
	CLUSTER cluster;
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
	if (!fread(&cluster, sizeof(CLUSTER), 1, lightfs)) {
		printf("File read error\n");
		exit(1);
	}
	fclose(lightfs);

	print_interface();
	printf("%s.%s>", cluster.filename, cluster.extension);

	while (strcmp(input, "EXIT") != 0) {
		recebe_input(input, &cluster, metadata);
		printf("\n%s.%s>", cluster.filename, cluster.extension);
	}

	return 0;
}