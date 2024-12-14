.PHONY: all lint

all:

lint:
	clang-format -i $(shell git diff --cached --name-only --diff-filter=ACM | egrep '.(cc|h)' 2>&1) /dev/null
