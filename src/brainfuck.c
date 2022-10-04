#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define __CASE__(v1,v2) if(v1 == v2)

char *chomp(char *s) {
	int l = strlen(s);
	switch(s[--l]) {
	case '\r': case '\n':
		s[l] = 0; chomp(s);
	} return s;
}

int main(int argc, char **argv) {
	FILE *fp;
	fp = (argc <= 1 ? stdin : fopen(argv[0], "r"));
	if(fp == NULL) {
		fprintf(stderr, "brainfuck: ");
		perror(argv[0]);
		return 1;
	}
	char *cpu;
	cpu = calloc(30000, sizeof(char));
	if(cpu == NULL) {
		fprintf(stderr, "brainfuck: failed to allocate virtual memory");
		fclose(fp);
		return 2;
	}
	char *buf;
	buf = calloc(16384, sizeof(char));
	if(buf == NULL) {
		fprintf(stderr, "brainfuck: failed to allocate buffer");
		fclose(fp);
		free(cpu);
		return 3;
	}
	unsigned int j=0;
	int brc = 0;
	if(fp == stdin)
		printf("Brainfuck interpreter, cpu=%d bytes.", 30000);
	for(; !feof(fp); ) {
		if(fp == stdin)
			printf("\n>> ");
		fgets(buf, 4096, fp);
		chomp(buf);
		if(fp == stdin && strncmp(buf, "/", 1) == 0) {
			char cmd[4096];
			strcpy(cmd, &buf[1]);
			if(strcmp(cmd, "dump") == 0) {
				FILE *dumpf = fopen("dump.bf", "wb");
				if(dumpf == NULL) {
					fprintf(stderr, "Failed to open dump.bf!\n");
					continue;
				}
				fwrite(cpu, sizeof(char), 30001, dumpf);
				fclose(dumpf);
				printf("Successfully dumped.");
				continue;
			} else if(strcmp(cmd, "stats") == 0) {
				if(j < 0 || j > 30000) {
					fprintf(stderr, "Invalid address: `%d`.\nFalling to 0.\n", j);
					j = 0;
				}
				printf("Current gate:\n"
"- Value: %d\n"
"- Value (ASCII): '%c'\n"
"- Index: %d\n"
"Arguments count: %d\n", cpu[j], cpu[j], j, argc);
			} else if(strcmp(cmd, "exit") == 0) {
				printf("brainfuck: exit");
				free(cpu);
				free(buf);
				return 0;
			} else if(strcmp(cmd, "reset") == 0) {
				printf("Reseting memory...\n");
				memset(cpu, 0, 30000);
				printf("Reseting stats...\n");
				j = 0;
				printf("Successfully reset interpreter.\n");
			} else {
				fprintf(stderr, "Unknown command: \"%s\"\n", cmd);
			}
			continue;
		}
		int len = strlen(buf);
		for(int ch, i=0;i < len; i++) {
			ch = buf[i];
			if(j < 0 || j > 30000) {
				fprintf(stderr, "Invalid address: `%d`.\nFalling to 0.\n", j);
				j = 0;
				continue;
			}
			if(ch == '\r' || ch == '\n' || ch == '\t')
				continue;
			__CASE__(buf[i], '>') {
				j++;
			} __CASE__(buf[i], '<') {
				j--;
			} __CASE__(buf[i], '+') {
				cpu[j]++;
			} __CASE__(buf[i], '-') {
				cpu[j]--;
			} __CASE__(buf[i], '.') {
				putchar(cpu[j]);
			} __CASE__(buf[i], ',') {
				cpu[j] = getchar();
			} __CASE__(buf[i], '[') {
				if(!cpu[j]) {
					++brc;
					while(brc) {
						++i;
						if(buf[i] == '[')
							++brc;
						if(buf[i] == ']')
							--brc;
					}
				} else
					continue;
				break;
			} else __CASE__(buf[i], ']') {
				if(!cpu[j])
					continue;
				else {
					if(buf[i] == ']')
							brc++;
					while(brc) {
						--i;
						if(buf[i] == '[')
							brc--;
						if(buf[i] == ']')
							brc++;
					}
					--i;
				}
			}
		}
	}
	free(cpu);
	free(buf);
	return 0;
}