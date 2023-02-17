from setuptools import Extension, setup

XPDFCPP = Extension(
    'XPDFCPP',
    sources = [
        'src/xpdf-4.04/normal_cdf.cpp',
    ],
    include_dirs = [
        'src/xpdf-4.04',
    ],
    extra_compile_args=[
        '-std=c++11',
    ]
)

setup(ext_modules = [XPDFCPP])