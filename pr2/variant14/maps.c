#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUF_SIZE (8 * 1024)
#define MAPS_SIZE 128 /* example value */

typedef struct Mapping {
	int start;
	int end;
	char perms[5];
	int file_offset;
	int dev[2];
	int inode;
	char pathname[64];
} Mapping;

int parse_mappings(Mapping *maps) {
	FILE *maps_file = fopen("/proc/self/maps", "r");

	char line[BUF_SIZE];
	int count = 0;
	
	while (fgets(line, sizeof(line), maps_file)) {
		if (count > MAPS_SIZE) break;

		Mapping *cur_map = maps + count;

		int start, end, file_offset, inode;
		int dev[2];
		char perms[5];
		char pathname[64];
		sscanf(line, "%x-%x %4s %x %2d:%2d %d %63s", &start, &end, perms, &file_offset, &dev[0], &dev[1], &inode, pathname);

		cur_map->start = start;	
		cur_map->end = end;
		memcpy(cur_map->perms, perms, 5);
		cur_map->file_offset = file_offset;
		memcpy(cur_map->dev, dev, 2);
		cur_map->inode = inode;
		memcpy(cur_map->pathname, pathname, 64);

		count++;
	}
	return count;
}

void print_mapping(Mapping *m) {
	printf("mapping for %s with offset %x with %s perms\n", m->pathname, m->file_offset, m->perms);
	printf("starts at %x, ends at %x, total %d bytes\n", m->start, m->end, (m->end - m->start));
	printf("device %d:%d, inode number %d\n\n", m->dev[0], m->dev[1], m->inode);
}

int main(void) {

	Mapping *maps = calloc(MAPS_SIZE, sizeof(Mapping));
	
	int n = parse_mappings(maps);

	for (int i = 0; i < n; i++) {
		print_mapping(maps + i);
	}

	return 0;
}
