#include "../lib/ht.h"
#include "../lib/list.h"

#ifndef __mapreduce_h__
#define __mapreduce_h__

// Different function pointer types used by MR
typedef char *(*Getter)(char *key, int partition_number); // Why does Getter need the partition number?
typedef void (*Mapper)(char *file_name);
typedef void (*Reducer)(char *key, Getter get_func, int partition_number);
typedef unsigned long (*Partitioner)(char *key, int num_partitions);

ht *wordsTable;
ht *keyAccessTable;

typedef struct Entry
{
	char *word;
	char *countToken;
} Entry;

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
	//
	Entry *entry = malloc(sizeof(Entry));

	entry->word = key;
	entry->countToken = value;
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
		int *index = malloc(sizeof(int));
		*(index) = 1;
		if (ht_set(keyAccessTable, key, (void *)index) == NULL)
		{
			exit(1);
		}
	}
	else
	{
		currentIndex = *((int *)savedIndex);
		int *nextIndexToSave = malloc(sizeof(int));
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
	return ((Entry *)item->data)->countToken;
}

void MR_Run(int argc, char *argv[],
			Mapper map, int num_mappers,
			Reducer reduce, int num_reducers,
			Partitioner partition)
{
	wordsTable = ht_create();
	keyAccessTable = ht_create();

	map("./lorem.txt");
	hti wordsIt = ht_iterator(wordsTable);

	while (ht_next(&wordsIt))
	{
		reduce((char *)wordsIt.key, getNext, 1);
	}

	MR_Clean();
}

#endif // __mapreduce_h__
