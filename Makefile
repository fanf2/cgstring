all: clean cgtest test

clean:
	rm -f cgtest test

readme README.html: README.md
	markdown README.md | tee README.html | pbcopy

test: test.c
	-cc -DHEADER='<string.h>' -otest test.c
	-./test
	@echo ==== NOTE: that should have exploded at run time

cgtest: test.c
	-cc -DHEADER='"cgstring.h"' -ocgtest test.c
	-./cgtest
	@echo ==== NOTE: that should have failed to compile
