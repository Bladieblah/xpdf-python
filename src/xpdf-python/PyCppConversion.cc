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

    for (int i = 0; i < data.size(); i++) {
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

    for (int i = 0; i < data.size(); i++) {
        PyObject *dictObj = PyDict_New();
        if (!dictObj) throw logic_error("Unable to allocate memory for Python dict");

        PyDict_SetItemString(dictObj, "page_number", PyLong_FromLong((long)data[i].pageNum));
        PyDict_SetItemString(dictObj, "width", PyFloat_FromDouble(data[i].width));
        PyDict_SetItemString(dictObj, "height", PyFloat_FromDouble(data[i].height));
        PyDict_SetItemString(dictObj, "pages", vectorImagesToList(data[i].images));

        PyList_SET_ITEM(listObj, i, dictObj);
    }

    return listObj;
}

PyObject *vectorImagesToList(const std::vector<ImageInfo> &data) {
    PyObject *listObj = PyList_New(data.size());
    if (!listObj) throw logic_error("Unable to allocate memory for Python list");

    for (int i = 0; i < data.size(); i++) {
        PyObject *dictObj = PyDict_New();
        if (!dictObj) throw logic_error("Unable to allocate memory for Python dict");

        PyDict_SetItemString(dictObj, "width", PyFloat_FromDouble(data[i].width));
        PyDict_SetItemString(dictObj, "height", PyFloat_FromDouble(data[i].height));

        PyList_SET_ITEM(listObj, i, dictObj);
    }

    return listObj;
}
