#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

#include "Python.h"

using namespace std;

// ------------------- Class wrappers -------------------

PyObject *construct(PyObject *self, PyObject *args) {
    PTSConfig config;
    PdfToString *pts = new PdfToString(config);
    vector<string> result = pts->loadFile(argv[1]);
    
    PyObject *ptsCapsule = PyCapsule_New((void *)pts, "ptsPtr", NULL);
    PyCapsule_SetPointer(ptsCapsule, (void *)pts);
    
    return Py_BuildValue("O", ptsCapsule);
}

PyObject *loadFile(PyObject *self, PyObject *args) {
    PyObject *pobj0;
    vector<string> res;
    string fileName;
    
    PyObject *ptsCapsule;
    PyArg_ParseTuple(args, "OO", &treeCapsule, &pobj0);
    
    PdfToString *pts = (PdfToString *)PyCapsule_GetPointer(ptsCapsule, "ptsPtr");

    pts->loadFile(fileName);
    
    return Py_BuildValue("O", treeCapsule);
}

PyObject *delete_object(PyObject *self, PyObject *args) {
    PyObject *ptsCapsule;
    PyArg_ParseTuple(args, "O", &ptsCapsule);
    
    PdfToString *pts = (PdfToString *)PyCapsule_GetPointer(ptsCapsule, "ptsPtr");
    
    delete pts;
    
    return Py_BuildValue("");
}

PyMethodDef PdfToStringFunctions[] = {
    {"construct",
      construct, METH_VARARGS,
     "Create `PdfToString` object"},
    
    {"loadFile",
      loadFile, METH_VARARGS,
     "Fit the classifier"},
    
    {"delete_object",
      delete_object, METH_VARARGS,
     "Delete `PdfToString` object"},

    {NULL, NULL, 0, NULL}      // Last function description must be empty.
                               // Otherwise, it will create seg fault while
                               // importing the module.
};


struct PyModuleDef PdfToStringModule = {
/*
 *  Structure which defines the module.
 *
 *  For more info look at: https://docs.python.org/3/c-api/module.html
 *
 */
   PyModuleDef_HEAD_INIT,
   "PdfToString",
   "Non-binary decision tree with an entropy-based splitting criterion.", 
   // Docstring for the module.
   -1,                   // Used by sub-interpreters, if you do not know what
                         // it is then you do not need it, keep -1 .
   PdfToStringFunctions
};


PyMODINIT_FUNC PyInit_PdfToString(void) {
    return PyModule_Create(&PdfToStringModule);
}