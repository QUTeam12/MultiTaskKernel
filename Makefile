################################################################
### makefile for C Jikken.
### Yoshinari Nomura <nom@csce.kyushu-u.ac.jp>
### $Id: Makefile,v 1.1.1.1 2001/04/08 06:29:47 nom Exp $
################################################################

LIB_JIKKEN=~/.local/lib/soft-jikken

default:
	@echo '###################################################'
	@echo '# make test1  -- build test1.abs                  #'
	@echo '# make test2  -- build test2.abs                  #'
	@echo '# make test3  -- build test3.abs                  #'
	@echo '# make clean  -- cleanup current directory        #'
	@echo '# make depend -- make dependency in .depend       #'
	@echo '###################################################'

all: test1 test2 test3

test1:
	$(MAKE) $(MAKEFLAGS) LIB_JIKKEN=$(LIB_JIKKEN) -f Makefile.1
test2:
	$(MAKE) $(MAKEFLAGS) LIB_JIKKEN=$(LIB_JIKKEN) -f Makefile.2
test3:
	$(MAKE) $(MAKEFLAGS) LIB_JIKKEN=$(LIB_JIKKEN) -f Makefile.3

include $(LIB_JIKKEN)/make.conf
