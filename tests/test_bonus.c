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

void only_test(void **state)
{
	(void) state;

	int fds[3];
	char *line;
	bool done[3] = {0};
	bool exit;

	fds[0] = open("mobydick.txt", O_RDONLY);
	fds[1] = open("quijote.txt", O_RDONLY);
	fds[2] = open("book1.txt", O_RDONLY);

	int size = sizeof(fds) / sizeof(int);

	for (int i = 0; i < size; ++i)
		if (fds[i] < 0)
			fail();

	for (int i = 0;; i = (i + 1) % size)
	{
		if ((line = get_next_line(fds[i])) == NULL)
		{
			done[i] = true;
			exit = true;
			for (int j = 0; j < size; ++j)
				exit &= done[j];
			if (exit)
			{
				for (int j = 0; j < size; ++j)
					close(fds[j]);
				return;
			}
			continue;
		}
		printf("%s", line);
		free(line);
	}
}

int main(void) {
    const struct CMUnitTest tests[] = {
		cmocka_unit_test(only_test),
    };
 
    return cmocka_run_group_tests(tests, NULL, NULL);
}
