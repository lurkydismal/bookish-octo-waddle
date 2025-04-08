SRCDIR                = .
SUBDIRS               =
DLLS                  =
LIBS                  = $(OUTPUT_FILE)
EXES                  =



### Common settings

CEXTRA                = $(C_FLAGS)
CXXEXTRA              =
RCEXTRA               =
DEFINES               = $(DEFINES)
INCLUDE_PATH          = $(INCLUDES) \
			$(patsubst %/*.h, %, $(FILES_TO_INCLUDE))
DLL_PATH              =
DLL_IMPORTS           =
LIBRARY_PATH          =
LIBRARIES             =


### Output file sources and settings

libfile_a_MODULE       = $(OUTPUT_FILE)
libfile_a_C_SRCS       = $(foreach _pattern, $(FILES_TO_COMPILE), $(wildcard $(_pattern)))
libfile_a_CXX_SRCS     =
libfile_a_RC_SRCS      =
libfile_a_LDFLAGS      =
libfile_a_ARFLAGS      = rc
libfile_a_DLL_PATH     =
libfile_a_DLLS         =
libfile_a_LIBRARY_PATH =
libfile_a_LIBRARIES    =

libfile_a_OBJS         = $(libfile_a_C_SRCS:.c=.o) \
			$(libfile_a_CXX_SRCS:.cpp=.o) \
			$(libfile_a_RC_SRCS:.rc=.res)



### Global source lists

C_SRCS                = $(libfile_a_C_SRCS)
CXX_SRCS              = $(libfile_a_CXX_SRCS)
RC_SRCS               = $(libfile_a_RC_SRCS)


### Tools

CC = $(C_COMPILER)
CXX = ccache g++
RC = rcc
AR = gcc-ar


### Generic targets

all: $(SUBDIRS) $(DLLS:%=%.so) $(LIBS) $(EXES)

### Build rules

.PHONY: all clean dummy

$(SUBDIRS): dummy
	@cd $@ && $(MAKE)

# Implicit rules

.SUFFIXES: .cpp .cxx .rc .res
DEFINCL = $(INCLUDE_PATH) $(DEFINES) $(OPTIONS)

.c.o:
	$(CC) -c $(CFLAGS) $(CEXTRA) $(DEFINCL) -o $@ $<

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(CXXEXTRA) $(DEFINCL) -o $@ $<

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(CXXEXTRA) $(DEFINCL) -o $@ $<

.rc.res:
	$(RC) $(RCFLAGS) $(RCEXTRA) $(DEFINCL) -fo$@ $<

# Rules for cleaning

CLEAN_FILES     = y.tab.c y.tab.h lex.yy.c core *.orig *.rej \
                  \\\#*\\\# *~ *% .\\\#*

clean:: $(SUBDIRS:%=%/__clean__) $(EXTRASUBDIRS:%=%/__clean__)
	$(RM) $(CLEAN_FILES) $(RC_SRCS:.rc=.res) $(C_SRCS:.c=.o) $(CXX_SRCS:.cpp=.o)
	$(RM) $(DLLS:%=%.so) $(LIBS) $(EXES) $(EXES:%=%.so)

$(SUBDIRS:%=%/__clean__): dummy
	cd `dirname $@` && $(MAKE) clean

$(EXTRASUBDIRS:%=%/__clean__): dummy
	-cd `dirname $@` && $(RM) $(CLEAN_FILES)

### Target specific build rules
DEFLIB = $(LIBRARY_PATH) $(LIBRARIES) $(DLL_PATH) $(DLL_IMPORTS:%=-l%)

$(libfile_a_MODULE): $(libfile_a_OBJS)
	$(AR) $(libfile_a_ARFLAGS) $@ $(libfile_a_OBJS)


