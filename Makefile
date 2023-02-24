PROGNAME = a.out
PYDIR = src/xpdf-python/

XDIR = src/xpdf-4.04/
SPLASH_DIR = $(XDIR)splash/
FOFI_DIR = $(XDIR)fofi/
GOO_DIR = $(XDIR)goo/
XPDF_DIR = $(XDIR)xpdf/

OBJDIR = obj/

PY_SRC = src/xpdf-python/pdftostring.cc
SPLASH_SRC = $(wildcard $(SPLASH_DIR)*.cc)
FOFI_SRC = $(wildcard $(FOFI_DIR)*.cc)

XPDF_FILES = GlobalParams.cc Object.cc Stream.cc Array.cc Dict.cc XRef.cc Catalog.cc Page.cc PDFDoc.cc TextOutputDev.cc UnicodeMap.cc TextString.cc Error.cc UTF8.cc UnicodeTypeTable.cc BuiltinFontTables.cc JBIG2Stream.cc NameToCharCode.cc OptionalContent.cc SecurityHandler.cc UnicodeRemapping.cc BuiltinFont.cc CharCodeToUnicode.cc Gfx.cc GfxState.cc GfxFont.cc Annot.cc Lexer.cc Link.cc Parser.cc PSTokenizer.cc JArithmeticDecoder.cc Decrypt.cc AcroForm.cc Outline.cc Function.cc CMap.cc JPXStream.cc OutputDev.cc PDF417Barcode.cc XFAScanner.cc FontEncodingTables.cc Zoox.cc PDFDocEncoding.cc
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

# -include $(OBJFILES:.o=.d)
