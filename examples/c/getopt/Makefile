OUT := main.out
SRC := main.c

all: $(OUT)

$(OUT): $(SRC)
	$(CC) "$^" -o "$@"

clean:
	-rm -f "$(OUT)"