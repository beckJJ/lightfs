/* Bruno Gorhs Vergara			- 324256
 * Joao Pedro Ferreira Pereira	- 324521
 * Pedro Company Beck			- 324055
 * 
 * Programa com a interface de linha de comando para operar o arquivo
 * LIGHTFS.BIN com as funcoes em libfuncs.h.
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
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
	printf("\t\t  Arquitetura e Organizacao de Computadores II\n");
	printf("\t\t\t\tTrabalho 2 - 2021/1\n");
	printf("\n\n\n\n\n");
	printf("        __        __   _______   _     _   _______   _______   _______ \n");
	printf("       |  |      |  | |  _____| | |   | | |___ ___| |  _____| |  _____|\n");
	printf("       |  |      |  | | |       | |   | |    | |    | |       | |      \n");
	printf("       |  |      |  | | |  ___  | |___| |    | |    | |_____  | |_____ \n");
	printf("       |  |      |  | | | |_  | |  ___  |    | |    |  _____| |_____  |\n");
	printf("       |  |      |  | | |   | | | |   | |    | |    | |             | |\n");
	printf("       |  |____  |  | | |___| | | |   | |    | |    | |        _____| |\n");
	printf("       |_______| |__| |_______| |_|   |_|    |_|    |_|       |_______|\n");
	printf("\n\n\n\n\n");
	printf("                                          ");
	printf("Bruno Grohs Vergara          - 324256\n");
	printf("                                          ");
	printf("Joao Pedro Ferreira Pereira  - 324521\n");
	printf("                                          ");
	printf("Pedro Company Beck           - 324055\n");
	printf("\nAperte ENTER para comecar... ");

	while (key != ENTER) {
		fflush(stdin);
		key = getch();
	}
	system("cls");
}

void print_help(void)
{
	printf("Comandos disponiveis:\n");
	printf("CD\t- Troca de diretorio\n");
	printf("CLS\t- Limpa a tela\n");
	printf("DIR\t- Mostra o conteudo do diretorio\n");
	printf("DISP\t- Mostra o conteudo de um arquivo\n");
	printf("EDIT\t- Edita o conteudo de um arquivo\n");
	printf("HELP\t- Mostra essa pagina de ajuda\n");
	printf("MKDIR\t- Cria um diretorio\n");
	printf("MKFILE\t- Cria um arquivo\n");
	printf("MOVE\t- Move um arquivo ou diretorio\n");
	printf("RENAME\t- Renomeia um arquivo ou diretorio\n");
	printf("RM\t- Remove um arquivo ou diretorio\n");
	printf("\nEXIT\t- Termina a execucao\n");
	printf("\n\nMais informacoes no arquivo README.TXT\n"); 
}

void recebe_input(char input[], CLUSTER *cluster, METADATA metadata)
{
	char input_lower[MAX_INPUT] = { 0 };
	char *comando;
	char filename[TAM_FILENAME], filename_new[TAM_FILENAME]; 
	char *input_aux;
	char ext[TAM_EXT], ext_new[TAM_EXT]; 
	char path[MAX_INPUT], path_old[MAX_INPUT], path_new[MAX_INPUT];
	int i, j;
	INDEX index_aux;

	fflush(stdin);
	gets(input_lower);
	fflush(stdin);
	
	if (strlen(input_lower) == 0) {
		return;
	}
	if (strlen(input_lower) >= MAX_INPUT) {
		printf("Erro: Input com mais de 200 caracteres\n");
		return;
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
		comando = strtok(NULL, " ");
		if (comando == NULL) {
			printf("Erro: Caminho nao informado\n");
			return;
		}

		strcpy(path, comando);

		cd_aux(metadata, path, cluster);

		return;
	}
	if (strcmp(comando, "DIR") == 0 || strcmp(comando, "LS")  == 0) {
		printf("Conteudo do diretorio %s.%s:\n", cluster->filename, cluster->extension);
		dir_func(*cluster, metadata);
		// ignorar o resto
		while (comando != NULL) {
			comando = strtok(NULL, "");
		}
		return;
	}
	if (strcmp(comando, "RM") == 0) {
		comando = strtok(NULL, ".");
		if (comando == NULL) {
			printf("Erro: Arquivo nao informado\n");
			return;
		}
		strcpy(filename, comando);

		comando = strtok(NULL, " ");
		if (comando == NULL) {
			printf("Erro: Extensao nao informada\n");
			return;
		}
		strcpy(ext, comando);
		// ignorar o resto
		while (comando != NULL) {
			comando = strtok(NULL, "");
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

		strcpy(filename, comando);

		comando = strtok(NULL, " ");
		if (comando == NULL) {
			printf("Erro: Extensao nao informada\n");
			return;
		}

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
		input_aux = malloc(strlen(input_lower));
		comando = strtok(NULL, ".");
		if (comando == NULL) {
			printf("Erro: Nome nao informado\n");
			return;
		}
		strcpy(filename, comando);

		comando = strtok(NULL, " ");
		if (comando == NULL) {
			printf("Erro: Extensao nao informada\n");
			return;
		}

		strcpy(ext, comando);

		comando = strtok(NULL, "\"");
		if (comando == NULL) {
			printf("Erro: Conteudo nao informado\n");
			return;
		}
		input_aux = strtok(input_lower, " ");
		input_aux = strtok(NULL, " ");
		input_aux = strtok(NULL, "\"");

		if (edit_aux(cluster, metadata, filename, ext, input_aux)) {
			printf("Arquivo %s.%s editado com sucesso.\n", filename, ext);
		}
		return;
	}
	if (strcmp(comando, "DISP") == 0) {
		comando = strtok(NULL, ".");
		if (comando == NULL) {
			printf("Erro: Nome nao informado\n");
			return;
		}

		strcpy(filename, comando);

		comando = strtok(NULL, " ");
		if (comando == NULL) {
			printf("Erro: Extensao nao informada\n");
			return;
		}

		strcpy(ext, comando);

		if (!disp_aux(cluster, metadata, filename, ext)) {
			printf("Erro: Arquivo nao encontrado\n");
		}
		return;
	}
	if (strcmp(comando, "MOVE") == 0) {
		comando = strtok(NULL, " ");
		if (comando == NULL) {
			printf("Erro: Caminho antigo nao informado\n");
			return;
		}

		strcpy(path_old, comando);	

		comando = strtok(NULL, " ");
		if (comando == NULL) {
			printf("Erro: Caminho novo nao informado\n");
			return;
		}

		strcpy(path_new, comando);	

		if (move_aux(metadata, path_old, path_new, cluster)) {
			printf("Arquivo movido com sucesso.\n");
		}
		return;
	}
	if (strcmp(comando, "RENAME") == 0) {
		comando = strtok(NULL, ".");
		if (comando == NULL) {
			printf("Erro: Arquivo nao informado\n");
			return;
		}

		strcpy(filename, comando);

		comando = strtok(NULL, " ");
		if (comando == NULL) {
			printf("Erro: Extensao nao informada\n");
			return;
		}

		strcpy(ext, comando);
		
		comando = strtok(NULL, ".");
		if (comando == NULL) {
			printf("Erro: Novo nome nao informado\n");
			return;
		}

		strcpy(filename_new, comando);
		
		comando = strtok(NULL, ".");
		if (comando == NULL) {
			printf("Erro: Nova extensao nao informada\n");
			return;
		}

		strcpy(ext_new, comando);

		// ignorar o resto
		while (comando != NULL) {
			comando = strtok(NULL, "");
		}
		if (rename_aux(cluster, metadata, filename, ext, filename_new, ext_new)) {
			printf("Arquivo %s.%s renomeado para %s.%s com sucesso.\n", 
										 filename, ext, filename_new, ext_new);
		}

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

void printAbsPath(METADATA metadata, const CLUSTER cluster, CLUSTER root)
{
	FILE *arq;
	CLUSTER aux;

	aux = cluster;
	
	// abrir arquivo 
	if (!(arq = fopen("LIGHTFS.BIN","r"))) {
		printf("File open error\n");
		exit(1);
	}

	printf("\n/%s.%s/", root.filename, root.extension);
	if (cluster.index == root.index) {
		printf(">");
		fclose(arq);
		return;
	}
	while (root.index != aux.index) {
		while (root.index != aux.father) {
			aux = busca_cluster(aux.father, metadata, arq);
		}
		printf("%s.%s/", aux.filename, aux.extension);
		root = aux;
		aux = cluster;
	}
	printf(">");
	fclose(arq);
	return;
}

int main(void)
{
	char input[MAX_INPUT] = { 0 };
	METADATA metadata;
	FILE *lightfs;
	CLUSTER cluster, root;
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
	root = cluster;

	print_interface();
	printAbsPath(metadata, cluster, root);

	while (strcmp(input, "EXIT") != 0) {
		recebe_input(input, &cluster, metadata);
		printAbsPath(metadata, cluster, root);
	}

	return 0;
}