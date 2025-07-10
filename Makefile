SRCDIR                = .
SUBDIRS               =
DLLS                  =
LIBS                  = $(OUTPUT_FILE)
EXES                  =



### Common settings

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
libfile_a_SRCS         = $(foreach _pattern, $(FILES_TO_COMPILE), $(wildcard $(_pattern)))
libfile_a_RC_SRCS      =
libfile_a_LDFLAGS      =
libfile_a_ARFLAGS      = rc
libfile_a_DLL_PATH     =
libfile_a_DLLS         =
libfile_a_LIBRARY_PATH =
libfile_a_LIBRARIES    =

libfile_a_OBJS         = $(libfile_a_SRCS:.c=.o) \
			$(libfile_a_RC_SRCS:.rc=.res)



### Global source lists

SRCS                = $(libfile_a_SRCS)
RC_SRCS               = $(libfile_a_RC_SRCS)


### Tools

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
	@echo "Compiling $<"
	@$(COMPILER) -c $< $(BUILD_FLAGS) $(DEFINCL) -o $@

.rc.res:
	@echo "Compiling resource $<"
	@$(RC) $(RCFLAGS) $(RCEXTRA) $(DEFINCL) -fo$@ $<

# Rules for cleaning

CLEAN_FILES     = y.tab.c y.tab.h lex.yy.c core *.orig *.rej \
                  \\\#*\\\# *~ *% .\\\#*

clean:: $(SUBDIRS:%=%/__clean__) $(EXTRASUBDIRS:%=%/__clean__)
	@$(RM) $(CLEAN_FILES) $(RC_SRCS:.rc=.res) $(SRCS:.c=.o)
	@$(RM) $(DLLS:%=%.so) $(LIBS) $(EXES) $(EXES:%=%.so)

$(SUBDIRS:%=%/__clean__): dummy
	cd `dirname $@` && $(MAKE) clean

$(EXTRASUBDIRS:%=%/__clean__): dummy
	-cd `dirname $@` && $(RM) $(CLEAN_FILES)

### Target specific build rules
DEFLIB = $(LIBRARY_PATH) $(LIBRARIES) $(DLL_PATH) $(DLL_IMPORTS:%=-l%)

$(libfile_a_MODULE): $(libfile_a_OBJS)
	@echo "Archiving $<"
	@$(AR) $(libfile_a_ARFLAGS) $@ $(libfile_a_OBJS)


