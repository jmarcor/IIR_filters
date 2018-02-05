MAKE=make
LIBSRC=src
LIBTEST=test

# Just build all tests and run them
all install clean uninstall:
	cd $(LIBTEST) && $(MAKE) $@