/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef __OS_GRAPH_H__
#define __OS_GRAPH_H__	1

#include <stdio.h>

typedef struct os_node_t {
	unsigned int nodeID;
	signed int nodeInfo;

	unsigned int cNeighbours;   // Neighbours count
	unsigned int *neighbours;
} os_node_t;

typedef struct os_graph_t {
	unsigned int nCount;        // Nodes count
	unsigned int eCount;        // Edges count

	os_node_t **nodes;
	unsigned int *visited;
} os_graph_t;

typedef struct os_edge_t {
	int src, dst;
} os_edge_t;

os_node_t *os_create_node(unsigned int _nodeID, int _nodeInfo);
os_graph_t *create_graph_from_data(unsigned int nc, unsigned int ec,
		int *values, os_edge_t *edges);
os_graph_t *create_graph_from_file(FILE *file);
void printGraph(os_graph_t *graph);

#endif
