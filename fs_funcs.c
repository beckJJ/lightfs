/* Bruno Gorhs Vergara			- 324256
 * Joao Pedro Ferreira Pereira	- 324521
 * Pedro Company Beck			- 324055
 * 
 * Programa com as funcoes para operar no arquivo LIGHTFS.BIN que sao chamadas
 * utilizando a interface de linha de comando.
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>
#include "libfs.h"
#include "libfuncs.h"

#ifndef MAX_INPUT
#define MAX_INPUT 200
#endif

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

void refresh_cluster(CLUSTER *cluster, METADATA metadata)
{
	FILE *arq;
	if (!(arq = fopen("LIGHTFS.BIN","r"))) {
		printf("File open error\n");
		return;
	}
	*cluster = busca_cluster(cluster->index, metadata, arq);
	fclose(arq);
}

INDEX busca_file(METADATA metadata, CLUSTER father, FILE *arq,
													   char nome[], char ext[])
{ /* recebe um nome e extensao e retorna o ponteiro do cluster */
	CLUSTER cluster; 
	int i = 0;

	cluster = busca_cluster(father.content[i], metadata, arq);
	if (!strcmp(cluster.filename, nome) && !strcmp(cluster.extension, ext)) {
		return cluster.index;
	}
	while (strcmp(cluster.filename, nome) || strcmp(cluster.extension, ext)) {
		cluster = busca_cluster(father.content[i], metadata, arq);
		i++;
		if (i >= 256) {
			break;
		}
	}
	if (!strcmp(cluster.filename, nome) && !strcmp(cluster.extension, ext)) {
		return cluster.index;
	} else {
		printf("Erro: Arquivo nao encontrado\n");
		return 0;
	}
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
			printf("\t<vazio>\n");
		} else { /* percorrer lista de ponteiros até chegar em um vazio */
			i = 0;
			while (cluster.content[i]) {
				sub = busca_cluster(cluster.content[i], metadata, lightfs);

				printf("\t%s.%s\n", sub.filename, sub.extension);
				i++;
			}
		}
	}
	fclose(lightfs);
}

int mk_func(CLUSTER *father, METADATA metadata, char nome[], char ext[])
{ /* mesma funcao para mkfile e mkdir, "everything is a file" */
	INDEX i, j;
	unsigned long int absAdd;
	FILE *lightfs;
	CLUSTER new;
	char nome_upper[TAM_FILENAME] = { 0 };
	char nome2[TAM_FILENAME] = { 0 };
	char ext_upper[TAM_EXT] = { 0 };
	char ext2[TAM_EXT] = { 0 };
	
	if (strlen(nome) >= TAM_FILENAME) {
		printf("Erro: Nome com mais de 8 caracteres\n");
		return 0;
	}

	if (strlen(ext) >= TAM_EXT) {
		printf("Erro: Extensao com mais de 3 caracteres\n");
		return 0;
	}
	if (strlen(ext) < TAM_EXT-1) {
		printf("Erro: Extensao com menos de 3 caracteres\n");
		return 0;
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
		return 0;
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
			for (j = 0; father->content[j] != 0; j++) {
				/* comparar se tem um arquivo com o mesmo nome e extensao */
				nome_cluster(father->content[j], metadata, lightfs, nome2);
				ext_cluster(father->content[j], metadata, lightfs, ext2);
				if (!strcmp(nome_upper, nome2) && !strcmp(ext_upper, ext2)) {
					printf("Erro: Ja existe um arquivo com esse nome e extensao\n");
					fclose(lightfs);
					return 0;
				}
			}

			strcpy(new.filename, nome_upper);
			strcpy(new.extension, ext_upper);
			new.flags = 0x01;
			new.index = i;
			new.next = new.index;
			new.prev = new.index;
			new.father = father->index;

			absAdd = findAbsAdd(i, metadata);
			fseek(lightfs, absAdd, SEEK_SET);
			fwrite(&new, sizeof(CLUSTER), 1, lightfs);

			father->content[j] = new.index;
			father->flags |= 0x02;
			absAdd = findAbsAdd(father->index, metadata);
			fseek(lightfs, absAdd, SEEK_SET);
			fwrite(father, sizeof(CLUSTER), 1, lightfs);

		} else { /* se nao encontrou */
			printf("Sistema de arquivos cheio!\n");
			return 0;
		}
	}
	fclose(lightfs);
	return 1;
}
int rm_aux(CLUSTER *father, METADATA metadata, char nome[], char ext[])
{
	INDEX i;
	FILE *lightfs;

	if (!(lightfs = fopen("LIGHTFS.BIN", "r+"))) {
		printf("File open error\n");
		return 0;
	} else {
		i = busca_file(metadata, *father, lightfs, nome, ext);
		fclose(lightfs);
		if (i) {
			return rm_func(father, metadata, i);
		} else {
			return 0;
		}
	}
}

int rm_func(CLUSTER *father, METADATA metadata, INDEX point)
{
	FILE *lightfs;
	unsigned long int address;
	CLUSTER cluster;
	int i;

	if (!(lightfs = fopen("LIGHTFS.BIN", "r+"))) {
		printf("File open error\n");
		return 0;
	}
	/* receber cluster do ponteiro */
	cluster = busca_cluster(point, metadata, lightfs);
	/* testar se o cluster e root */
	if (cluster.flags & 0x80) {
		printf("Erro: Nao e permitido remover root\n");
		fclose(lightfs);
		return 0;
	}
	/* testar se o cluster ja esta vazio */
	if (!(cluster.flags & 0x01)) {
		printf("Erro: Cluster vazio\n");
		fclose(lightfs);
		return 0;
	}
	/* testar se o diretorio nao esta vazio */
	if ((cluster.flags & 0x02) && (strcmp(cluster.extension, "DIR") == 0)) {
		printf("Erro: O diretorio nao esta vazio\n");
		fclose(lightfs);
		return 0;
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
		return 0;
	}
	/* pegar endereco do cluster */
	address = findAbsAdd(cluster.index, metadata);
	fseek(lightfs, address, SEEK_SET);
	/* remover recursivamente todos clusters do arquivo */
	if (cluster.next != cluster.index) {
		rm_func(father, metadata, cluster.next); // testar depois
	}
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
	cluster.next = 0;
	cluster.prev = 0;
	/* escrever cluster limpo */
	fwrite(&cluster, sizeof(CLUSTER), 1, lightfs);

	fclose(lightfs);
	return 1;
}

int rename_func(CLUSTER father, METADATA metadata, INDEX point,
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
		return 0;
	}
	if (strlen(ext) > TAM_EXT) {
		printf("Erro: extensao com mais de 3 caracteres\n");
		return 0;
	}

	if (!(lightfs = fopen("LIGHTFS.BIN","r+"))) {
		printf("File open error\n");
		return 0;
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
			return 0;
		}
	}
	/* ler cluster do arquivo */
	cluster = busca_cluster(point, metadata, lightfs);
	/* testar se o cluster e root */
	if (cluster.flags & 0x80) {
		printf("Erro: Nao e permitido renomear root\n");
		fclose(lightfs);
		return 0;
	}
	/* testar se o cluster esta vazio */
	if (!(cluster.flags & 0x01)) {
		printf("Erro: Cluster vazio\n");
		fclose(lightfs);
		return 0;
	}
	/* verificar se a extensao e .DIR */
	if (!(strcmp(cluster.extension, "DIR")) && strcmp(ext_upper, "DIR")) {
		printf("Erro: Nao e possivel trocar extensao de diretorio\n");
		fclose(lightfs);
		return 0;
	}
	if (strcmp(cluster.extension, "DIR") && !(strcmp(ext_upper, "DIR"))) {
		printf("Erro: Nao e possivel trocar extensao de arquivo texto para .DIR\n");
		fclose(lightfs);
		return 0;
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
	return 1;
}

INDEX mk_next(METADATA metadata, INDEX c_index)
{ 
	CLUSTER current, next;
	FILE *lightfs;
	int i;
	unsigned long int absAdd;

	if (!(lightfs = fopen("LIGHTFS.BIN","r+"))) {
		printf("File open error\n");
		return 0;
	} else {
		/* load current */
		current = busca_cluster(c_index, metadata, lightfs);

		/* procurar primeiro cluster livre */
		i = 0;
		do {
			absAdd = findAbsAdd(i, metadata);
			fseek(lightfs, absAdd, SEEK_SET);
			fread(&next, sizeof(CLUSTER), 1, lightfs);
			i++;
		} while ((next.flags & 0x01) && !feof(lightfs));
		i--;
		
		if (!(next.flags & 0x01)) { /* se encontrou cluster livre */
			
			strcpy(next.filename, current.filename);
			strcpy(next.extension, current.extension);
			next.flags = 0x01;
			next.index = i;
			next.next = next.index;
			next.prev = current.index;
			next.father = current.father; 

			absAdd = findAbsAdd(i, metadata);
			fseek(lightfs, absAdd, SEEK_SET);
			fwrite(&next, sizeof(CLUSTER), 1, lightfs);

			current.next = next.index;
			absAdd = findAbsAdd(current.index, metadata);
			fseek(lightfs, absAdd, SEEK_SET);
			fwrite(&current, sizeof(CLUSTER), 1, lightfs);

			return next.index;

		} else { /* se nao encontrou */
			printf("Sistema de arquivos cheio!\n");
			return 0;
		}
	}
}

int edit_func(METADATA metadata, INDEX point, char content[])
{
	CLUSTER cluster;
	unsigned long int address;
	FILE *lightfs;
	char *aux, *aux2;
	int i;
	INDEX next;

	/* testar depois */
	if (strlen(content) > TAM_CONTENT) { /* file spans more than one cluster */
		/* break the string in two */
		aux = malloc(TAM_CONTENT);
		aux2 = malloc(strlen(content - TAM_CONTENT));
		/* write aux */
		edit_func(metadata, point, aux);
		/* create next cluster */
		next = mk_next(metadata, point);
		if (next) { /* recursively writes the content */
			edit_func(metadata, next, aux2);
		} else {
			printf("Erro: Arquivo muito grande para o sistema de arquivos\n");
			return 0;
		}
		return 1;
	}	

	/* abrir arquivo */
	if (!(lightfs = fopen("LIGHTFS.BIN","r+"))) {
		printf("File open error\n");
		return 0;
	}
	/* carregar cluster na memoria */
	cluster = busca_cluster(point, metadata, lightfs);
	/* verificar se e dir */
	if (strcmp(cluster.extension, "DIR") == 0) {
		printf("Erro: Nao e possivel modificar conteudo de diretorio\n");
		return 0;
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
	return 1;
}

int cd_func(METADATA metadata, INDEX point, CLUSTER *cluster)
{ /* retorna o cluster do ponteiro, se der erro nao troca o cluster */
	FILE *lightfs;
	CLUSTER aux;
	
	if (!(lightfs = fopen("LIGHTFS.BIN","r+"))) {
		printf("File open error\n");
		return 0;
	}
	aux = busca_cluster(point, metadata, lightfs);
	if (strcmp(aux.extension, "DIR") != 0) {
		printf("Erro: so e possivel trocar para diretorios\n");
		return 0;
	} else {
		*cluster = aux;
		return 1;
	}
}

INDEX absPath2point(METADATA metadata, char path[], FILE *arq)
{
	char path_upper[MAX_INPUT] = { 0 };
	char filename[TAM_FILENAME] = { 0 };
	char extension[TAM_EXT] = { 0 };
	int i, j;
	CLUSTER cluster, aux;
	INDEX index;

	if (path[0] == 0) { /* se estiver vazio */
		return 0;
	}
	for (i = 0; path[i] != 0; i++) {
		path_upper[i] = toupper(path[i]);
	}
	/* carregar aux com root */
	aux = busca_cluster(0, metadata, arq);

	/* ler primeiro caminho do endereco */
	i = 0;
	j = 0;
	if (path_upper[i] == '/') {
		i++;
	}
	/* pegar primeiro filename e ext */
	while (path_upper[i] != '.' && path_upper[i] != 0 && j < TAM_FILENAME) {
		filename[j] = path_upper[i];
		i++;
		j++;
	}
	if (j >= TAM_FILENAME || path_upper[i] == 0) { /* nao pode acabar em \0 */
		printf("Erro: O caminho especificado nao e valido.\n");
		return 0;
	}
	i++;
	j = 0;
	while (path_upper[i] != '/' && path_upper[i] != 0 && j < TAM_EXT) {
		extension[j] = path_upper[i];
		i++;
		j++;
	}
	if (j >= TAM_EXT) {
		printf("Erro: O caminho especificado nao e valido.\n");
		return 0;
	}
	/* verificar se root foi informado */
	if (strcmp(filename, "ROOT") != 0 || strcmp(extension, "DIR") != 0) {
		printf("Erro: O caminho especificado nao e valido.\n");
		return 0;
	}
	/* limpar filename e ext */
	for (j = 0; j < TAM_FILENAME; j++) {
		filename[j] = 0;
	}
	for (j = 0; j < TAM_EXT; j++) {
		extension[j] = 0;
	}
	/* fazer o mesmo ate o final do caminho */
	while (path_upper[i] != 0) {
		j = 0;
		if (path_upper[i] == '/') {
			i++;
		}
		if (path_upper[i] == 0) {
			break;
		}
		while (path_upper[i] != '.' && path_upper[i] != 0 && j < TAM_FILENAME) {
			filename[j] = path_upper[i];
			i++;
			j++;
		}
		if (j >= TAM_FILENAME || path_upper[i] == 0) {
			printf("Erro: O caminho especificado nao e valido.\n");
			return 0;
		}
		i++;
		j = 0;
		while (path_upper[i] != '/' && path_upper[i] != 0 && j < TAM_EXT) {
			extension[j] = path_upper[i];
			i++;
			j++;
		}
		if (j >= TAM_EXT) {
			printf("Erro: O caminho especificado nao e valido.\n");
			return 0;
		}
		index = busca_file(metadata, aux, arq, filename, extension);
		/* limpar filename e ext */
		for (j = 0; j < TAM_FILENAME; j++) {
			filename[j] = 0;
		}
		for (j = 0; j < TAM_EXT; j++) {
			extension[j] = 0;
		}
		/* verificar se o caminho termina com '/' */
		if (path_upper[i] == '/') {
			i++;
		}

		if (index) {
			aux = busca_cluster(index, metadata, arq);
		} else {
			return aux.index;
		}
	}
	return aux.index;
}

int cd_aux(METADATA metadata, char path[], CLUSTER *cluster)
{
	FILE *arq;
	INDEX index;
	int i, j;
	char path_aux[MAX_INPUT] = { 0 };

	if (!(arq = fopen("LIGHTFS.BIN","r"))) {
		printf("File open error\n");
		return 0;
	}
	if (strcmp(path, "..") == 0) { /* voltar para father */
		*cluster = busca_cluster(cluster->father, metadata, arq);
		fclose(arq);
		return 1;
	}
	
	index = absPath2point(metadata, path, arq);
	fclose(arq);

	if (index == 0) { /* testar se realmente foi chamado root */
		i = 0;
		j = 0;
		if (path[i] == '/') {
			i++;
		}
		while (path[i] != '/' && path[i] != 0) {
			path_aux[j] = path[i];
			i++;
			j++;
		}
		path_aux[j] = 0;
		if (strcmp(path_aux, "ROOT.DIR") != 0) {
			printf("Erro: Diretorio nao encontrado.\n");
			return 0;
		} else {
			return cd_func(metadata, index, cluster);
		}
	}
	return cd_func(metadata, index, cluster);
}


int edit_aux(CLUSTER *father, METADATA metadata, char nome[], char ext[],
																char content[])
{
	INDEX i;
	FILE *lightfs;

	if (!(lightfs = fopen("LIGHTFS.BIN", "r+"))) {
		printf("File open error\n");
		return 0;
	} else {
		i = busca_file(metadata, *father, lightfs, nome, ext);
		fclose(lightfs);
		if (i) {
			return edit_func(metadata, i, content);
		} else {
			return 0;
		}
	}
}

void disp_func(METADATA metadata, INDEX point)
{
	CLUSTER cluster;
	FILE *arq;

	if (!(arq = fopen("LIGHTFS.BIN", "r+"))) {
		printf("File open error\n");
		return;
	}
	cluster = busca_cluster(point, metadata, arq);
	if (strcmp(cluster.extension, "DIR") == 0) {
		printf("Conteudo do diretorio %s.%s:\n", cluster.filename, cluster.extension);
		dir_func(cluster, metadata);
	} else if (cluster.flags & 0x2) {
		printf("Conteudo do arquivo %s.%s:\n", cluster.filename, cluster.extension);
		printf("%s\n", cluster.content);
	} else {
		printf("Conteudo do arquivo %s.%s:\n", cluster.filename, cluster.extension);
		printf("<Arquivo vazio>\n");
	}
	fclose(arq);
}

int disp_aux(CLUSTER *father, METADATA metadata, char nome[], char ext[])
{
	INDEX i;
	FILE *lightfs;

	if (!(lightfs = fopen("LIGHTFS.BIN", "r+"))) {
		printf("File open error\n");
		return 0;
	}
	i = busca_file(metadata, *father, lightfs, nome, ext);
	fclose(lightfs);
	if (i == 0) {
		if (strcmp(nome, "ROOT") == 0 && strcmp(ext, "DIR") == 0) {
			disp_func(metadata, i);
			return 1;
		} else {
			return 0;
		}
	}
	disp_func(metadata, i);
	return 1;
}

int rename_aux(CLUSTER *father, METADATA metadata, char nome_old[],
							   char ext_old[], char nome_new[], char ext_new[])
{
	INDEX i;
	FILE *lightfs;

	if (!(lightfs = fopen("LIGHTFS.BIN", "r+"))) {
		printf("File open error\n");
		return 0;
	}
	i = busca_file(metadata, *father, lightfs, nome_old, ext_old);
	fclose(lightfs);
	if (i == 0) {
		if (strcmp(nome_old, "ROOT") == 0 && strcmp(ext_old, "DIR") == 0) {
			return rename_func(*father, metadata, i, nome_new, ext_new);
		} else {
			return 0;
		}
	}
	return rename_func(*father, metadata, i, nome_new, ext_new);
}

int move_func(METADATA metadata, INDEX point, INDEX p_father_new)
{
	FILE *lightfs;
	unsigned long int address;
	CLUSTER cluster, father_old, father_new;
	int i, j;
	char nome2[TAM_FILENAME] = { 0 };
	char ext2[TAM_EXT] = { 0 };

	if (!(lightfs = fopen("LIGHTFS.BIN", "r+"))) {
		printf("File open error\n");
		return 0;
	}
	/* receber cluster do ponteiro */
	cluster = busca_cluster(point, metadata, lightfs);
	/* testar se o cluster e root */
	if (cluster.flags & 0x80) {
		printf("Erro: Nao e permitido mover root\n");
		fclose(lightfs);
		return 0;
	}
	/* testar se o cluster ja esta vazio */
	if (!(cluster.flags & 0x01)) {
		printf("Erro: Cluster vazio\n");
		fclose(lightfs);
		return 0;
	}
	/* receber cluster father_old */
	father_old = busca_cluster(cluster.father, metadata, lightfs);
	/* receber cluster father_new */
	father_new = busca_cluster(p_father_new, metadata, lightfs);

	/* testar se father_new e um diretorio */
	if (strcmp(father_new.extension, "DIR") != 0) {
		printf("Erro: So e possivel mover para diretorio.\n");
		return 0;
	}

	/* chegar no primeiro bloco vazio do father_new */
	for (j = 0; father_new.content[j] != 0; j++) {
		/* comparar se tem um arquivo com o mesmo nome e extensao */
		nome_cluster(father_new.content[j], metadata, lightfs, nome2);
		ext_cluster(father_new.content[j], metadata, lightfs, ext2);
		if (!strcmp(cluster.filename, nome2) && !strcmp(cluster.extension, ext2)) {
			printf("Erro: Ja existe um arquivo com esse nome e extensao em %s.%s\n",
											father_new.filename, father_new.extension);
			fclose(lightfs);
			return 0;
		}
	}
	/* limpar ponteiro em father_old */
	i = 0;
	while (father_old.content[i] != cluster.index && i < TAM_CONTENT) {
		i++;
	}
	if (father_old.content[i] == cluster.index) {
		father_old.content[i] = 0;
		do {
			father_old.content[i] = father_old.content[i+1];
			i++;
		} while (father_old.content[i]);
		if (father_old.content[0] == 0) {
			father_old.flags &= 0xFD; /* limpar nas flags */
		}
		/* escrever novo father_old no arquivo */
		address = findAbsAdd(father_old.index, metadata);
		fseek(lightfs, address, SEEK_SET);
		fwrite(&father_old, sizeof(CLUSTER), 1, lightfs);
	} else {
		printf("Erro: Cluster nao encontrado\n");
		fclose(lightfs);
		return 0;
	}
	/* escrever ponteiro em father_new */
	father_new.content[j] = cluster.index;
	father_new.flags |= 0x02;
	address = findAbsAdd(father_new.index, metadata);
	fseek(lightfs,address, SEEK_SET);
	fwrite(&father_new, sizeof(CLUSTER), 1, lightfs);
	/* escrever cluster com novo father */
	cluster.father = father_new.index;
	address = findAbsAdd(cluster.index, metadata);
	fseek(lightfs, address, SEEK_SET);
	fwrite(&cluster, sizeof(CLUSTER), 1, lightfs);
	fclose(lightfs);
	return 1;
}

int move_aux(METADATA metadata, char path_old[], char path_new[], CLUSTER *cluster)
{
	FILE *arq;
	INDEX index_cluster, index_father;
	int i, j, k;
	char path_aux_1[MAX_INPUT] = { 0 };
	char path_aux_2[MAX_INPUT] = { 0 };

	if (!(arq = fopen("LIGHTFS.BIN","r"))) {
		printf("File open error\n");
		return 0;
	}
	
	index_cluster = absPath2point(metadata, path_old, arq);
	index_father = absPath2point(metadata, path_new, arq);
	fclose(arq);

	if (index_cluster == 0) { /* testar se realmente foi chamado root */
		i = 0;
		j = 0;
		if (path_old[i] == '/') {
			i++;
		}
		while (path_old[i] != '/' && path_old[i] != 0) {
			path_aux_1[j] = path_old[i];
			i++;
			j++;
		}
		path_aux_1[j] = 0;
		if (strcmp(path_aux_1, "ROOT.DIR") != 0) {
			printf("Erro: Arquivo nao encontrado.\n");
			return 0;
		} else {
			k = move_func(metadata, index_cluster, index_father);
			refresh_cluster(cluster, metadata);
			return k;
		}
	}
	if (index_father == 0) { /* testar se realmente foi chamado root */
		i = 0;
		j = 0;
		if (path_new[i] == '/') {
			i++;
		}
		while (path_new[i] != '/' && path_new[i] != 0) {
			path_aux_2[j] = path_new[i];
			i++;
			j++;
		}
		path_aux_2[j] = 0;
		if (strcmp(path_aux_2, "ROOT.DIR") != 0) {
			printf("Erro: Diretorio nao encontrado.\n");
			return 0;
		} else {
			k = move_func(metadata, index_cluster, index_father);
			refresh_cluster(cluster, metadata);
			return k;
		}
	}
	k = move_func(metadata, index_cluster, index_father);
	refresh_cluster(cluster, metadata);
	return k;
}
