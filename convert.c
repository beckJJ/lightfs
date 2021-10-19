// convert LIGHTFS.BIN LE <-> BE
#include <stdio.h>
#include <stdlib.h>

void changeEndianness(void)
{
	FILE *lightfs;
	unsigned char aux = 0, aux2 = 0;
	unsigned char index_size[2], cluster_size[2];
	unsigned char index_start[2], first_cluster[2];
	
	if (!(lightfs = fopen("LIGHTFS.BIN","r+"))) {
		printf("File open error\n");
		exit(1);
	} else {
		fread(&index_size[1], sizeof(char), 1, lightfs);
		fread(&index_size[0], sizeof(char), 1, lightfs);

		fread(&cluster_size[1], sizeof(char), 1, lightfs);
		fread(&cluster_size[0], sizeof(char), 1, lightfs);

		fread(&index_start[1], sizeof(char), 1, lightfs);
		fread(&index_start[0], sizeof(char), 1, lightfs);

		fread(&first_cluster[1], sizeof(char), 1, lightfs);
		fread(&first_cluster[0], sizeof(char), 1, lightfs);

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
	changeEndianness();
}