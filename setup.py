from glob import glob
from pathlib import Path
from setuptools import Extension, setup


python_dir = Path('src/xpdf-python')
xpdf_dir = Path('src/xpdf-4.04/xpdf')
splash_dir = Path('src/xpdf-4.04/splash')
fofi_dir = Path('src/xpdf-4.04/fofi')
goo_dir = Path('src/xpdf-4.04/goo')

python_src = glob(str(python_dir / "*.cc"))
splash_src = glob(str(splash_dir / "*.cc"))
fofi_src = glob(str(fofi_dir / "*.cc"))
goo_src = glob(str(goo_dir / "*.cc"))

xpdf_files = [
    'GlobalParams.cc',
    'Object.cc',
    'Stream.cc',
    'Array.cc',
    'Dict.cc',
    'XRef.cc',
    'Catalog.cc',
    'Page.cc',
    'PDFDoc.cc',
    'TextOutputDev.cc',
    'UnicodeMap.cc',
    'TextString.cc',
    'Error.cc',
    'UTF8.cc',
    'UnicodeTypeTable.cc',
    'BuiltinFontTables.cc',
    'JBIG2Stream.cc',
    'NameToCharCode.cc',
    'OptionalContent.cc',
    'SecurityHandler.cc',
    'UnicodeRemapping.cc',
    'BuiltinFont.cc',
    'CharCodeToUnicode.cc',
    'Gfx.cc',
    'GfxState.cc',
    'GfxFont.cc',
    'Annot.cc',
    'Lexer.cc',
    'Link.cc',
    'Parser.cc',
    'PSTokenizer.cc',
    'JArithmeticDecoder.cc',
    'Decrypt.cc',
    'AcroForm.cc',
    'Outline.cc',
    'Function.cc',
    'CMap.cc',
    'JPXStream.cc',
    'OutputDev.cc',
    'PDF417Barcode.cc',
    'XFAScanner.cc',
    'FontEncodingTables.cc',
    'Zoox.cc',
    'PDFDocEncoding.cc',
]

xpdf_src = [str(xpdf_dir / filename) for filename in xpdf_files]

cXpdfPython = Extension(
    'cXpdfPython',
    sources = python_src + xpdf_src + splash_src + goo_src + fofi_src,
    include_dirs = [
        'src/xpdf-4.04',
        str(xpdf_dir),
        str(fofi_dir),
        str(splash_dir),
        str(goo_dir),
        str(python_dir),
    ],
    extra_compile_args=[
        '-std=c++11',
    ]
)

setup(ext_modules = [cXpdfPython])