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

#include "PdfLoader.h"
#include "ImageInfoDev.h"


static void outputToStringStream(void *stream, const char *text, int len) {
  ((std::stringstream *)stream)->write(text, len);
}


PdfLoader::PdfLoader(LoaderConfig config, char *fileName, char *ownerPw, char *userPw) {
  if (globalParams == NULL) {
    globalParams = new GlobalParams("");
  }

  globalParams->setPrintStatusInfo(config.verbose);
  globalParams->setErrQuiet(config.quiet);
  globalParams->setMapNumericCharNames(gFalse);
  globalParams->setMapUnknownCharNames(gTrue);

  switch (config.mode) {
    default:
    case 0:
      textOutControl.mode = textOutTableLayout;
      break;
    case 1:
      textOutControl.mode = textOutSimpleLayout;
      break;
    case 2:
      textOutControl.mode = textOutLinePrinter;
      break;
    case 3:
      textOutControl.mode = textOutPhysLayout;
      break;
  }

  textOutControl.clipText = config.clipText;
  textOutControl.discardDiagonalText = config.discardDiag;
  textOutControl.discardRotatedText = config.discardRotatedText;
  textOutControl.insertBOM = config.insertBOM;

  textFileName = new GString(fileName);

  GString *ownerPwGs = NULL;
  GString *userPwGs = NULL;
  
  if (ownerPw) {
    ownerPwGs = new GString(ownerPw);
  }
  if (userPw) {
    userPwGs = new GString(userPw);
  }
    
  doc = new PDFDoc(fileName, ownerPwGs, userPwGs);

  if (ownerPwGs) {
    delete ownerPwGs;
  }
  if (userPwGs) {
    delete userPwGs;
  }
}

PdfLoader::~PdfLoader() {
  delete textFileName;
  delete doc;

  Object::memCheck(stderr);
  gMemReport(stderr);
}

std::vector<std::string> PdfLoader::extractText() {
  TextOutputDev *textOut;
  std::stringstream *stream = new std::stringstream();
  std::vector<std::string> pages;
  int firstPage, lastPage;

  if (!doc->isOk()) {
    goto err;
  }

  firstPage = 1;
  lastPage = doc->getNumPages();
  
  textOut = new TextOutputDev(&outputToStringStream, stream, &textOutControl);

  if (textOut->isOk()) {
    for (int page = firstPage; page <= lastPage; page++) {
      stream->str("");
      doc->displayPages(textOut, page, page, 72, 72, 0, gFalse, gTrue, gFalse);
      pages.push_back(stream->str());
    }
  } 

  delete textOut;
err:
  delete stream;

  Object::memCheck(stderr);
  gMemReport(stderr);

  return pages;
}

std::vector<PageImageInfo> PdfLoader::extractImages() {
  ImageInfoDev *imageOut;
  int firstPage, lastPage;
  std::vector<PageImageInfo> pagesInfo;
  char dummyFile[1] = "";

  if (!doc->isOk()) {
    goto err;
  }

  firstPage = 1;
  lastPage = doc->getNumPages();
  
  imageOut = new ImageInfoDev(dummyFile, gFalse, gFalse, gTrue);

  if (imageOut->isOk()) {
    for (int page = firstPage; page <= lastPage; page++) {
      Page *pdfpage = doc->getCatalog()->getPage(page);
      PDFRectangle *box = pdfpage->getTrimBox();
      double page_width = box->x2 - box->x1;
      double page_height = box->y2 - box->y1;
      
      doc->displayPages(imageOut, page, page, 72, 72, 0, gFalse, gTrue, gFalse);

      std::vector<ImageInfo> images(imageOut->images);
      pagesInfo.push_back(PageImageInfo{
        page,
        page_width,
        page_height,
        images,
      });

    }
  }

  delete imageOut;
 err:

  Object::memCheck(stderr);
  gMemReport(stderr);

  return pagesInfo;
}

bool PdfLoader::isOk() {
  if (!doc) {
    return false;
  }

  return (bool)doc->isOk();
}

// int main() {
//   LoaderConfig cfg = LoaderConfig();
//   cfg.quiet = false;
//   cfg.verbose = true;
//   PdfLoader loader = PdfLoader(cfg, "../NL-PDF-Extractietool/data/batches/batch-0/e6155a39-a9bb-11ec-b6f3-005056a6ed7a.pdf");
//   std::vector<std::string> result = loader.extractText();
//   for (auto page : result) {
//     fprintf(stderr, "%s\n", page.c_str());
//   }
// }
