#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CLR_RESET "\033[0m"

#define CLR_RED "\033[31m"
#define CLR_YELLOW "\033[33m"
#define CLR_BLUE "\033[34m"

#define BUF_SIZE (8 * 1024)
#define MAPS_SIZE 128 /* example value */

typedef struct Mapping {
	unsigned long start;
	unsigned long end;
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
		if (count >= MAPS_SIZE) break;

		Mapping *cur_map = maps + count;

		unsigned long start, end;
		int file_offset, inode;
		int dev[2];
		char perms[5];
		char pathname[64];
		sscanf(line, "%lx-%lx %4s %x %x:%x %d %63s", &start, &end, perms, &file_offset, &dev[0], &dev[1], &inode, pathname);

		cur_map->start = start;	
		cur_map->end = end;
		memcpy(cur_map->perms, perms, 5);
		cur_map->file_offset = file_offset;
		memcpy(cur_map->dev, dev, sizeof(dev));
		cur_map->inode = inode;
		memcpy(cur_map->pathname, pathname, 64);

		count++;
	}

	fclose(maps_file);
	return count;
}

void print_mapping(Mapping *m) {
	printf("mapping for %s with offset %x with " CLR_YELLOW"%s"CLR_RESET" perms\n", m->pathname, m->file_offset, m->perms);
	printf("starts at " CLR_RED"%lx"CLR_RESET", ends at " CLR_RED"%lx"CLR_RESET", total %ld bytes\n", m->start, m->end, (m->end - m->start));
	printf("device " CLR_BLUE"%d"CLR_RESET":"CLR_BLUE"%d"CLR_RESET", inode number "CLR_BLUE"%d\n"CLR_RESET, m->dev[0], m->dev[1], m->inode);
}

void print_sep(void) {
	for (int i = 0; i < 80; i++) putchar('=');
	putchar('\n');
}

int main(void) {

	Mapping *maps = calloc(MAPS_SIZE, sizeof(Mapping));
	
	int n = parse_mappings(maps);

	for (int i = 0; i < n; i++) {
		print_mapping(maps + i);
		print_sep();
	}

	return 0;
}
