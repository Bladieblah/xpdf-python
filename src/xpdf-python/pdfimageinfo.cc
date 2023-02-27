#include <aconf.h>
#include <sstream>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "Array.h"
#include "Catalog.h"
#include "CharTypes.h"
#include "config.h"
#include "Dict.h"
#include "Error.h"
#include "Error.h"
#include "GfxState.h"
#include "GList.h"
#include "GlobalParams.h"
#include "gmem.h"
#include "gmempp.h"
#include "GString.h"
#include "ImageOutputDev.h"
#include "Object.h"
#include "Outputdev.h"
#include "Page.h"
#include "parseargs.h"
#include "PDFDoc.h"
#include "pdftostring.h"
#include "Stream.h"
#include "TextOutputDev.h"
#include "TextString.h"
#include "UnicodeMap.h"
#include "XRef.h"


class ImageInfoDev: public ImageOutputDev {
public:
  ImageInfoDev(char *fileRootA, GBool dumpJPEGA, GBool dumpRawA,
		 GBool listA) : ImageOutputDev(fileRootA, dumpJPEGA, dumpRawA, listA) {};

  virtual void startPage(int pageNum, GfxState *state);
  virtual void drawImage(GfxState *state, Object *ref, Stream *str,
    int width, int height, GfxImageColorMap *colorMap,
    int *maskColors, GBool inlineImg, GBool interpolate);
    
  void printInfo(
    int width, int height, GfxState *state,
    GfxImageColorMap *colorMap);


  int curPageNum;
};


void ImageInfoDev::drawImage(GfxState *state, Object *ref, Stream *str,
  int width, int height,
  GfxImageColorMap *colorMap,
  int *maskColors, GBool inlineImg,
  GBool interpolate
) {
  int size, n, i, j;

  printInfo(width, height, state, colorMap);
}

void ImageInfoDev::printInfo(
  int width, int height, GfxState *state,
  GfxImageColorMap *colorMap
) {

}

void ImageInfoDev::startPage(int pageNum, GfxState *state) {
  curPageNum = pageNum;
}

void loadFile(const char *fileName) {
  PDFDoc *doc;
  GString *textFileName;
  ImageInfoDev *imageOut;
  int firstPage, lastPage;

  std::stringstream *stream = new std::stringstream();
  std::vector<std::string> pages;

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
      stream->str("");
      doc->displayPages(imageOut, page, page, 72, 72, 0, gFalse, gTrue, gFalse);
      pages.push_back(stream->str());
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
  delete stream;

  // check for memory leaks
  Object::memCheck(stderr);
  gMemReport(stderr);
}



int main(int argc, char **argv) {
  PTSConfig config;

  if (argc == 2) {
    int i = 0;
    PdfToString *pts = new PdfToString(config);
    std::vector<std::string> result = pts->loadFile(argv[1]);

    for (auto page : result) {
      i++;
      // fprintf(stderr, "--------------------------------------- PAGE %d ---------------------------------------\n", i);
      // fprintf(stderr, "%s", page.c_str());
    }
  }

  return 0;
}


int main(int argc, char **argv) {
  if (argc == 2) {
    loadFile(argv[1]);
  }

  return 0;
}
