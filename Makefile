SNAPCAST_SRC = $(wildcard src/*.c)
SNAPCAST_OBJ = $(SNAPCAST_SRC:.c=.o)
SNAPCAST_INCLUDE = -I include
LIBRARY_ARCHIVE = libsnapcast.a
INC_DIR = include
INC_HEADER = snapcast.h

DEST_PREFIX= /usr/local
DEST_LIB_DIR= $(DEST_PREFIX)/lib
DEST_INC_DIR= $(DEST_PREFIX)/include

TEST_SRC = $(wildcard test/*.c)

LDFLAGS = -L . -l cjson -l snapcast -l buffer
CFLAGS = -g

%.o: %.c
	$(CC) -o $@ -c $< $(SNAPCAST_INCLUDE) $(CFLAGS)

$(LIBRARY_ARCHIVE): $(SNAPCAST_OBJ)
	ar rcs $@ $^

test/test: $(LIBRARY_ARCHIVE) $(TEST_SRC)
	$(CC) -o $@ $(TEST_SRC) $(LDFLAGS) $(SNAPCAST_INCLUDE) $(CFLAGS)

test: test/test
	./test/test

install: $(LIBRARY_ARCHIVE)
	mkdir -p $(DEST_LIB_DIR) $(DEST_INC_DIR)
	install -m 644 $(LIBRARY_ARCHIVE) $(DEST_LIB_DIR)
	install -m 644 $(INC_DIR)/$(INC_HEADER) $(DEST_INC_DIR)

uninstall:
	rm -f $(addprefix $(DEST_LIB_DIR)/,$(LIBRARY_ARCHIVE))
	rm -f $(addprefix $(DEST_INC_DIR)/,$(INC_HEADER))

examples/minimal_client/minimal_client: $(LIBRARY_ARCHIVE) examples/minimal_client/main.c
	$(CC) -o $@ examples/minimal_client/main.c $(LDFLAGS) $(SNAPCAST_INCLUDE) $(CFLAGS)

examples: examples/minimal_client/minimal_client

.PHONY: clean
clean:
	rm -f $(SNAPCAST_OBJ) $(LIBRARY_ARCHIVE) examples/minimal_client/minimal_client
