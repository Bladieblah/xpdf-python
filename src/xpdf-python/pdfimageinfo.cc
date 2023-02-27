#include <aconf.h>

#include <sstream>
#include <stddef.h>
#include <stdlib.h>

#include "GlobalParams.h"
#include "gmem.h"
#include "gmempp.h"
#include "config.h"
#include "Error.h"
#include "GfxState.h"
#include "Object.h"
#include "PDFDoc.h"
#include "Stream.h"

#include "ImageInfoDev.h"
#include "pdfimageinfo.h"

PdfImageInfo::PdfImageInfo(PIIConfig config) {
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
}

void PdfImageInfo::loadFile(const char *fileName) {
  PDFDoc *doc;
  GString *textFileName;
  ImageInfoDev *imageOut;
  int firstPage, lastPage;

  textFileName = new GString(fileName);

  // Apparently initialising the pdfdoc with a GString is broken when you delete it
  char *fileNameArray = (char *)malloc((int)strlen(fileName) * sizeof(char));
  strncpy(fileNameArray, fileName, (int)strlen(fileName));
  doc = new PDFDoc(fileNameArray);

  if (!doc->isOk()) {
    goto err2;
  }

  firstPage = 1;
  lastPage = doc->getNumPages();
  
  // textOut = new TextOutputDev(&outputToStringStream, stream, &textOutControl);
  imageOut = new ImageInfoDev(fileNameArray, gFalse, gFalse, gTrue);

  if (imageOut->isOk()) {
    for (int page = firstPage; page <= lastPage; page++) {
      // stream->str("");
      doc->displayPages(imageOut, page, page, 72, 72, 0, gFalse, gTrue, gFalse);
      // pages.push_back(stream->str());
    }
  } else {
    delete imageOut;
    goto err3;
  }
  delete imageOut;

  // clean up
 err3:
  delete textFileName;
 err2:
  delete doc;
  // delete stream;

  // check for memory leaks
  Object::memCheck(stderr);
  gMemReport(stderr);
}

PdfImageInfo::~PdfImageInfo() {
  delete globalParams;
}



int main(int argc, char **argv) {
  PIIConfig config;

  if (argc == 2) {
    int i = 0;
    PdfImageInfo *pii = new PdfImageInfo(config);
    pii->loadFile(argv[1]);
  }

  return 0;
}
