###########################################################################################
###########################################################################################
#

#deps: pcre++

#pcre++
INCLUDE_PCREPLUSPLUS = 

INCLUDE=  $(INCLUDE_PCREPLUSPLUS) -I../aurreprozesua_raw -I../morfologia


CCFLAGS= -fPIC -O
CPPFLAGS=

LEX = flex
YACC = bison
LEXYACC_IZEN_HASIERA = hat_karga
LEXRES = lex.$(LEXYACC_IZEN_HASIERA).cc
YACCRES = $(LEXYACC_IZEN_HASIERA).tab.cc

LEXFLAGS = -8 -P$(LEXYACC_IZEN_HASIERA) -o$(LEXRES)
YACCFLAGS = -vd -p $(LEXYACC_IZEN_HASIERA) -b $(LEXYACC_IZEN_HASIERA) -o $(YACCRES)

LEXFILE = $(LEXYACC_IZEN_HASIERA).l
YACCFILE = $(LEXYACC_IZEN_HASIERA).y


SOURCES= IM_info.cc int_bikote.cc LEMA.cc HAT.cc $(LEXYACC_IZEN_HASIERA).tab.cc \
	lex.$(LEXYACC_IZEN_HASIERA).cc HAT_class.cc FORMA.cc HITZ.cc AN.cc HAT_lag.cc

MEMBERS=$(SOURCES:.cc=.o)


CCC=g++
COMPILE.cc=$(CCC) $(INCLUDE) $(CCFLAGS) $(CPPFLAGS) -c
LINK.cc=$(CCC) $(CCFLAGS) $(CPPFLAGS)

.cc.o: 
	$(COMPILE.cc) $< 

all: $(LEXRES) $(YACCRES) $(MEMBERS)

$(MEMBERS): $(SOURCES)

$(LEXRES): $(LEXFILE)
	$(LEX) $(LEXFLAGS) $(LEXFILE)

$(YACCRES): $(YACCFILE)
	$(YACC) $(YACCFLAGS) $(YACCFILE)

clean: FORCE
	rm -f $(MEMBERS) $(LEXRES) $(YACCRES)

FORCE:

