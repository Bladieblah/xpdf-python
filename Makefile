PROGNAME = a.out
PYDIR = src/xpydf/

XDIR = src/xpdf-4.04/
SPLASH_DIR = $(XDIR)splash/
FOFI_DIR = $(XDIR)fofi/
GOO_DIR = $(XDIR)goo/
XPDF_DIR = $(XDIR)xpdf/

OBJDIR = obj/

PY_SRC = src/xpydf/PdfLoader.cc src/xpydf/ImageInfoDev.cc src/xpydf/ImageDataDev.cc
SPLASH_SRC = $(wildcard $(SPLASH_DIR)*.cc)
FOFI_SRC = $(wildcard $(FOFI_DIR)*.cc)

XPDF_FILES = AcroForm.cc \
Array.cc \
Stream.cc \
Annot.cc \
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

XPDF_SRC = $(XPDF_FILES:%.cc=$(XDIR)/xpdf/%.cc)
GOO_SRC = $(GOO_FILES:%.cc=$(XDIR)/goo/%.cc)

SRC = $(PY_SRC) $(SPLASH_SRC) $(FOFI_SRC)

CC = g++ -std=c++17

WARNFLAGS = -Wall -Wno-deprecated-declarations -Wno-writable-strings
CFLAGS = -g -O3 $(WARNFLAGS) -MD -Isrc/xpdf-4.04 -Isrc/xpdf-4.04/goo -Isrc/xpdf-4.04/splash -Isrc/xpdf-4.04/xpdf -Isrc/xpdf-4.04/fofi -I/usr/local/include
# LDFLAGS =-framework opencl -framework GLUT -framework OpenGL -framework Cocoa -L/usr/local/lib -lGLEW

# Do some substitution to get a list of .o files from the given .cc files.
OBJFILES = $(patsubst $(PYDIR)%.cc, $(OBJDIR)%.o, $(PY_SRC)) $(patsubst $(SPLASH_DIR)%.cc, $(OBJDIR)%.o, $(SPLASH_SRC)) $(patsubst $(FOFI_DIR)%.cc, $(OBJDIR)%.o, $(FOFI_SRC)) $(XPDF_FILES:%.cc=$(OBJDIR)%.o) $(GOO_FILES:%.cc=$(OBJDIR)%.o)
# INCFILES = $(patsubst $(SRCDIR)%.cc, $(INCDIR)%.hpp, $(SRC))

.PHONY: all clean

all: $(PROGNAME)

$(PROGNAME): $(OBJFILES)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR)%.o: $(PYDIR)%.cc
	$(CC) -c $(CFLAGS) -o $@ $<

$(OBJDIR)%.o: $(XPDF_DIR)%.cc
	$(CC) -c $(CFLAGS) -o $@ $<

$(OBJDIR)%.o: $(GOO_DIR)%.cc
	$(CC) -c $(CFLAGS) -o $@ $<

$(OBJDIR)%.o: $(FOFI_DIR)%.cc
	$(CC) -c $(CFLAGS) -o $@ $<

$(OBJDIR)%.o: $(SPLASH_DIR)%.cc
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -fv $(PROGNAME) $(OBJFILES)
	rm -fv $(OBJDIR)*.d

-include $(OBJFILES:.o=.d)
