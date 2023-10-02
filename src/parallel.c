// SPDX-License-Identifier: BSD-3-Clause

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

#include "os_graph.h"
#include "os_threadpool.h"
#include "os_list.h"

#define MAX_TASK 100
#define MAX_THREAD 4

static int sum;
static os_graph_t *graph;

int main(int argc, char *argv[])
{
	FILE *input_file;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s input_file\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	input_file = fopen(argv[1], "r");
	if (input_file == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	graph = create_graph_from_file(input_file);
	if (graph == NULL) {
		fprintf(stderr, "[Error] Can't read the graph from file\n");
		exit(EXIT_FAILURE);
	}

	/* TODO: Create thread pool and traverse the graph. */

	printf("%d", sum);

	return 0;
}
