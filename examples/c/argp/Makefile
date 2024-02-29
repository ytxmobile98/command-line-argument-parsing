DIRS := $(shell find -mindepth 1 -maxdepth 1 -type d)

all:
	@for dir in $(DIRS); do \
		$(MAKE) -C "$$dir" all; \
	done

clean:
	@for dir in $(DIRS); do \
		$(MAKE) -C "$$dir" clean; \
	done