#include <aconf.h>

#include <cmath>
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

std::vector<PageImageInfo> PdfImageInfo::loadFile(const char *fileName) {
  PDFDoc *doc;
  GString *textFileName;
  ImageInfoDev *imageOut;
  int firstPage, lastPage;
  std::vector<PageImageInfo> pagesInfo;

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
  
  imageOut = new ImageInfoDev(fileNameArray, gFalse, gFalse, gTrue);

  if (imageOut->isOk()) {
    for (int page = firstPage; page <= lastPage; page++) {
      Page *pdfpage = doc->getCatalog()->getPage(page);
      PDFRectangle *box = pdfpage->getTrimBox();
      double page_width = box->x2 - box->x1;
      double page_height = box->y2 - box->y1;
      
      doc->displayPages(imageOut, page, page, 72, 72, 0, gFalse, gTrue, gFalse);

      std::vector<ImageInfo> images(imageOut->images);
      pagesInfo.push_back((PageImageInfo){
        page,
        page_width,
        page_height,
        images,
      });

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

  return pagesInfo;
}

PdfImageInfo::~PdfImageInfo() {
  delete globalParams;
}



int main(int argc, char **argv) {
  PIIConfig config;

  if (argc == 2) {
    PdfImageInfo *pii = new PdfImageInfo(config);
    std::vector<PageImageInfo> pages = pii->loadFile(argv[1]);

    for (auto page : pages) {
      fprintf(stderr, "Page %d has size (%.2f, %.2f)\n", page.pageNum, page.width, page.height);

      for (auto image : page.images) {
        fprintf(stderr, "    Image size (%.2f, %.2f)\n", image.width, image.height);
      }
    }
  }

  return 0;
}
