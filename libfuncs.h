#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>
//#include "libfs.h"


unsigned long int findAbsAdd(INDEX point, METADATA metadata);

CLUSTER busca_cluster(INDEX point, METADATA metadata, FILE *arq);

void nome_cluster(INDEX point, METADATA metadata, FILE *arq, char n[]);

void ext_cluster(INDEX point, METADATA metadata, FILE *arq, char n[]);

void dir_func(CLUSTER cluster, METADATA metadata);

void mk_func(CLUSTER *cluster, METADATA metadata, char nome[], char ext[]);

void rm_func(CLUSTER *father, METADATA metadata, INDEX point);

void rename_func(CLUSTER father, METADATA metadata, INDEX point,
													   char nome[], char ext[]);

void edit_func(METADATA metadata, INDEX point, char content[]);