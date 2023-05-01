import os
from glob import glob
from pathlib import Path
import numpy as np

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

xpdf_src = [str(xpdf_dir / filename) for filename in xpdf_files]

if os.name == 'nt':
    linker_libs = ['Ole32','AdvAPI32','shell32']
else:
    linker_libs = []
    
cXpdfPython = Extension(
    "cXpdfPython",
    sources=python_src + xpdf_src + splash_src + goo_src + fofi_src,
    include_dirs=[
        "src/xpdf-4.04",
        str(xpdf_dir),
        str(fofi_dir),
        str(splash_dir),
        str(goo_dir),
        str(python_dir),
        np.get_include(),
    ],
    libraries = linker_libs,
    extra_compile_args=[
        "-std=c++11",
    ],
)

setup(ext_modules=[cXpdfPython])
