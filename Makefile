VERSION=\"0.1b\"

export CFLAGS += -pipe -march=native -Wall -std=c99 -pedantic -D_XOPEN_SOURCE=700 -DVERSION=${VERSION}
export LDFLAGS +=
PREFIX=/usr

.PHONY: install debug release clean src strip

default: release

debug: CFLAGS += -g -D__DEBUG -Werror
debug: src
release: CFLAGS += -O2 -ffast-math -flto -ftree-vectorize
release: src

src:
	$(MAKE) -C $@

strip:
	$(MAKE) -C src strip

clean:
	$(MAKE) -C src clean

install: default
	@echo Installing executable file to ${PREFIX}/bin
	@mkdir -p ${PREFIX}/bin
	@cp src/meteor_demod ${PREFIX}/bin
	@chmod 755 ${PREFIX}/bin/meteor_demod

uninstall:
	@echo Removing executable file from ${PREFIX}/bin
	@rm -f ${PREFIX}/bin/meteor_demod
