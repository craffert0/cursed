.PHONY: all lint test test_crackle test_bazel

all:

lint:
	clang-format -i $(shell git diff --cached --name-only --diff-filter=ACM | egrep '.(cc|h)' 2>&1) /dev/null
	swiftformat --quiet --swiftversion 6 $(shell git diff --cached --name-only --diff-filter=ACM | egrep '.swift' 2>&1)

test: test_crackle test_bazel

test_crackle:
	bazel run //crackle:cracklepop | python3 crackle/test.py

test_bazel:
	bazel test -k //...
