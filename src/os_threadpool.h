/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef __OS_THREADPOOL_H__
#define __OS_THREADPOOL_H__	1

#include <pthread.h>
#include "os_list.h"

typedef struct {
	void *argument;
	void (*action)(void *arg);
	void (*destroy_arg)(void *arg);
	os_list_node_t list;
} os_task_t;

typedef struct os_threadpool {
	unsigned int num_threads;
	pthread_t *threads;

	/*
	 * Head of queue used to store tasks.
	 * First item is head.next, if head.next != head (i.e. if queue
	 * is not empty).
	 * Last item is head.prev, if head.prev != head (i.e. if queue
	 * is not empty).
	 */
	os_list_node_t head;

	/* TODO: Define threapool / queue synchronization data. */
	enum {
		INITIALIZED = 0,
		IN_PROGRESS = 1,
		FINISHED = 2
	} state;

	int blocked_thread_cnt;

	pthread_mutex_t queue_mutex;			// for accessing queue members
	pthread_mutex_t blocked_thread_mutex;	// for accessing blocked_thread_cnt
	pthread_cond_t waiting_cond;			// cond for threads to wait
	pthread_cond_t check_finish_cond;		// cond for main thread to change state to FINISHED

	pthread_mutex_t enum_mutex;

} os_threadpool_t;

os_task_t *create_task(void (*f)(void *), void *arg, void (*destroy_arg)(void *));
void destroy_task(os_task_t *t);

os_threadpool_t *create_threadpool(unsigned int num_threads);
void destroy_threadpool(os_threadpool_t *tp);

void enqueue_task(os_threadpool_t *q, os_task_t *t);
os_task_t *dequeue_task(os_threadpool_t *tp);
void wait_for_completion(os_threadpool_t *tp);

#endif
