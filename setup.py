import os
from glob import glob
from pathlib import Path
import numpy as np

from setuptools.command.build_ext import build_ext
from setuptools import Extension, setup

python_dir = Path("src/xpydf")
xpdf_dir = Path("src/xpdf-4.04/xpdf")
splash_dir = Path("src/xpdf-4.04/splash")
fofi_dir = Path("src/xpdf-4.04/fofi")
goo_dir = Path("src/xpdf-4.04/goo")

python_src = glob(str(python_dir / "*.cc"))
splash_src = glob(str(splash_dir / "*.cc"))
fofi_src = glob(str(fofi_dir / "*.cc"))
goo_src = glob(str(goo_dir / "*.cc"))

xpdf_files = [
    "AcroForm.cc",
    "Annot.cc",
    "Array.cc",
    "BuiltinFont.cc",
    "BuiltinFontTables.cc",
    "Catalog.cc",
    "CharCodeToUnicode.cc",
    "CMap.cc",
    "Decrypt.cc",
    "Dict.cc",
    "Error.cc",
    "FontEncodingTables.cc",
    "Function.cc",
    "Gfx.cc",
    "GfxFont.cc",
    "GfxState.cc",
    "GlobalParams.cc",
    "ImageOutputDev.cc",
    "JArithmeticDecoder.cc",
    "JBIG2Stream.cc",
    "JPXStream.cc",
    "Lexer.cc",
    "Link.cc",
    "NameToCharCode.cc",
    "Object.cc",
    "OptionalContent.cc",
    "Outline.cc",
    "OutputDev.cc",
    "Page.cc",
    "Parser.cc",
    "PDF417Barcode.cc",
    "PDFDoc.cc",
    "PDFDocEncoding.cc",
    "PSTokenizer.cc",
    "SecurityHandler.cc",
    "ShadingImage.cc",
    "SplashOutputDev.cc",
    "Stream.cc",
    "TextOutputDev.cc",
    "TextString.cc",
    "UnicodeMap.cc",
    "UnicodeRemapping.cc",
    "UnicodeTypeTable.cc",
    "UTF8.cc",
    "XFAScanner.cc",
    "XRef.cc",
    "Zoox.cc",
]

freetype_sources = [
    "src/freetype/src/autofit/autofit.c",
    "src/freetype/src/autofit/ft-hb.c",
    "src/freetype/src/base/ftbase.c",
    "src/freetype/src/base/ftbbox.c",
    "src/freetype/src/base/ftbdf.c",
    "src/freetype/src/base/ftbitmap.c",
    "src/freetype/src/base/ftcid.c",
    "src/freetype/src/base/ftdebug.c",
    "src/freetype/src/base/ftfstype.c",
    "src/freetype/src/base/ftgasp.c",
    "src/freetype/src/base/ftglyph.c",
    "src/freetype/src/base/ftgxval.c",
    "src/freetype/src/base/ftinit.c",
    "src/freetype/src/base/ftmm.c",
    "src/freetype/src/base/ftotval.c",
    "src/freetype/src/base/ftpatent.c",
    "src/freetype/src/base/ftpfr.c",
    "src/freetype/src/base/ftstroke.c",
    "src/freetype/src/base/ftsynth.c",
    "src/freetype/src/base/ftsystem.c",
    "src/freetype/src/base/fttype1.c",
    "src/freetype/src/base/ftwinfnt.c",
    "src/freetype/src/bdf/bdf.c",
    "src/freetype/src/bzip2/ftbzip2.c",
    "src/freetype/src/cache/ftcache.c",
    "src/freetype/src/cff/cff.c",
    "src/freetype/src/cid/type1cid.c",
    "src/freetype/src/dlg/dlgwrap.c",
    "src/freetype/src/gxvalid/gxvalid.c",
    "src/freetype/src/gzip/ftgzip.c",
    "src/freetype/src/lzw/ftlzw.c",
    "src/freetype/src/otvalid/otvalid.c",
    "src/freetype/src/pcf/pcf.c",
    "src/freetype/src/pfr/pfr.c",
    "src/freetype/src/psaux/psaux.c",
    "src/freetype/src/pshinter/pshinter.c",
    "src/freetype/src/psnames/psnames.c",
    "src/freetype/src/raster/raster.c",
    "src/freetype/src/sdf/sdf.c",
    "src/freetype/src/sfnt/sfnt.c",
    "src/freetype/src/smooth/smooth.c",
    "src/freetype/src/svg/svg.c",
    "src/freetype/src/truetype/truetype.c",
    "src/freetype/src/type1/type1.c",
    "src/freetype/src/type42/type42.c",
    "src/freetype/src/winfonts/winfnt.c",
]

xpdf_src = [str(xpdf_dir / filename) for filename in xpdf_files]

freetype_include = [
    *glob("src/freetype/src/*/"),
    "src/freetype/include",
    "src/freetype/include/freetype",
    "src/freetype/include/freetype/config",
    "src/freetype/include/freetype/internal",
]

if os.name == 'nt':
    linker_libs = ['Ole32','AdvAPI32','shell32']
else:
    linker_libs = []
    
cXpdfPython = Extension(
    "cXpdfPython",
    sources=python_src + xpdf_src + splash_src + goo_src + fofi_src + freetype_sources,
    include_dirs=[
        "src/xpdf-4.04",
        str(xpdf_dir),
        str(fofi_dir),
        str(splash_dir),
        str(goo_dir),
        str(python_dir),
        np.get_include(),
        *freetype_include,
    ],
    libraries = linker_libs,
    extra_compile_args=[
        "-DFT2_BUILD_LIBRARY",
    ],
    define_macros=[
        ("FT_CONFIG_MODULES_H", "<ftmodule.h>"),
        ("FT_CONFIG_OPTIONS_H", "<ftoption.h>"),
    ],

)

class custom_build_ext(build_ext):
    def build_extensions(self):
        # Force building all files with clang++, cannot combine clang and clang++ I think :'(
        # It'll throw some deprecation warnings but eh, it works
        self.compiler.set_executable("compiler_so", "clang++ -std=c++11 -Wno-unused-result -Wsign-compare -Wunreachable-code -fno-common -dynamic -DNDEBUG -g -fwrapv -O3 -Wall -isysroot /Library/Developer/CommandLineTools/SDKs/MacOSX13.sdk")
        build_ext.build_extensions(self)

setup(ext_modules=[cXpdfPython], cmdclass={"build_ext": custom_build_ext})
