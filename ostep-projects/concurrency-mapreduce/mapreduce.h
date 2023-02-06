#include "../lib/ht.h"
#include "../lib/list.h"
#include <string.h>

#include "common_threads.h"

#ifndef __mapreduce_h__
#define __mapreduce_h__

// Different function pointer types used by MR
typedef char *(*Getter)(char *key, int partition_number); // Why does Getter need the partition number?
typedef void *(*Mapper)(void *file_name);
typedef void (*Reducer)(char *key, Getter get_func, int partition_number);
typedef unsigned long (*Partitioner)(char *key, int num_partitions);

ht *wordsTable;
ht *keyAccessTable;

typedef struct Entry
{
	char *word;
	char *countToken;
} Entry;

typedef struct __wlock_t
{
	sem_t writelock;
} wlock_t;

wlock_t wlock;

void wlock_init(wlock_t *w)
{
	Sem_init(&(w->writelock), 1);
}

void wlock_aquire_write(wlock_t *w)
{
	Sem_wait(&(w->writelock));
}

void wlock_release_write(wlock_t *w)
{
	Sem_post(&(w->writelock));
}

// External functions: these are what you must define
void MR_Emit(char *key, char *value);

unsigned long MR_DefaultHashPartition(char *key, int num_partitions);

void MR_Run(int argc, char *argv[],
			Mapper map, int num_mappers,
			Reducer reduce, int num_reducers,
			Partitioner partition);

unsigned long MR_DefaultHashPartition(char *key, int num_partitions)
{
	// use this function to decide which partition (and hence, which reducer thread)
	// gets a particular key / list of values to process
	unsigned long hash = 5381;
	int c;
	while ((c = *key++) != '\0')
		hash = hash * 33 + c;
	return hash % num_partitions;
}

void MR_Emit(char *key, char *value)
{
	// takes a key/value pair and stores it in a data structure
	// { [key: string]: string[] values };  -> the last item in the values list is null '\0'
	// Invoked as in Map():
	// MR_Emit(token, "1");
	Entry *entry = (Entry *)malloc(sizeof(Entry));

	entry->word = key;
	entry->countToken = value;

	wlock_aquire_write(&wlock);

	void *currentEntry = ht_get(wordsTable, entry->word);

	if (currentEntry == NULL)
	{
		list_node *list = list_init();
		list_insert(list, entry);

		if (ht_set(wordsTable, entry->word, list) == NULL)
		{
			exit(1);
		}
	}
	else
	{
		list_node *listHead = (list_node *)currentEntry;
		list_insert(listHead, entry);
	}

	wlock_release_write(&wlock);
}

void MR_Clean()
{
	hti wordsIt = ht_iterator(wordsTable);
	hti accessIt = ht_iterator(keyAccessTable);

	while (ht_next(&wordsIt))
	{
		list_free((list_node *)wordsIt.value);
	}

	while (ht_next(&accessIt))
	{
		free(accessIt.value);
	}

	ht_destroy(wordsTable);
	ht_destroy(keyAccessTable);
}

char *getNext(char *key, int partition_number)
{
	int currentIndex = 0;
	void *savedIndex = ht_get(keyAccessTable, key);

	if (savedIndex == NULL)
	{
		int *index = (int *)malloc(sizeof(int));
		*(index) = 1;
		if (ht_set(keyAccessTable, key, (void *)index) == NULL)
		{
			exit(1);
		}
	}
	else
	{
		currentIndex = *((int *)savedIndex);
		int *nextIndexToSave = (int *)malloc(sizeof(int));
		*(nextIndexToSave) = currentIndex + 1;
		free(savedIndex);
		if (ht_set(keyAccessTable, key, (void *)nextIndexToSave) == NULL)
		{
			exit(1);
		}
	}

	void *wordEntry = ht_get(wordsTable, key);
	if (wordEntry == NULL)
	{
		exit(1);
	}
	list_node *listHead = (list_node *)wordEntry;
	list_node *item = list_get(listHead, currentIndex);
	if (item == NULL)
	{
		return NULL;
	}
	char *token = (char *)malloc(sizeof(char));
	strcpy(token, ((Entry *)item->data)->countToken);
	return token;
}

void MR_Run(int argc, char *argv[],
			Mapper map, int num_mappers,
			Reducer reduce, int num_reducers,
			Partitioner partition)
{
	wlock_init(&wlock);

	wordsTable = ht_create();
	keyAccessTable = ht_create();

	int mappers_count = argc - 1;
	pthread_t mapper_threads[mappers_count];

	for (size_t i = 1; i < argc; i++)
	{
		Pthread_create(&mapper_threads[i - 1], NULL, map, argv[i]);
	}

	for (size_t i = 0; i < mappers_count; i++)
	{
		Pthread_join(mapper_threads[i], NULL); // will "wait" for all mapper threads to finish.
	}

	hti wordsIt = ht_iterator(wordsTable);

	while (ht_next(&wordsIt))
	{
		// Where do I create threads and include locking for the reudcer_threads?
		reduce((char *)wordsIt.key, getNext, 1);
	}

	MR_Clean();
}

#endif // __mapreduce_h__
