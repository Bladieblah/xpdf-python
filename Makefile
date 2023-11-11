PROGNAME = a.out
OBJDIR = obj/

CC = g++ -std=c++17
C = gcc -pedantic -std=c99
WARNFLAGS = -Wall
#  -Wno-deprecated-declarations -Wno-writable-strings
CCFLAGS = -O3 $(WARNFLAGS) -MD -Isrc/xpdf-4.04 -Isrc/xpdf-4.04/goo -Isrc/xpdf-4.04/splash -Isrc/xpdf-4.04/xpdf -Isrc/xpdf-4.04/fofi -I/usr/local/include -Isrc/freetype/include  -Isrc/freetype/include/freetype
CFLAGS =  -O3 $(WARNFLAGS) -MD -pthread -DFT_CONFIG_CONFIG_H="<ftconfig.h>" -DFT_CONFIG_MODULES_H="<ftmodule.h>" -DFT_CONFIG_OPTIONS_H="<ftoption.h>" -DFT2_BUILD_LIBRARY -Isrc/freetype/include -Isrc/freetype/include/freetype -Isrc/freetype/include/freetype/config  -Isrc/freetype/include/freetype/internal

PYDIR = src/xpydf/

XDIR = src/xpdf-4.04/
SPLASH_DIR = $(XDIR)splash/
FOFI_DIR = $(XDIR)fofi/
GOO_DIR = $(XDIR)goo/
XPDF_DIR = $(XDIR)xpdf/

PY_SRC = src/xpydf/PdfLoader.cc src/xpydf/ImageInfoDev.cc src/xpydf/ImageDataDev.cc src/xpydf/FontOutputDev.cc
SPLASH_SRC = $(wildcard $(SPLASH_DIR)*.cc)
FOFI_SRC = $(wildcard $(FOFI_DIR)*.cc)

XPDF_FILES = AcroForm.cc \
Annot.cc \
Array.cc \
BuiltinFont.cc \
BuiltinFontTables.cc \
Catalog.cc \
CharCodeToUnicode.cc \
CMap.cc \
Decrypt.cc \
Dict.cc \
Error.cc \
FontEncodingTables.cc \
Function.cc \
Gfx.cc \
GfxFont.cc \
GfxState.cc \
GlobalParams.cc \
ImageOutputDev.cc \
JArithmeticDecoder.cc \
JBIG2Stream.cc \
JPXStream.cc \
Lexer.cc \
Link.cc \
NameToCharCode.cc \
Object.cc \
OptionalContent.cc \
Outline.cc \
OutputDev.cc \
Page.cc \
Parser.cc \
PDF417Barcode.cc \
PDFDoc.cc \
PDFDocEncoding.cc \
PSTokenizer.cc \
SecurityHandler.cc \
ShadingImage.cc \
SplashOutputDev.cc \
Stream.cc \
TextOutputDev.cc \
TextString.cc \
UnicodeMap.cc \
UnicodeRemapping.cc \
UnicodeTypeTable.cc \
UTF8.cc \
XFAScanner.cc \
XRef.cc \
Zoox.cc

GOO_FILES = gmem.cc gmempp.cc gfile.cc GHash.cc GList.cc GString.cc

FDIR = src/freetype/src/
FREE_DIRS = $(wildcard $(FDIR)*/)

FREE_OBJ = autofit.o \
bdf.o \
cff.o \
dlgwrap.o \
ft-hb.o \
ftbase.o \
ftbbox.o \
ftbdf.o \
ftbitmap.o \
ftbzip2.o \
ftcache.o \
ftcid.o \
ftdebug.o \
ftfstype.o \
ftgasp.o \
ftglyph.o \
ftgxval.o \
ftgzip.o \
ftinit.o \
ftlzw.o \
ftmm.o \
ftotval.o \
ftpatent.o \
ftpfr.o \
ftstroke.o \
ftsynth.o \
ftsystem.o \
fttype1.o \
ftwinfnt.o \
gxvalid.o \
otvalid.o \
pcf.o \
pfr.o \
psaux.o \
pshinter.o \
psnames.o \
raster.o \
sdf.o \
sfnt.o \
smooth.o \
svg.o \
truetype.o \
type1.o \
type1cid.o \
type42.o \
winfnt.o


# Do some substitution to get a list of .o files from the given .cc files.
OBJFILES = $(patsubst $(PYDIR)%.cc, $(OBJDIR)%.o, $(PY_SRC)) \
$(patsubst $(SPLASH_DIR)%.cc, $(OBJDIR)%.o, $(SPLASH_SRC)) \
$(patsubst $(FOFI_DIR)%.cc, $(OBJDIR)%.o, $(FOFI_SRC)) \
$(XPDF_FILES:%.cc=$(OBJDIR)%.o) \
$(GOO_FILES:%.cc=$(OBJDIR)%.o) \
$(FREE_OBJ:%.o=$(OBJDIR)%.o)

vpath %.c $(FREE_DIRS)

.PHONY: all clean

all: $(PROGNAME)

$(PROGNAME): $(OBJFILES)
	$(CC) $(CCFLAGS) -o $@ $^

$(OBJDIR)%.o: $(PYDIR)%.cc
	$(CC) -c $(CCFLAGS) -o $@ $<

$(OBJDIR)%.o: $(XPDF_DIR)%.cc
	$(CC) -c $(CCFLAGS) -o $@ $<

$(OBJDIR)%.o: $(GOO_DIR)%.cc
	$(CC) -c $(CCFLAGS) -o $@ $<

$(OBJDIR)%.o: $(FOFI_DIR)%.cc
	$(CC) -c $(CCFLAGS) -o $@ $<

$(OBJDIR)%.o: $(SPLASH_DIR)%.cc
	$(CC) -c $(CCFLAGS) -o $@ $<

$(OBJDIR)%.o: %.c
	$(C) -c $(CFLAGS) -o $@ $<

clean:
	rm -fv $(PROGNAME) $(OBJFILES)
	rm -fv $(OBJDIR)*.d

cleanft:
	rm -fv $(PROGNAME) $(FREE_OBJ)

test:
	$(info $(FREE_DIRS))
	$(info $(SKIP_DIRS))

-include $(OBJFILES:.o=.d)
