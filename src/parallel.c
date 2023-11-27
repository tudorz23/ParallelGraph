// SPDX-License-Identifier: BSD-3-Clause

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

#include "os_graph.h"
#include "os_threadpool.h"
#include "log/log.h"
#include "utils.h"

#define NUM_THREADS		4

static int sum;
static os_graph_t *graph;
static os_threadpool_t *tp;


/* TODO: Define graph synchronization mechanisms. */
pthread_mutex_t sum_mutex;
pthread_mutex_t check_node_mutex;



/* TODO: Define graph task argument. */
typedef struct graph_task_arg {
	unsigned int idx;
} graph_task_arg_t;

static void process_node(unsigned int idx);



static void destroy_graph_task_arg(void *arg) {
	graph_task_arg_t *graph_arg = (graph_task_arg_t *)arg;
	free(graph_arg);
}



void process_task(void *arg) {
	graph_task_arg_t *graph_arg = (graph_task_arg_t *)arg;

	os_node_t *node = graph->nodes[graph_arg->idx];

	pthread_mutex_lock(&check_node_mutex);
	if (graph->visited[graph_arg->idx] != NOT_VISITED) {
		pthread_mutex_unlock(&check_node_mutex);
		return;
	}

	graph->visited[graph_arg->idx] = PROCESSING;

	pthread_mutex_unlock(&check_node_mutex);


	pthread_mutex_lock(&sum_mutex);
	sum += node->info;
	printf("Current sum is (%d)\n", sum);
	pthread_mutex_unlock(&sum_mutex);

	for (int i = 0; i < node->num_neighbours; i++) {
		pthread_mutex_lock(&check_node_mutex);

		if (graph->visited[node->neighbours[i]] == NOT_VISITED) {
			process_node(node->neighbours[i]);
		}

		pthread_mutex_unlock(&check_node_mutex);
	}

	pthread_mutex_lock(&check_node_mutex);
	graph->visited[graph_arg->idx] = DONE;
	pthread_mutex_unlock(&check_node_mutex);
}

static void process_node(unsigned int idx)
{
	/* TODO: Implement thread-pool based processing of graph. */
	graph_task_arg_t *graph_arg = malloc(sizeof(graph_task_arg_t));
	DIE(!graph_arg, "malloc");

	graph_arg->idx = idx;

	os_task_t *new_task = create_task(process_task, graph_arg,
									  destroy_graph_task_arg);

	enqueue_task(tp, new_task);
}

int main(int argc, char *argv[])
{
	FILE *input_file;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s input_file\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	input_file = fopen(argv[1], "r");
	DIE(input_file == NULL, "fopen");

	graph = create_graph_from_file(input_file);

	/* TODO: Initialize graph synchronization mechanisms. */
	pthread_mutex_init(&sum_mutex, NULL);
	pthread_mutex_init(&check_node_mutex, NULL);

	tp = create_threadpool(NUM_THREADS);
	process_node(0);
	wait_for_completion(tp);
	destroy_threadpool(tp);

	pthread_mutex_destroy(&sum_mutex);
	pthread_mutex_destroy(&check_node_mutex);

	printf("%d\n", sum);

	return 0;
}
