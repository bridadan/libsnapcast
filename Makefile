SNAPCAST_SRC = $(wildcard src/*.c)
SNAPCAST_OBJ = $(SNAPCAST_SRC:.c=.o)
SNAPCAST_INCLUDE = include

TEST_SRC = $(wildcard test/*.c)
TEST_OBJ = $(TEST_SRC:.c=.o)

LDFLAGS = -L . -l cjson -l snapcast
CFLAGS = -g

%.o: %.c
	$(CC) -o $@ -c $< -I $(SNAPCAST_INCLUDE) $(CFLAGS)

libsnapcast.a: $(SNAPCAST_OBJ)
	ar rcs $@ $^

test_bin: $(TEST_OBJ) libsnapcast.a
	$(CC) -o $@ $(TEST_SRC) $(LDFLAGS) -I $(SNAPCAST_INCLUDE) $(CFLAGS)

test: test_bin
	./test_bin

.PHONY: clean
clean:
	rm -f $(SNAPCAST_OBJ) $(TEST_OBJ) libsnapcast.a
