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
#include "SplashOutputDev.h"
#include "SplashBitmap.h"

#include "PdfLoader.h"
#include "ImageDataDev.h"
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
  globalParams->setMapNumericCharNames(config.mapNumericCharNames);
  globalParams->setMapUnknownCharNames(config.mapUnknownCharNames);
  globalParams->setupBaseFonts(NULL);

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

std::vector<PageImageInfo> PdfLoader::extractPageInfo() {
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
      pagesInfo.push_back((PageImageInfo){
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

std::vector<Image> PdfLoader::extractImages(int pageNum) {
  ImageDataDev *imageOut;
  std::vector<Image> images;
  char dummyFile[1] = "";

  if (!doc->isOk()) {
    goto err;
  }
  
  imageOut = new ImageDataDev(dummyFile, gFalse, gFalse, gTrue, &images);

  if (imageOut->isOk()) {
    doc->displayPages(imageOut, pageNum, pageNum, 72, 72, 0, gFalse, gTrue, gFalse);
  }

  delete imageOut;
 err:

  Object::memCheck(stderr);
  gMemReport(stderr);

  return images;
}

Image PdfLoader::pageToImage(int pageNum) {
  Image pageImage;
  SplashColor paperColor;
  SplashOutputDev *splashOut;
  SplashBitmap *bitmap;
  bool pngAlpha = false;

  if (!doc->isOk()) {
    goto err;
  }
  
  paperColor[0] = paperColor[1] = paperColor[2] = 0xff;
  splashOut = new SplashOutputDev(splashModeRGB8, 1, gFalse, paperColor);
  if (pngAlpha)
  {
    splashOut->setNoComposite(gTrue);
  }
  
  splashOut->startDoc(doc->getXRef());

  doc->displayPages(splashOut, pageNum, pageNum, 72, 72, 0, gFalse, gTrue, gFalse);
  bitmap = splashOut->getBitmap();
  
  pageImage = {
    static_cast<unsigned int>(IMAGE_TYPES::P6),
    static_cast<unsigned int>(bitmap->getWidth()),
    static_cast<unsigned int>(bitmap->getHeight()),
    static_cast<unsigned int>(3 * bitmap->getWidth() * bitmap->getHeight()),
    nullptr
  };

  pageImage.data = (unsigned char *)malloc(pageImage.size * sizeof(unsigned char));
  memcpy(pageImage.data, bitmap->getDataPtr(), pageImage.size);

  delete splashOut;
 err:

  Object::memCheck(stderr);
  gMemReport(stderr);

  return pageImage;
}

bool PdfLoader::isOk() {
  if (!doc) {
    return false;
  }  
  return (bool)doc->isOk();
}

int PdfLoader::getErrorCode() {
  return (int)doc->getErrorCode();
}

int main() {
  // char filename[100] = "fc96f406-777c-11eb-8637-005056a6ed7a.pdf";
  // char filename[100] = "../NL-PDF-Extractietool/data/input/9999748983.pdf";
  char filename[100] = "../NL-PDF-Extractietool/data/input/Hamonterweg 19 VO+aanv 13-03-2014 Tritium 1401076TB-01.pdf";
  LoaderConfig config;
  config.quiet = false;
  PdfLoader loader = PdfLoader(config, filename);
  fprintf(stderr, "%s\n", loader.isOk() ? "Loader ok" : "Loader Err");

  // std::vector<std::string> pages = loader.extractText();

  // for (size_t i = 0; i < pages.size(); i++) {
  //   // fprintf(stderr, "############################ PAGE %.3zu ############################\n\n%s\n\n", i, pages[i].c_str());
  //   fprintf(stderr, "############################ PAGE %.3zu ############################\n\n", i);
  //   std::vector<Image> images = loader.extractImages(i+1);
  // }

  Image page = loader.pageToImage(1);
  fprintf(stderr, "Image shape %d x %d\n", page.width, page.height);


  return 0;
}
