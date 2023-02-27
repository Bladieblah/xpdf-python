#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

#include "Python.h"
#include "PdfLoader.h"
#include "PyCppConversion.h"

using namespace std;

// ------------------- Class wrappers -------------------

PyObject *construct(PyObject *self, PyObject *args) {
    PyObject *pobj0;
    Py_ssize_t size;
    PyArg_ParseTuple(args, "O", &pobj0);
    const char *fileName = PyUnicode_AsUTF8AndSize(pobj0, &size);
    
    LoaderConfig config;
    PdfLoader *loader = new PdfLoader(config, fileName);
    
    PyObject *loaderCapsule = PyCapsule_New((void *)loader, "loaderPtr", NULL);
    PyCapsule_SetPointer(loaderCapsule, (void *)loader);
    
    return Py_BuildValue("O", loaderCapsule);
}

PyObject *extractText(PyObject *self, PyObject *args) {
    vector<string> res;
    
    PyObject *loaderCapsule;
    PyArg_ParseTuple(args, "O", &loaderCapsule);

    PdfLoader *loader = (PdfLoader *)PyCapsule_GetPointer(loaderCapsule, "loaderPtr");
    vector<string> result = loader->extractText();
    
    PyObject *converted = vectorStringToList(result);
    return Py_BuildValue("OO", loaderCapsule, converted);
}

PyObject *extractImages(PyObject *self, PyObject *args) {
    vector<string> res;
    
    PyObject *loaderCapsule;
    PyArg_ParseTuple(args, "O", &loaderCapsule);

    PdfLoader *loader = (PdfLoader *)PyCapsule_GetPointer(loaderCapsule, "loaderPtr");
    vector<PageImageInfo> result = loader->extractImages();
    
    PyObject *converted = vectorPagesToList(result);
    return Py_BuildValue("OO", loaderCapsule, converted);
}

PyObject *deleteObject(PyObject *self, PyObject *args) {
    PyObject *loaderCapsule;
    PyArg_ParseTuple(args, "O", &loaderCapsule);
    
    PdfLoader *loader = (PdfLoader *)PyCapsule_GetPointer(loaderCapsule, "loaderPtr");
    
    delete loader;
    
    return Py_BuildValue("");
}

PyMethodDef cXpdfPythonFunctions[] = {
    {"construct",
      construct, METH_VARARGS,
     "Create `PdfLoader` object"},
    
    {"extractText",
      extractText, METH_VARARGS,
     "Extract text as bytes"},
    
    {"extractImages",
      extractImages, METH_VARARGS,
     "Extract image metadata"},
    
    {"deleteObject",
      deleteObject, METH_VARARGS,
     "Delete `PdfLoader` object"},

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