# Hau erabiltzeko honako ingurune aldagaiak definitu behar dira:

# deps: pcre++, foma

#pcre++
INCLUDE_PCREPLUSPLUS = 
#foma
INCLUDE_FOMA = 
#
INCLUDE =  $(INCLUDE_PCREPLUSPLUS) $(INCLUDE_FOMA)


SOURCES= anabihurtzailea.cc analizatzailea.cc fomamorf.cc getenvvar.cc kendu_marka_lex.cc maiztasun_handikoak.cc 
SOURCESC= azterzen.c fs_lag.c fun_def.c irakaut.c zerhitz.c


MEMBERS=$(SOURCES:.cc=.o)
MEMBERSC=$(SOURCESC:.c=.o)


CCC= g++
CCFLAGS= -O -fpic 
CPPFLAGS= -Wall -D_USE_FOMA_ -D__BRIDGE_VARIANTS__ 

COMPILE.cc=$(CCC) $(CCFLAGS) $(CPPFLAGS) $(INCLUDE) -c

COMPILE.c=$(CCC) $(CCFLAGS) $(CPPFLAGS) $(INCLUDE) -c

all: $(MEMBERS) $(MEMBERSC)


$(MEMBERS): $(SOURCES)

$(MEMBERSC): $(SOURCESC)

clean: FORCE
	rm -f $(MEMBERS) $(MEMBERSC)

install: FORCE
	install -d -m 755 $(INSTALL_PATH)/include/
	install -m 644 *.h $(INSTALL_PATH)/include/

FORCE:
