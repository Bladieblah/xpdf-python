#include <aconf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <sstream>
#include <vector>

#include "gmem.h"
#include "gmempp.h"
#include "parseargs.h"
#include "GString.h"
#include "GList.h"
#include "GlobalParams.h"
#include "Object.h"
#include "Stream.h"
#include "Array.h"
#include "Dict.h"
#include "XRef.h"
#include "Catalog.h"
#include "Page.h"
#include "PDFDoc.h"
#include "TextOutputDev.h"
#include "CharTypes.h"
#include "UnicodeMap.h"
#include "TextString.h"
#include "Error.h"
#include "config.h"

#include "pdftostring.h"


static void outputToStringStream(void *stream, const char *text, int len) {
  ((std::stringstream *)stream)->write(text, len);
}


PdfToString::PdfToString(PTSConfig config) {
  // read config file
  if (globalParams == NULL) {
    globalParams = new GlobalParams("");
  }

  if (config.verbose) {
    globalParams->setPrintStatusInfo(config.verbose);
  }

  if (config.quiet) {
    globalParams->setErrQuiet(config.quiet);
  }

  textOutControl.mode = textOutTableLayout;
  textOutControl.clipText = config.clipText;
  textOutControl.discardDiagonalText = config.discardDiag;
  textOutControl.discardRotatedText = config.discardRotatedText;
  textOutControl.insertBOM = config.insertBOM;
}

std::vector<std::string> PdfToString::loadFile(char *fileName) {
  PDFDoc *doc;
  GString *textFileName;
  TextOutputDev *textOut;
  int firstPage, lastPage;

  std::stringstream *stream = new std::stringstream();
  std::vector<std::string> pages;

  doc = new PDFDoc(fileName);

  if (!doc->isOk()) {
    goto err2;
  }

  // construct text file name
  textFileName = new GString(fileName);

  firstPage = 1;
  lastPage = doc->getNumPages();
  
  textOut = new TextOutputDev(&outputToStringStream, stream, &textOutControl);

  if (textOut->isOk()) {
    for (int page = firstPage; page <= lastPage; page++) {
      stream->str("");
      doc->displayPages(textOut, page, page, 72, 72, 0, gFalse, gTrue, gFalse);
      pages.push_back(stream->str());
    }
  } else {
    delete textOut;
    goto err3;
  }
  delete textOut;

  // clean up
 err3:
  delete textFileName;
 err2:
  delete doc;
  delete stream;

  // check for memory leaks
  Object::memCheck(stderr);
  gMemReport(stderr);

  return pages;
}

PdfToString::~PdfToString() {
  delete globalParams;
}


int main(int argc, char **argv) {
  PTSConfig config;

  if (argc == 2) {
    int i = 0;
    PdfToString *pts = new PdfToString(config);
    std::vector<std::string> result = pts->loadFile(argv[1]);

    for (auto page : result) {
      i++;
      fprintf(stderr, "--------------------------------------- PAGE %d ---------------------------------------\n", i);
      fprintf(stderr, "%s", page.c_str());
    }
  }

  return 0;
}
