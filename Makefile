# Makefile for chkbck
PROJ = chkbck

CC = gcc
CFLAGS = -g3 -O0 -Wall -Wextra

ARGS =
INSTALDIR = /usr/local/bin

all: $(PROJ) man

man: $(PROJ).1.gz

$(PROJ).1.gz: $(PROJ).t2t
	txt2tags -o$(basename $@) $<
	gzip -q $(basename $@)

mancheck: $(PROJ).1.gz
	man ./$<

$(PROJ): $(PROJ).c

install: $(PROJ) $(PROJ).1.gz uninstall
	cp $(PROJ).1.gz $(MANDIR)/
	cp $(PROJ) $(INSTALDIR)/

.PHONY: uninstall
uninstall:
	$(RM) $(MANDIR)/$(PROJ).1.gz
	$(RM) $(INSTALDIR)/$(PROJ)

run:
	./$(PROJ) $(ARGS)

.PHONY: clean
clean:
	$(RM) *.o
	$(RM) *.exe
	$(RM) *.asm
	$(RM) *.map
	$(RM) *.stackdump
	$(RM) *.core
