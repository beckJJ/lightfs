// convert LIGHTFS.BIN LE <-> BE
#include <stdio.h>
#include <stdlib.h>

void big2little(void)
{
	FILE *lightfs;
	unsigned char aux = 0, aux2 = 0;
	unsigned char index_size[2], cluster_size[2];
	unsigned char index_start[2], first_cluster[2];
	
	if (!(lightfs = fopen("LIGHTFS.BIN","r+"))) {
		printf("File open error\n");
		exit(1);
	} else {
		fread(&aux, sizeof(char), 1, lightfs);
		fread(&aux2, sizeof(char), 1, lightfs);
		index_size[0] = aux2;
		index_size[1] = aux;

		fread(&aux, sizeof(char), 1, lightfs);
		fread(&aux2, sizeof(char), 1, lightfs);
		cluster_size[0] = aux2;
		cluster_size[1] = aux;

		fread(&aux, sizeof(char), 1, lightfs);
		fread(&aux2, sizeof(char), 1, lightfs);
		index_start[0] = aux2;
		index_start[1] = aux;

		fread(&aux, sizeof(char), 1, lightfs);
		fread(&aux2, sizeof(char), 1, lightfs);
		first_cluster[0] = aux2;
		first_cluster[1] = aux;

		rewind(lightfs);

		fwrite(&index_size, sizeof(short), 1, lightfs);
		fwrite(&cluster_size, sizeof(short), 1, lightfs);
		fwrite(&index_start, sizeof(short), 1, lightfs);
		fwrite(&first_cluster, sizeof(short), 1, lightfs); 
	}
	fclose(lightfs);
}


void little2big(void)
{
	FILE *lightfs;
	unsigned short aux = 0, aux2 = 0;
	unsigned short index_size, cluster_size, index_start, first_cluster;
	
	if (!(lightfs = fopen("LIGHTFS.BIN","r+"))) {
		printf("File open error\n");
		exit(1);
	} else {
		fread(&aux, sizeof(char), 1, lightfs);
		aux <<= 8;
		fread(&aux2, sizeof(char), 1, lightfs);
		printf("%x %x\n", aux, aux2);
		aux |= aux2;
		index_size = aux;

		fread(&aux, sizeof(char), 1, lightfs);
		aux <<= 8;
		fread(&aux2, sizeof(char), 1, lightfs);
		aux |= aux2;
		cluster_size = aux;

		fread(&aux, sizeof(char), 1, lightfs);
		aux <<= 8;
		fread(&aux2, sizeof(char), 1, lightfs);
		aux |= aux2;
		index_start = aux;

		fread(&aux, sizeof(char), 1, lightfs);
		aux <<= 8;
		fread(&aux2, sizeof(char), 1, lightfs);
		aux |= aux2;
		first_cluster = aux;

		rewind(lightfs);

		fwrite(&index_size, sizeof(short), 1, lightfs);
		fwrite(&cluster_size, sizeof(short), 1, lightfs);
		fwrite(&index_start, sizeof(short), 1, lightfs);
		fwrite(&first_cluster, sizeof(short), 1, lightfs); 
	}
	fclose(lightfs);
}

int main(void)
{
	int c;
	printf("0: big2little\n");
	printf("1: little2big\n");
	scanf(" %d", c);
	c %= 2;
	
	switch (c) {
		case 0:
			big2little();
			return 0;
		case 1:
			little2big();
			return 0;
	}
}