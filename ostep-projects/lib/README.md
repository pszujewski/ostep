gcc ./test-ht.c ../ht.c -o ./test-ht -Wall -Werror

gcc ./test-list.c ../list.c -o ./test-list -Wall -Werror

gcc ./test-ht-list.c ../list.c ../ht.c -o ./test-ht-list -Wall -Werror

add `-g` to allow debugging.

## To try sometime

To try: Try to "install" glib under /usr/include. Although what if it has dependencies?
https://developer.ibm.com/tutorials/l-glib/

## Valgrind

To check if there is any risk of memory leaks.

https://stackoverflow.com/questions/5134891/how-do-i-use-valgrind-to-find-memory-leaks

for example:

`valgrind --leak-check=full --verbose ./tests/test-ht-list`

Should see something like the following:

```
==3283== HEAP SUMMARY:
==3283==     in use at exit: 0 bytes in 0 blocks
==3283==   total heap usage: 30 allocs, 30 frees, 1,807 bytes allocated
==3283==
==3283== All heap blocks were freed -- no leaks are possible
==3283==
==3283== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
==3283== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```
