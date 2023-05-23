#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

#include <Python.h>
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>

#include "PdfLoader.h"
#include "PyCppConversion.h"
#include "ErrorCodes.h"

using namespace std;

// ------------------- Class wrappers -------------------

PyObject *construct(PyObject *self, PyObject *args) {
    PyObject *pobj0;
    Py_ssize_t size;
    LoaderConfig config;

    char *ownerPw = NULL;
    char *userPw = NULL;

    PyArg_ParseTuple(args, "Opppppppbzz", &pobj0,
        &(config.clipText),
        &(config.discardDiag),
        &(config.discardRotatedText),
        &(config.verbose),
        &(config.quiet),
        &(config.mode),
        &(config.mapNumericCharNames),
        &(config.mapUnknownCharNames),
        &ownerPw,
        &userPw
    );
    
    char *fileName = (char *)PyUnicode_AsUTF8AndSize(pobj0, &size);
    
    PdfLoader *loader = new PdfLoader(config, fileName, ownerPw, userPw);

    if (!loader->isOk()) {
        int errCode = NULL;
        errCode = loader->getErrorCode();
        char message[1000] = "";
        switch (errCode) {        
          default:
            snprintf(message, 1000, "Error loading file %s", fileName);
            PyErr_SetString(PyExc_Exception, message);
            break;
          case errOpenFile: // couldn't open the PDF file
            snprintf(message, 1000, "Error opening file %s", fileName);
            PyErr_SetString(PyExc_OSError, message);
            break;
          case errBadCatalog: // couldn't read the page catalog
            snprintf(message, 1000, "Error parsing PDF catalog in file %s", fileName);
            PyErr_SetString(PyExc_IOError, message);
            break;
          case errDamaged: // PDF file was damaged and couldn't be repaired
            snprintf(message, 1000, "Error parsing PDF file %s. File might be damaged", fileName);            
            PyErr_SetString(PyExc_IOError, message);
            break;
          case errEncrypted: // file was encrypted and password was incorrect or not supplied
            snprintf(message, 1000, "Error decrypting PDF file %s", fileName);
            PyErr_SetString(PyExc_PermissionError, message);
            break;
          case errHighlightFile: // nonexistent or invalid highlight file
            snprintf(message, 1000, "Error highlight PDF file %s", fileName);
            PyErr_SetString(PyExc_OSError, message);
            break;
          case errBadPrinter: // invalid printer
            snprintf(message, 1000, "Error bad printer" );
            PyErr_SetString(PyExc_Exception, message);
            break;
          case errPrinting: // error during printing
            snprintf(message, 1000, "Error printing" );
            PyErr_SetString(PyExc_Exception, message);
            break;
          case errPermission: // PDF file doesn't allow that operation
            snprintf(message, 1000, "Error permissions PDF file %s", fileName);
            PyErr_SetString(PyExc_PermissionError, message);
            break;
          case errBadPageNum: // invalid page number
            snprintf(message, 1000, "Error bad PDF page number in file %s", fileName);
            PyErr_SetString(PyExc_IOError, message);
            break;
          case errFileIO: // file I/O error
            snprintf(message, 1000, "Error while read/write file %s", fileName);
            PyErr_SetString(PyExc_OSError, message);
            break;
        }
        delete loader;
        return NULL;
    }

    PyObject *loaderCapsule = PyCapsule_New((void *)loader, "loaderPtr", NULL);
    PyCapsule_SetPointer(loaderCapsule, (void *)loader);
    
    return loaderCapsule;
}

PyObject *extractText(PyObject *self, PyObject *args) {
    vector<string> res;
    
    PyObject *loaderCapsule;
    PyArg_ParseTuple(args, "O", &loaderCapsule);

    PdfLoader *loader = (PdfLoader *)PyCapsule_GetPointer(loaderCapsule, "loaderPtr");
    vector<string> result = loader->extractText();
    
    PyObject *converted = vectorStringToList(result);
    return Py_BuildValue("O", converted);
}

PyObject *extractPageInfo(PyObject *self, PyObject *args) {
    vector<string> res;
    
    PyObject *loaderCapsule;
    PyArg_ParseTuple(args, "O", &loaderCapsule);

    PdfLoader *loader = (PdfLoader *)PyCapsule_GetPointer(loaderCapsule, "loaderPtr");
    vector<PageImageInfo> result = loader->extractPageInfo();
    
    PyObject *converted = vectorPagesToList(result);
    return Py_BuildValue("O", converted);
}

PyObject *extractImages(PyObject *self, PyObject *args) {
    vector<string> res;
    
    PyObject *loaderCapsule;
    int pageNum;
    PyArg_ParseTuple(args, "Oi", &loaderCapsule, &pageNum);

    PdfLoader *loader = (PdfLoader *)PyCapsule_GetPointer(loaderCapsule, "loaderPtr");
    vector<Image> images = loader->extractImages(pageNum);
    
    PyObject *imageList = PyList_New(images.size());
    PyObject *array;
    
    for (size_t i = 0; i < images.size(); i++) {
      Image image = images[i];
      npy_intp dims[1] = {image.size};
      array = PyArray_SimpleNewFromData(1, dims, NPY_UINT8, image.data);
      
      // Only reshape P6 images, P4 contain multiple pixels per byte
      if (image.image_type == IMAGE_TYPES::P4) {
        PyObject *shape = Py_BuildValue("ii", image.height, image.width);
        array = PyArray_Reshape((PyArrayObject *)array, shape);
      } else if (image.image_type == IMAGE_TYPES::P5) {
        PyObject *shape = Py_BuildValue("ii", image.height, image.width);
        array = PyArray_Reshape((PyArrayObject *)array, shape);
      } else if (image.image_type == IMAGE_TYPES::P6) {
        PyObject *shape = Py_BuildValue("iii", image.height, image.width, 3);
        array = PyArray_Reshape((PyArrayObject *)array, shape);
      }

      PyList_SetItem(imageList, i, array);
    }

    return Py_BuildValue("O", imageList);
}

PyObject *pageToImage(PyObject *self, PyObject *args) {
    vector<string> res;
    
    PyObject *loaderCapsule;
    int pageNum, dpi;
    PyArg_ParseTuple(args, "Oii", &loaderCapsule, &pageNum, &dpi);

    PdfLoader *loader = (PdfLoader *)PyCapsule_GetPointer(loaderCapsule, "loaderPtr");
    Image pageImage = loader->pageToImage(pageNum, dpi);
    
    PyObject *array;
    
    npy_intp dims[1] = {pageImage.size};
    array = PyArray_SimpleNewFromData(1, dims, NPY_UINT8, pageImage.data);
    
    PyObject *shape = Py_BuildValue("iii", pageImage.height, pageImage.width, 3);
    array = PyArray_Reshape((PyArrayObject *)array, shape);

    return Py_BuildValue("O", array);
}

PyObject *deleteObject(PyObject *self, PyObject *args) {
    PyObject *loaderCapsule;
    PyArg_ParseTuple(args, "O", &loaderCapsule);
    
    PdfLoader *loader = (PdfLoader *)PyCapsule_GetPointer(loaderCapsule, "loaderPtr");
    
    if (loader) {
        delete loader;
    }

    void *context = PyCapsule_GetContext(loaderCapsule);
    
    if (context) {
        delete context;
    }
    
    return Py_BuildValue("");
}

PyMethodDef cXpdfPythonFunctions[] = {
    {"construct",
      construct, METH_VARARGS,
     "Create `PdfLoader` object"},
    
    {"extractText",
      extractText, METH_VARARGS,
     "Extract text as bytes"},
    
    {"extractPageInfo",
      extractPageInfo, METH_VARARGS,
     "Extract image metadata"},
    
    {"extractImages",
      extractImages, METH_VARARGS,
     "Extract images"},
    
    {"pageToImage",
      pageToImage, METH_VARARGS,
     "Convert a page to an image"},
    
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
   "Python wrapper around the pdftotext functionality of xpdf.", 
   // Docstring for the module.
   -1,                   // Used by sub-interpreters, if you do not know what
                         // it is then you do not need it, keep -1 .
   cXpdfPythonFunctions
};

PyMODINIT_FUNC PyInit_cXpdfPython(void) {
    import_array();
    return PyModule_Create(&cXpdfPythonModule);
}
