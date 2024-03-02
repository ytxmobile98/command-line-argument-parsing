OUT := main.out
SRC := main.c

.PHONY: all
all: $(OUT)

$(OUT): $(SRC)
	$(CC) "$^" -o "$@"

.PHONY: clean
clean:
	-rm -f "$(OUT)"