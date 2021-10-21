gcc ./test-ht.c ../ht.c -o ./test-ht -Wall -Werror 

gcc ./test-list.c ../list.c -o ./test-list -Wall -Werror

add `-g` to allow debugging.

## To try sometime

To try: Try to "install" glib under /usr/include. Although what if it has dependencies?
https://developer.ibm.com/tutorials/l-glib/

## Valgrind

https://stackoverflow.com/questions/5134891/how-do-i-use-valgrind-to-find-memory-leaks