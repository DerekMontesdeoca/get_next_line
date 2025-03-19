################################################################################
# TEST GET_NEXT_LINE
################################################################################

CC := cc

CMOCKA_BUILD_DIR := cmocka/build
CMOCKA_INCLUDE := cmocka/include
CMOCKA_LIB_DIR := $(CMOCKA_BUILD_DIR)/src
CMOCKA := $(CMOCKA_LIB_DIR)/libcmocka.so

NAME := test
BONUS_NAME := test_bonus
SRCS := test.c ../get_next_line.c ../get_next_line_utils.c
OBJS := $(SRCS:.c=.o)
BONUS_SRCS :=  $(SRCS:.c=_bonus.c)
BONUS_OBJS := $(BONUS_SRCS:.c=.o)
override CFLAGS += -g3 -fno-omit-frame-pointer -Wall -Wextra -Werror
override CPPFLAGS += -I$(CMOCKA_INCLUDE) 
override LDFLAGS += -L$(CMOCKA_LIB_DIR)
LDLIBS := -lcmocka


################################################################################
# Rules
################################################################################

all: run_buf_tests

$(CMOCKA):
	mkdir -p $(CMOCKA_BUILD_DIR)
	cd $(CMOCKA_BUILD_DIR) && cmake .. && make

debug: $(NAME)
	LD_LIBRARY_PATH=$(CMOCKA_LIB_DIR) gdb $(NAME)

run_buf_tests:
	n=0; for i in $$(seq 0 16); do \
	  n=$$(echo "2^$$i" | bc); \
	  $(MAKE) re CPPFLAGS=-DBUFFER_SIZE=$$n; \
	  $(MAKE) run_tests; \
  	done;

bonus:
	n=0; for i in $$(seq 0 16); do \
	  n=$$(echo "2^$$i" | bc); \
	  $(MAKE) fclean; \
	  $(MAKE) $(BONUS_NAME) CPPFLAGS=-DBUFFER_SIZE=$$n; \
	  $(MAKE) run_bonus_tests; \
  	done;

run_bonus_tests:
	LD_LIBRARY_PATH=$(CMOCKA_LIB_DIR) ./$(BONUS_NAME)

run_tests:
	LD_LIBRARY_PATH=$(CMOCKA_LIB_DIR) ./$(NAME)

$(NAME): $(CMOCKA) $(OBJS)
	$(CC) $(LDFLAGS) $(LDLIBS) $(OBJS) -o $@

$(BONUS_NAME): $(CMOCKA) $(BONUS_OBJS)
	$(CC) $(LDFLAGS) $(LDLIBS) $(BONUS_OBJS) -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(BONUS_OBJS)

fclean: clean
	rm -rf $(NAME) $(BONUS_NAME)

re: fclean $(NAME)

.PHONY: all run_tests clean fclean bonus run_bonus_tests 
