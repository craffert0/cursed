.PHONY: all lint test test_crackle test_bazel

all:

lint:
	tools/reformat.py

test: test_crackle test_bazel

test_crackle:
	bazel run //crackle:cracklepop | python3 crackle/test.py

test_bazel:
	bazel test -k //...
