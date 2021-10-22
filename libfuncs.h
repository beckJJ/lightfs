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

void refresh_cluster(CLUSTER *cluster, METADATA metadata);

INDEX busca_file(METADATA metadata, CLUSTER father, FILE *arq,
													   char nome[], char ext[]);

void dir_func(CLUSTER cluster, METADATA metadata);

int mk_func(CLUSTER *cluster, METADATA metadata, char nome[], char ext[]);

int rm_aux(CLUSTER *father, METADATA metadata, char nome[], char ext[]);

int rm_func(CLUSTER *father, METADATA metadata, INDEX point);

int rename_func(CLUSTER father, METADATA metadata, INDEX point,
													   char nome[], char ext[]);

int edit_func(METADATA metadata, INDEX point, char content[]);

int cd_func(METADATA metadata, INDEX point, CLUSTER *cluster);

INDEX absPath2point(METADATA metadata, char path[], FILE *arq);

int cd_aux(METADATA metadata, char path[], CLUSTER *cluster);

int edit_aux(CLUSTER *father, METADATA metadata, char nome[], char ext[],
															   char content[]);

int disp_aux(CLUSTER *father, METADATA metadata, char nome[], char ext[]);

int rename_aux(CLUSTER *father, METADATA metadata, char nome_old[],
							  char ext_old[], char nome_new[], char ext_new[]);

int move_func(METADATA metadata, INDEX point, INDEX p_father_new);

int move_aux(METADATA metadata, char path_old[], char path_new[], CLUSTER *cluster);
