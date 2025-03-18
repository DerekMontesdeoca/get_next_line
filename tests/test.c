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
#include "../get_next_line.h"

static void test_gnl(const char *text, const char **expected, int size) {
	int		pip[2];
	char	*line;

	line = NULL;
	if (pipe(pip) < 0)
		fail_msg("pipe failed: %s", strerror(errno));
	write(pip[1], text, strlen(text));
	close(pip[1]);
	pip[1] = -1;
	for (int i = 0; i < size; ++i)
	{
		line = get_next_line(pip[0]);
		assert_non_null(line);
		assert_memory_equal(line, expected[i], strlen(expected[i]));
		free(line);
	}
	line = get_next_line(pip[0]);
	assert_null(line);
	close(pip[0]);
}

static void test_gnl_with_diff_fd(const char *text, const char **expected, int size, int old_fd) {
	int		pip[2];
	char	*line;

	line = NULL;
	if (pipe(pip) < 0)
		fail_msg("pipe failed: %s", strerror(errno));
	if (pip[0] == old_fd)
	{
		pip[0] = dup(pip[0]);
		assert_true(pip[0] >= 0);
		close(old_fd);
	}
	write(pip[1], text, strlen(text));
	close(pip[1]);
	pip[1] = -1;
	for (int i = 0; i < size; ++i)
	{
		line = get_next_line(pip[0]);
		assert_non_null(line);
		assert_memory_equal(line, expected[i], strlen(expected[i]));
		free(line);
	}
	line = get_next_line(pip[0]);
	assert_null(line);
	close(pip[0]);
}

static void test_gnl_not_full(const char *text, const char **expected, int size, int *r_fd) {
	int		pip[2];
	char	*line;

	line = NULL;
	if (pipe(pip) < 0)
		fail_msg("pipe failed: %s", strerror(errno));
	*r_fd = pip[0];
	write(pip[1], text, strlen(text));
	close(pip[1]);
	pip[1] = -1;
	for (int i = 0; i < size; ++i)
	{
		line = get_next_line(pip[0]);
		assert_non_null(line);
		assert_memory_equal(line, expected[i], strlen(expected[i]));
		free(line);
	}
	close(pip[0]);
}


static void test_2_gnl(const char *text, const char **expected, int size) {
	int		pip[2];
	int		pip2[2];
	char	*line;

	line = NULL;
	if (pipe(pip) < 0)
		fail_msg("pipe failed: %s", strerror(errno));
	if (pipe(pip2) < 0)
		fail_msg("pipe failed: %s", strerror(errno));
	write(pip[1], text, strlen(text));
	close(pip[1]);
	pip[1] = -1;
	for (int i = 0; i < size; ++i)
	{
		line = get_next_line(pip[0]);
		assert_non_null(line);
		assert_memory_equal(line, expected[i], strlen(expected[i]));
		free(line);
	}
	line = get_next_line(pip[0]);
	assert_null(line);

	dup2(pip2[0], pip[0]);
	if (pip2[0] != pip[0])
		close(pip2[0]);
	write(pip2[1], text, strlen(text));
	close(pip2[1]);
	pip2[1] = -1;
	for (int i = 0; i < size; ++i)
	{
		line = get_next_line(pip[0]);
		assert_non_null(line);
		assert_memory_equal(line, expected[i], strlen(expected[i]));
		free(line);
	}
	line = get_next_line(pip[0]);
	assert_null(line);
	close(pip[0]);
}


static void one_char(void **state)
{
	(void)state;
	char	*line;

	int fd = open("one_char.txt", O_RDONLY);
	if (fd < 0)
		fail_msg("open failed: %s", strerror(errno));
	line = NULL;
	line = get_next_line(fd);
	assert_non_null(line);
	assert_memory_equal(line, "h", 1);
	free(line);
	line = get_next_line(fd);
	assert_null(line);
	close(fd);
}

static void one_char2(void **state)
{
	(void)state;
	test_gnl("h", (const char *[]){"h"}, 1);
}

static void one_char3(void **state)
{
	(void)state;
	test_gnl("\n", (const char *[]){"\n"}, 1);
}

static void	one_line(void **state)
{
	(void)state;
	test_gnl(
		"terrific smash from campagnolo\n", 
		(const char *[]){"terrific smash from campagnolo\n"},
		1
	);
}

static void	one_and_half_line(void **state)
{
	(void)state;
	test_gnl(
		"the volley by jl stays behind the net\nThe match is now 4:3", 
		(const char *[]){
		"the volley by jl stays behind the net\n", 
		"The match is now 4:3", 
		},
		2
	);
}

static void	two_lines(void **state)
{
	(void)state;
	test_gnl(
		"the volley by jl stays behind the net\nThe match is now 4:3\n", 
		(const char *[]){
		"the volley by jl stays behind the net\n", 
		"The match is now 4:3", 
		},
		2
	);
}

static void	many_lines(void **state)
{
	(void)state;
	test_gnl(
		"But what is frustration precisely and what causes it? Simply put, "
		"frustration arises when the path toward your goal is blocked, whet"
		"her the goal is setting a new PR in your squats, nailing a new tec"
		"hnique, or getting a good result. Frustration is an emotion that y"
		"ou probably experience frequently. And it can be really discouragi"
		"ng and, well, frustrating.\n"
		"Most people think of frustration as a bad emotion, but it is actua"
		"lly more complex than that. The fact is that frustration is hard-w"
		"ired into us to help us survive. Frustration starts as a good emot"
		"ion because when you get frustrated, you're motivated to remove th"
		"e obstacle that is blocking the path toward your goals. You try ha"
		"rder and that extra effort can result in clearing that path enabli"
		"ng you to continue to pursue your goals.\n"
		"The sky is blue.\n"
		"Birds chirp in the morning.\n"
		"Coffee smells amazing.\n"
		"Coding is fun!\n"
		"Life is full of surprises.\n"
		"Keep learning every day.\n"
		"Stay curious, stay sharp.\n"
		"Time waits for no one.\n"
		"A smile goes a long way.\n"
		"The ocean is vast.\n"
		"Books open new worlds.\n"
		"Kindness is powerful.\n"
		"Music soothes the soul.\n"
		"Challenges make us stronger.\n"
		"Take a deep breath.\n"
		"Stars shine at night.\n"
		"Rain makes everything fresh.\n"
		"Hard work pays off.\n"
		"Small steps lead to big changes.\n"
		"Believe in yourself!\n",
		(const char *[]){
			"But what is frustration precisely and what causes it? Simply put, "
			"frustration arises when the path toward your goal is blocked, whet"
			"her the goal is setting a new PR in your squats, nailing a new tec"
			"hnique, or getting a good result. Frustration is an emotion that y"
			"ou probably experience frequently. And it can be really discouragi"
			"ng and, well, frustrating.\n",
			"Most people think of frustration as a bad emotion, but it is actua"
			"lly more complex than that. The fact is that frustration is hard-w"
			"ired into us to help us survive. Frustration starts as a good emot"
			"ion because when you get frustrated, you're motivated to remove th"
			"e obstacle that is blocking the path toward your goals. You try ha"
			"rder and that extra effort can result in clearing that path enabli"
			"ng you to continue to pursue your goals.\n",
			"The sky is blue.\n",
			"Birds chirp in the morning.\n",
			"Coffee smells amazing.\n",
			"Coding is fun!\n",
			"Life is full of surprises.\n",
			"Keep learning every day.\n",
			"Stay curious, stay sharp.\n",
			"Time waits for no one.\n",
			"A smile goes a long way.\n",
			"The ocean is vast.\n",
			"Books open new worlds.\n",
			"Kindness is powerful.\n",
			"Music soothes the soul.\n",
			"Challenges make us stronger.\n",
			"Take a deep breath.\n",
			"Stars shine at night.\n",
			"Rain makes everything fresh.\n",
			"Hard work pays off.\n",
			"Small steps lead to big changes.\n",
			"Believe in yourself!\n",
		},
		22
	);
}

static void giant_lines(void **state)
{
	(void)state;
	char	*line;
	char	*line2 = NULL;
	size_t	len2;

	FILE *file = fopen("long.txt", "r");
	assert_non_null(file);
	int fd = open("long.txt", O_RDONLY);
	if (fd < 0)
		fail_msg("open failed: %s", strerror(errno));
	line = NULL;
	for (int i = 0; i < 12; ++i)
	{
		getline(&line2, &len2, file);
		line = get_next_line(fd);
		assert_non_null(line);
		assert_non_null(line2);
		assert_uint_equal(strlen(line), strlen(line2));
		assert_memory_equal(line, line2, strlen(line));
		free(line);
		free(line2);
		line2 = NULL;
	}
	line = get_next_line(fd);
	assert_null(line);
	close(fd);
	fclose(file);
}

static void read_2_files_on_same_fd(void **state)
{
	(void)state;
	test_2_gnl("hola\namigo", (const char*[]){"hola", "amigo"}, 2);
}

static void calling_two_fd_to_end(void **state)
{
	(void) state;
	test_gnl("\n\n\nhahaha", (const char*[]){"\n", "\n", "\n", "hahaha"}, 4);
	test_gnl("hasta aqui llegamos", (const char*[]){"hasta aqui llegamos"}, 1);
}

static void change_fd_mid_file(void **state)
{
	(void) state;
	int r_fd;
	test_gnl_not_full(
		"life is better with nice people around you\nright?\nRIGHT?\nRIGHTTTTTT\n",
		(const char *[]){"life is better with nice people around you\n", "right?\n"},
		2,
		&r_fd
	);
	test_gnl_with_diff_fd(
		"new safe\nzone\nidentified\n",
		(const char *[]){"new safe\n", "zone\n", "identified\n"},
		3,
		r_fd
	);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(one_char),
        cmocka_unit_test(one_char2),
        cmocka_unit_test(one_char3),
        cmocka_unit_test(one_line),
        cmocka_unit_test(one_and_half_line),
        cmocka_unit_test(two_lines),
        cmocka_unit_test(many_lines),
        cmocka_unit_test(giant_lines),
        cmocka_unit_test(read_2_files_on_same_fd),
        cmocka_unit_test(calling_two_fd_to_end),
        cmocka_unit_test(change_fd_mid_file),
    };
 
    return cmocka_run_group_tests(tests, NULL, NULL);
}
