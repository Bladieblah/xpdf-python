#include <stdio.h>
#include <iostream>
#include <map>

#include "Python.h"
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
