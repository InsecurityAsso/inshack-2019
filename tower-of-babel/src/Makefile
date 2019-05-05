QUIET      := @
PWD        := $(shell pwd)
CHALL_CONF := $(PWD)/../.mkctf.yml
TOOLCHAIN  := $(PWD)/toolchain
SOURCES    := $(PWD)/sources
CONFIG     := $(PWD)/config
BUILD      := $(PWD)/build

all: build-dir module test

build-dir:
	$(QUIET)mkdir -p $(BUILD)

test: build-dir
	$(QUIET)gcc -o $(BUILD)/test $(SOURCES)/test.c

module: build-dir
	$(QUIET)cp $(SOURCES)/Makefile $(BUILD)/Makefile
	$(QUIET)python3 $(TOOLCHAIN)/resgen.py \
		--chall-conf=$(CHALL_CONF) \
		--header-file=$(SOURCES)/tob.header.c \
		--footer-file=$(SOURCES)/tob.footer.c \
		--output-file=$(BUILD)/tob.plaintext.c \
		--strings-file=$(CONFIG)/strings.res
	$(QUIET)python3 $(TOOLCHAIN)/obfuscate.py \
		--input-file=$(BUILD)/tob.plaintext.c \
		--output-file=$(BUILD)/tob.c \
		--identifiers-file=$(CONFIG)/identifiers.res
	$(QUIET)$(MAKE) -C $(BUILD)

clean:
	$(QUIET)rm -rf $(BUILD)
