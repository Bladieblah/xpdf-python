#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

#include "Python.h"
#include "pdftostring.h"
#include "PyCppConversion.h"

using namespace std;

// ------------------- Class wrappers -------------------

PyObject *construct(PyObject *self, PyObject *args) {
    PTSConfig config;
    PdfToString *pts = new PdfToString(config);
    
    PyObject *ptsCapsule = PyCapsule_New((void *)pts, "ptsPtr", NULL);
    PyCapsule_SetPointer(ptsCapsule, (void *)pts);
    
    return Py_BuildValue("O", ptsCapsule);
}

PyObject *loadFile(PyObject *self, PyObject *args) {
    PyObject *pobj0;
    vector<string> res;
    Py_ssize_t size;
    
    PyObject *ptsCapsule;
    PyArg_ParseTuple(args, "OO", &ptsCapsule, &pobj0);
    const char *fileName = PyUnicode_AsUTF8AndSize(pobj0, &size);

    PdfToString *pts = (PdfToString *)PyCapsule_GetPointer(ptsCapsule, "ptsPtr");
    vector<string> result = pts->loadFile(fileName);
    
    PyObject *converted = vectorStringToList(result);
    return Py_BuildValue("OO", ptsCapsule, converted);
}

PyObject *deleteObject(PyObject *self, PyObject *args) {
    PyObject *ptsCapsule;
    PyArg_ParseTuple(args, "O", &ptsCapsule);
    
    PdfToString *pts = (PdfToString *)PyCapsule_GetPointer(ptsCapsule, "ptsPtr");
    
    delete pts;
    
    return Py_BuildValue("");
}

PyMethodDef cXpdfPythonFunctions[] = {
    {"construct",
      construct, METH_VARARGS,
     "Create `PdfToString` object"},
    
    {"loadFile",
      loadFile, METH_VARARGS,
     "Fit the classifier"},
    
    {"deleteObject",
      deleteObject, METH_VARARGS,
     "Delete `PdfToString` object"},

    {NULL, NULL, 0, NULL}      // Last function description must be empty.
                               // Otherwise, it will create seg fault while
                               // importing the module.
};


struct PyModuleDef cXpdfPythonModule = {
/*
 *  Structure which defines the module.
 *
 *  For more info look at: https://docs.python.org/3/c-api/module.html
 *
 */
   PyModuleDef_HEAD_INIT,
   "cXpdfPython",
   "Non-binary decision tree with an entropy-based splitting criterion.", 
   // Docstring for the module.
   -1,                   // Used by sub-interpreters, if you do not know what
                         // it is then you do not need it, keep -1 .
   cXpdfPythonFunctions
};


PyMODINIT_FUNC PyInit_cXpdfPython(void) {
    return PyModule_Create(&cXpdfPythonModule);
}