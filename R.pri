# R related includes

R_HOME = 		$$system(R RHOME)

#message("Using $$R_HOME as RHOME")

## include headers and libraries for R
RCPPFLAGS= 		$$system($$R_HOME/bin/R CMD config --cppflags)
RLDFLAGS= 		$$system($$R_HOME/bin/R CMD config --ldflags)
RBLAS=  		$$system($$R_HOME/bin/R CMD config BLAS_LIBS)
RLAPACK= 		$$system($$R_HOME/bin/R CMD config LAPACK_LIBS)

## if you need to set an rpath to R itself, also uncomment
RRPATH=                 -Wl,-rpath,$$R_HOME/lib

## include headers and libraries for Rcpp interface classes
## note that RCPPLIBS will be empty with Rcpp (>= 0.11.0) and can be omitted
RCPPINCL= 		$$system(echo 'Rcpp:::CxxFlags\\(\\)' | $$R_HOME/bin/R --vanilla --slave)
RCPPLIBS= 		$$system(echo 'Rcpp:::LdFlags\\(\\)'  | $$R_HOME/bin/R --vanilla --slave)


## include headers and libraries for RInside embedding classes
RINSIDEINCL= 		$$system(echo 'RInside:::CxxFlags\\(\\)' | $$R_HOME/bin/R --vanilla --slave)
RINSIDELIBS= 		$$system(echo 'RInside:::LdFlags\\(\\)'  | $$R_HOME/bin/R --vanilla --slave)

## compiler etc settings used in default make rules
#RCXX := 			$$system(shell $(R_HOME)/bin/R CMD config CXX)
#RCPPFLAGS := 		-Wall $$system(shell $(R_HOME)/bin/R CMD config CPPFLAGS)
#CXXFLAGS := 		$(RCPPFLAGS) $(RCPPINCL) $(RINSIDEINCL) $(shell $(R_HOME)/bin/R CMD config CXXFLAGS)
#LDLIBS := 		$(RLDFLAGS) $(RRPATH) $(RBLAS) $(RLAPACK) $(RCPPLIBS) $(RINSIDELIBS)

QMAKE_CXXFLAGS += -Wall $$system($$R_HOME/bin/R CMD config CPPFLAGS) \
    $$RCPPFLAGS $$RCPPINCL $$RINSIDEINCL $$system($$R_HOME/bin/R CMD config CXXFLAGS)

LIBS += $$RLDFLAGS $$RRPATH $$RBLAS $$RLAPACK $$RCPPLIBS $$RINSIDELIBS

#message("QMAKE_CXXFLAGS = $$QMAKE_CXXFLAGS")
#message("LIBS = $$LIBS")
