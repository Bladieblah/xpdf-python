#ifndef PY_CPP_CONVERSION_H
#define PY_CPP_CONVERSION_H

#include <map>
#include <string>
#include <vector>

#include "Python.h"

#include "PdfLoader.h"

// ------------------- Helpers for converting between Python and C++ -------------------

PyObject *vectorStringToList(const std::vector<std::string> &data);

PyObject *vectorPagesToList(const std::vector<PageImageInfo> &data);

PyObject *vectorImagesToList(const std::vector<ImageInfo> &data);

PyObject *mapFontSpecsToDict(const std::map<unsigned int, NamedFontSpec> &data);

#endif