#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include "../get_next_line_bonus.h"

/* make CPPFLAGS=-DBUFFER_SIZE=4096 test_bonus && make run_bonus_tests &>/dev/null && diff final2.txt <(awk ' */
/* BEGIN { */
/*     found = 1 */
/*     while (found) { */
/*         found = 0 */
/*         if (getline < "mobydick.txt") { */
/*             found = 1 */
/*             print */
/*         } */
/*         if (getline < "quijote.txt") { */
/*             found = 1 */
/*             print */
/*         } */
/*         if (getline < "book1.txt") { */
/*             found = 1 */
/*             print */
/*         } */
/*     } */
/*     exit */
/* } */
/* ') */ 

void only_test(void **state)
{
	(void) state;

	int fds[3];
	char *line;
	bool found;
	bool fdone[3] = {0};

	fds[0] = open("mobydick.txt", O_RDONLY);
	fds[1] = open("quijote.txt", O_RDONLY);
	fds[2] = open("book1.txt", O_RDONLY);
	FILE *output = fopen("final2.txt", "w");

	int size = sizeof(fds) / sizeof(int);

	for (int i = 0; i < size; ++i)
	{
		if (fds[i] < 0)
			fail();
	}

	found = true;
	while (found)
	{
		found = false;
		for (int i = 0; i < size; ++i)
		{
			if (!fdone[i])
			{
				line = get_next_line(fds[i]);
				fdone[i] = line == NULL;
				if (line != NULL)
				{
					fprintf(output, "%s", line);
					found = true;
					free(line);
				}
			}
		}
	}
	fclose(output);
}

int main(void) {
    const struct CMUnitTest tests[] = {
		cmocka_unit_test(only_test),
    };
 
    return cmocka_run_group_tests(tests, NULL, NULL);
}
