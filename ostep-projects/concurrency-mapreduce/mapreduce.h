#ifndef __mapreduce_h__
#define __mapreduce_h__

// Different function pointer types used by MR
typedef char *(*Getter)(char *key, int partition_number); // Why does Getter need the partition number?
typedef void (*Mapper)(char *file_name);
typedef void (*Reducer)(char *key, Getter get_func, int partition_number);
typedef unsigned long (*Partitioner)(char *key, int num_partitions);

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
	return;
}

void MR_Run(int argc, char *argv[],
			Mapper map, int num_mappers,
			Reducer reduce, int num_reducers,
			Partitioner partition)
{
	return;
}

#endif // __mapreduce_h__
