PROJ = chkbck

all: $(PROJ) man

man: $(PROJ).1.gz

$(PROJ).1.gz: $(PROJ).t2t
	txt2tags -o$(basename $@) $<
	gzip -q $(basename $@)

mancheck: $(PROJ).1.gz
	man ./$<

$(PROJ): chkbck.c

install: $(PROJ) $(PROJ).1.gz uninstall
	cp $(PROJ).1.gz /usr/share/man/man1
	cp $(PROJ) /usr/local/bin

.PHONY: uninstall
uninstall:
	$(RM) /usr/share/man/man1/$(PROJ).1.gz
	$(RM) /usr/local/bin/$(PROJ)
