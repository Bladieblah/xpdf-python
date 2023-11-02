#include <stdio.h>
#include <iostream>
#include <map>

#include "Python.h"

#include "PdfLoader.h"
#include "PyCppConversion.h"

using namespace std;

// ------------------- Helpers for converting between Python and C++ -------------------

PyObject *vectorStringToList(const vector<string> &data) {
    PyObject *listObj = PyList_New(data.size());
    if (!listObj) throw logic_error("Unable to allocate memory for Python list");

    for (size_t i = 0; i < data.size(); i++) {
        PyObject *num = PyBytes_FromStringAndSize(data[i].c_str(), data[i].size());

        if (!num) {
            Py_DECREF(listObj);
            throw logic_error("Unable to allocate memory for Python string");
        }

        PyList_SET_ITEM(listObj, i, num);
    }

    return listObj;
}

PyObject *vectorPagesToList(const std::vector<PageImageInfo> &data) {
    PyObject *listObj = PyList_New(data.size());
    if (!listObj) throw logic_error("Unable to allocate memory for Python list");

    for (size_t i = 0; i < data.size(); i++) {
        PyObject *dictObj = PyDict_New();
        if (!dictObj) throw logic_error("Unable to allocate memory for Python dict");

        PyDict_SetItemString(dictObj, "page_number", PyLong_FromLong((long)data[i].pageNum));
        PyDict_SetItemString(dictObj, "width", PyFloat_FromDouble(data[i].width));
        PyDict_SetItemString(dictObj, "height", PyFloat_FromDouble(data[i].height));
        PyDict_SetItemString(dictObj, "images", vectorImagesToList(data[i].images));

        PyList_SET_ITEM(listObj, i, dictObj);
    }

    return listObj;
}

PyObject *vectorImagesToList(const std::vector<ImageInfo> &data) {
    PyObject *listObj = PyList_New(data.size());
    if (!listObj) throw logic_error("Unable to allocate memory for Python list");

    for (size_t i = 0; i < data.size(); i++) {
        PyObject *dictObj = PyDict_New();
        if (!dictObj) throw logic_error("Unable to allocate memory for Python dict");

        PyDict_SetItemString(dictObj, "width", PyFloat_FromDouble(data[i].width));
        PyDict_SetItemString(dictObj, "height", PyFloat_FromDouble(data[i].height));

        PyList_SET_ITEM(listObj, i, dictObj);
    }

    return listObj;
}

PyObject *mapFontSpecsToDict(const std::map<unsigned int, NamedFontSpec> &data) {
    PyObject *dict = PyDict_New();
    if (!dict) throw logic_error("Unable to allocate memory for Python dict");

    for (auto pair : data) {
        PyObject *item = PyDict_New();
        if (!item) throw logic_error("Unable to allocate memory for Python dict");

        PyObject *id = PyLong_FromLong(pair.first);
        PyObject *name = PyUnicode_FromString(pair.second.fontName.c_str());
        PyObject *type = PyUnicode_FromString(pair.second.fontType.c_str());

        PyDict_SetItemString(item, "name", name);
        PyDict_SetItemString(item, "type", type);
        PyDict_SetItemString(item, "size", PyLong_FromLong(pair.second.fontSize));

        PyDict_SetItem(dict, id, item);
    }

    return dict;
}
