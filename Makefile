################################

SYSTEM     = x86-64_linux
LIBFORMAT  = static_pic
CPLEXDIR   = /opt/ibm/ILOG/CPLEX_Studio129/cplex
CONCERTDIR = /opt/ibm/ILOG/CPLEX_Studio129/concert

################

# Compilador 
CCC = g++ -O0

# Opcoes de compilacao
CCOPT = -m64 -O -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD -Wno-ignored-attributes

# Bibliotecas e includes
CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

CPLEXBINDIR   = $(CPLEXDIR)/bin/$(SYSTEM)
CPLEXLIB      = cplex$(dynamic:yes=1290)

CCLNDIRS  = -L$(CPLEXLIBDIR) -L$(CONCERTLIBDIR) $(dynamic:yes=-L$(CPLEXBINDIR))
CCLNFLAGS = -lconcert -lilocplex -l$(CPLEXLIB) -lm -lpthread -ldl

CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include

CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) 

all: pc

pc : pc.o clique.o
	$(CCC) $(CCFLAGS) $(CCLNDIRS) -o pc pc.o clique.o $(CCLNFLAGS)
pc.o: pc.cpp
	$(CCC) -c $(CCFLAGS) pc.cpp -o pc.o
clique.o: clique.cpp	
	$(CCC) -c $(CCFLAGS) clique.cpp -o clique.o
clean:
	rm --force pc pc.o clique.o

