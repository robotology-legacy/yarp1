MAIN = vision

#VPATH += text
#VPATH += images

OUTPUT = out

#MAIN = $(patsubst %.tex, %, $(wildcard *.tex))
SUBMAIN = $(wildcard *.tex)
JPEGS = $(wildcard *.jpg)
GIFS = $(wildcard *.gif)
EPSSRCS = $(wildcard *.epssrc)
PRNSRCS = $(wildcard *.prn)
PSSRCIS = $(wildcard *.pssrci)
PSSRCS = $(wildcard *.pssrc)
EPSPREV = $(wildcard *.eps)
EPSJPEG = $(patsubst %.jpg, %.eps, $(JPEGS))
EPSGIF = $(patsubst %.gif, %.eps, $(GIFS))
EPSSRC = $(patsubst %.epssrc, %.eps, $(EPSSRCS))
PSSRC = $(patsubst %.pssrc, %.eps, $(PSSRCS))  $(patsubst %.prn, %.eps, $(PRNSRCS))
PSSRCI = $(patsubst %.pssrci, %.eps, $(PSSRCIS))
EPS = $(EPSPREV) $(EPSJPEG) $(EPSGIF) $(EPSSRC) $(PSSRC) $(PSSRCI)

all: $(MAIN).dvi

%.pssrc: %.prn
	cp $< $@

%.eps: %.jpg
	convert $^ eps2:$@.tmp
	sed "s/statusdict/%%%statusdict/g" < $@.tmp > $@
	rm -f $@.tmp

%.eps: %.gif
	convert $^ eps2:$@.tmp
	sed "s/statusdict/%%%statusdict/g" < $@.tmp > $@
	rm -f $@.tmp

%.eps: %.epssrc
	cp $^ $@

%.eps: %.pssrc
	ps2eps $<
	( sed "s/statusdict/%%%statusdict/g" | sed "s/^(%%\[[^ ]/%%(%%\[/" | sed "s/: N 111 /: %%N 111 /" ) < $<.eps > $@
	rm -f $^.eps

%.eps: %.pssrci
	ps2epsi $<
	sed "s/statusdict/%%%statusdict/g" < $<.epsi > $@
	rm -f $<.epsi
#	mv $^.epsi $@

%.bbl: %.bib
	bibtex $*

%.pdf: %.ps
	distill -compatlevel 2.1 -colordownsample off -graydownsample off -monodownsample off $^

d: $(MAIN).dvi
	xdvi $(MAIN).dvi

gv: $(MAIN).ps
	gv $(MAIN).ps

p: $(MAIN).ps

b: $(MAIN).bbl

fb:
	rm -f $(MAIN).bbl
	make $(MAIN).bbl

f:
	rm -f $(MAIN).dvi
	make $(MAIN).dvi

wc:
	cat *.tex | wc

bounce:
	make fb ; make f ; make fb ; make f ; make fb ; make f

$(MAIN).dvi: $(MAIN).tex $(EPS) $(MAIN).bbl $(SUBMAIN)
	latex $(MAIN).tex

force:
	latex $(MAIN).tex

$(MAIN).ps: $(MAIN).dvi
	dvips -f $< > $@

short:
	cp defs-iros.tex defs.tex
	make bounce

long:
	cp defs-epi.tex defs.tex
	make bounce

# only works on a sun, with distill installed
pdf: $(MAIN).pdf

clean:
	rm -f $(MAIN).dvi $(MAIN).ps

