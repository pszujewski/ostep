#include "../ht.h"
#include "../list.h"

#include <stdio.h>
#include <stdlib.h>

/**
 * @brief
 * Demonstrates how to construct a hash table (ht.c) containing lists (list.c)
 * at each "key" of the table.
 */

// gcc ./test-ht-list.c ../list.c ../ht.c -o ./test-ht-list -Wall -Werror -g

typedef struct Entry
{
    char *word;
    int count;
} Entry;

#define NUM_STATES 3
#define ENTRIES_PER_STATE 4

char *states[NUM_STATES] = {"Maine", "Michigan", "Florida"};

int main()
{
    ht *wordsByOccurence = ht_create();
    int total = NUM_STATES * ENTRIES_PER_STATE;

    for (size_t i = 0; i < total; i++)
    {
        Entry *entry = malloc(sizeof(Entry));

        // This will be the "key" into the table.
        entry->word = states[i % NUM_STATES];
        entry->count = 1;

        void *currentEntry = ht_get(wordsByOccurence, entry->word);

        if (currentEntry == NULL)
        {
            // Create a new list for "key"
            list_node *list = list_init();
            list_insert(list, entry);
            // Insert the list into the table at "key"
            if (ht_set(wordsByOccurence, entry->word, list) == NULL)
            {
                exit(1);
            }
        }
        else
        {
            // A list already exists at that "key"
            list_node *listHead = (list_node *)currentEntry;
            list_insert(listHead, entry);
        }
    }

    // "Assert" that the lists were created correctly.
    list_node *stateEntries = (list_node *)ht_get(wordsByOccurence, states[1]);
    size_t size = list_get_size(stateEntries);

    for (size_t i = 0; i < size; i++)
    {
        list_node *entryList = list_get(stateEntries, i);
        Entry *foundStateEntry = (Entry *)entryList->data;
        printf("Found state: %s\n", (foundStateEntry->word));
        printf("Found occurence: %d\n", (foundStateEntry->count));
    }

    stateEntries = (list_node *)ht_get(wordsByOccurence, states[0]);
    size = list_get_size(stateEntries);

    printf("Size of list for state %s = %ld\n", states[0], size);

    // Free all "values" in the hash table. Which in this case means
    // each list at every found key in the table. All must be freed
    // Before the table itself can be "destroyed"/"freed".
    hti it = ht_iterator(wordsByOccurence);

    while (ht_next(&it))
    {
        printf("Free list at key: %s\n", it.key);
        list_free((list_node *)it.value);
    }

    ht_destroy(wordsByOccurence);
}