#include <aconf.h>
#include <map>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <sstream>
#include <vector>

#include <math.h>

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
#include "Annot.h"
#include "AcroForm.h"

#include "PdfLoader.h"
#include "FontOutputDev.h"
#include "ImageDataDev.h"
#include "ImageInfoDev.h"

#define PAGES 1062
#define PAGEE 1062


static void outputToStringStream(void *stream, const char *text, int len) {
  // fprintf(stderr, "Adding %d characters\n", len);
  // if (len > 1)
  //   fprintf(stderr, "(%.3d) %s\n", len, text);
  ((std::stringstream *)stream)->write(text, len);
}

static void outputToStringStream2(void *stream, const char *text, int len) {
  // fprintf(stderr, "Adding %d characters\n", len);
  // if (len > 1) {
  //   fprintf(stderr, "(%.3d) ", len);
  //   for (int i = 0; i < len; i++) {
  //     if (text[i] == ' ') {
  //       fprintf(stderr, " ");
  //     } else {
  //       fprintf(stderr, "x");
  //     }
  //   }

  //   fprintf(stderr, "\n");
  // }
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
  globalParams->setReadUnicodeCMap(config.readUnicodeCMap);
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

  firstPage = PAGES;
  lastPage = PAGEE;
  firstPage = 1;
  lastPage = doc->getNumPages();
  
  textOut = new TextOutputDev(&outputToStringStream, stream, &textOutControl);

  if (textOut->isOk()) {
    for (int page = firstPage; page <= lastPage; page++) {
      // fprintf(stderr, "Processing page %d\n", page);
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

std::vector<std::string> PdfLoader::extractFontMap(std::map<unsigned int, NamedFontSpec> &fontSpecs) {
  FontOutputDev *fontOut;
  std::stringstream *stream = new std::stringstream();
  std::vector<std::string> pages;
  int firstPage, lastPage;

  if (!doc->isOk()) {
    goto err;
  }

  firstPage = PAGES;
  lastPage = PAGEE;
  firstPage = 1;
  lastPage = doc->getNumPages();
  
  fontOut = new FontOutputDev(&outputToStringStream2, stream, &textOutControl);

  if (fontOut->isOk()) {
    for (int page = firstPage; page <= lastPage; page++) {
      // fprintf(stderr, "Processing page %d\n", page);
      stream->str("");
      doc->displayPages(fontOut, page, page, 72, 72, 0, gFalse, gTrue, gFalse);
      pages.push_back(stream->str());
    }
  }

  fontSpecs = fontOut->getFontSpecs();

  delete fontOut;
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

static Ref *fonts;
static int fontsLen;
static int fontsSize;

static char *seenObjs;
static int numObjects;

void PdfLoader::scanFonts(Object *obj) {
  Object obj2;

  if (checkFontObject(obj, &obj2) && obj2.isDict()) {
    scanFonts(obj2.getDict());
  }
  obj2.free();
}

void PdfLoader::scanFonts(Dict *resDict) {
  Object fontDict1, fontDict2, xObjDict1, xObjDict2, xObj1, xObj2;
  Object patternDict1, patternDict2, pattern1, pattern2;
  Object gsDict1, gsDict2, gs1, gs2, smask1, smask2, smaskGroup1, smaskGroup2;
  Object resObj;
  Ref r;
  GfxFontDict *gfxFontDict;
  GfxFont *font;
  int i;

  // scan the fonts in this resource dictionary
  gfxFontDict = NULL;
  resDict->lookupNF("Font", &fontDict1);
  if (checkFontObject(&fontDict1, &fontDict2) && fontDict2.isDict()) {
    if (fontDict1.isRef()) {
      r = fontDict1.getRef();
      gfxFontDict = new GfxFontDict(doc->getXRef(), &r, fontDict2.getDict());
    } else {
      gfxFontDict = new GfxFontDict(doc->getXRef(), NULL, fontDict2.getDict());
    }
    if (gfxFontDict) {
      for (i = 0; i < gfxFontDict->getNumFonts(); ++i) {
        if ((font = gfxFontDict->getFont(i))) {
          scanFont(font);
        }
      }
      delete gfxFontDict;
    }
  }

  fontDict2.free();
  fontDict1.free();

  // recursively scan any resource dictionaries in XObjects in this
  // resource dictionary
  resDict->lookupNF("XObject", &xObjDict1);
  if (checkFontObject(&xObjDict1, &xObjDict2) && xObjDict2.isDict()) {
    for (i = 0; i < xObjDict2.dictGetLength(); ++i) {
      xObjDict2.dictGetValNF(i, &xObj1);
      if (checkFontObject(&xObj1, &xObj2) && xObj2.isStream()) {
        xObj2.streamGetDict()->lookupNF("Resources", &resObj);
        scanFonts(&resObj);
        resObj.free();
      }
      xObj2.free();
      xObj1.free();
    }
  }
  xObjDict2.free();
  xObjDict1.free();

  // recursively scan any resource dictionaries in Patterns in this
  // resource dictionary
  resDict->lookupNF("Pattern", &patternDict1);
  if (checkFontObject(&patternDict1, &patternDict2) && patternDict2.isDict()) {
    for (i = 0; i < patternDict2.dictGetLength(); ++i) {
      patternDict2.dictGetValNF(i, &pattern1);
      if (checkFontObject(&pattern1, &pattern2) && pattern2.isStream()) {
        pattern2.streamGetDict()->lookupNF("Resources", &resObj);
        scanFonts(&resObj);
        resObj.free();
      }
      pattern2.free();
      pattern1.free();
    }
  }
  patternDict2.free();
  patternDict1.free();

  // recursively scan any resource dictionaries in ExtGStates in this
  // resource dictionary
  resDict->lookupNF("ExtGState", &gsDict1);
  if (checkFontObject(&gsDict1, &gsDict2) && gsDict2.isDict()) {
    for (i = 0; i < gsDict2.dictGetLength(); ++i) {
      gsDict2.dictGetValNF(i, &gs1);
      if (checkFontObject(&gs1, &gs2) && gs2.isDict()) {
        gs2.dictLookupNF("SMask", &smask1);
        if (checkFontObject(&smask1, &smask2) && smask2.isDict()) {
          smask2.dictLookupNF("G", &smaskGroup1);
          if (checkFontObject(&smaskGroup1, &smaskGroup2) &&
              smaskGroup2.isStream()) {
            smaskGroup2.streamGetDict()->lookupNF("Resources", &resObj);
            scanFonts(&resObj);
            resObj.free();
          }
          smaskGroup2.free();
          smaskGroup1.free();
        }
        smask2.free();
        smask1.free();
      }
      gs2.free();
      gs1.free();
    }
  }
  gsDict2.free();
  gsDict1.free();
}

std::map<std::string, std::set<std::string>> fontDict;

void PdfLoader::scanFont(GfxFont *font) {
  Ref fontRef;
  Object fontObj, toUnicodeObj;
  GString *name;
  int i;

  fontRef = *font->getID();

  // check for an already-seen font
  for (i = 0; i < fontsLen; ++i) {
    if (fontRef.num == fonts[i].num && fontRef.gen == fonts[i].gen) {
      return;
    }
  }

  // font name
  name = font->getName();

  // print the font info
  if (name) {
    char fontCode[1000], fontName[1000], fontType[1000];

    if (sscanf(name->getCString(), "%[^+]+%[^-]-%s", fontCode, fontName, fontType) != EOF) {
      if (fontDict.find(fontName) == fontDict.end()) {
        fontDict[fontName] = std::set<std::string>();
      }

      fontDict[fontName].insert(fontType);
    }
  }

  // add this font to the list
  if (fontsLen == fontsSize) {
    if (fontsSize <= INT_MAX - 32) {
      fontsSize += 32;
    } else {
      // let greallocn throw an exception
      fontsSize = -1;
    }
    fonts = (Ref *)greallocn(fonts, fontsSize, sizeof(Ref));
  }
  fonts[fontsLen++] = *font->getID();
}

GBool PdfLoader::checkFontObject(Object *in, Object *out) {
  int objNum;

  if (!in->isRef()) {
    in->copy(out);
    return gTrue;
  }
  objNum = in->getRefNum();
  if (objNum < 0 || objNum >= numObjects) {
    out->initNull();
    return gTrue;
  }
  if (seenObjs[objNum]) {
    out->initNull();
    return gFalse;
  }
  seenObjs[objNum] = (char)1;
  in->fetch(doc->getXRef(), out);
  return gTrue;
}

std::vector<std::string> PdfLoader::extractFonts() {
  int firstPage, lastPage;
  std::vector<std::string> fontInfo;

  Dict *resDict;
  Annots *annots;
  AcroForm *form;
  Object obj1, obj2;

  if (!doc->isOk()) {
    goto err;
  }

  firstPage = 1;
  lastPage = doc->getNumPages();

  fonts = NULL;
  fontsLen = fontsSize = 0;
  numObjects = doc->getXRef()->getNumObjects();
  seenObjs = (char *)gmalloc(numObjects);
  memset(seenObjs, 0, numObjects);

  for (int page = firstPage; page <= lastPage; page++) {
    Page *pdfPage = doc->getCatalog()->getPage(page);

    if ((resDict = pdfPage->getResourceDict())) {
      // fprintf(stderr, "scanFonts\n");
      scanFonts(resDict);
    }

    annots = new Annots(doc, pdfPage->getAnnots(&obj1));
    obj1.free();

    for (int i = 0; i < annots->getNumAnnots(); i++) {
      if (annots->getAnnot(i)->getAppearance(&obj1)->isStream()) {
        obj1.streamGetDict()->lookupNF("Resources", &obj2);
        scanFonts(&obj2);
        obj2.free();
      }
      obj1.free();
    }
    delete annots;
  }
  if ((form = doc->getCatalog()->getForm())) {
    for (int i = 0; i < form->getNumFields(); ++i) {
      form->getField(i)->getResources(&obj1);
      if (obj1.isArray()) {
        for (int j = 0; j < obj1.arrayGetLength(); ++j) {
          obj1.arrayGetNF(j, &obj2);
          scanFonts(&obj2);
          obj2.free();
        }
      } else if (obj1.isDict()) {
        scanFonts(obj1.getDict());
      }
      obj1.free();
    }
  }

  for (auto pair : fontDict) {
    fprintf(stderr, "%s has types:\n", pair.first.c_str());
    for (auto ft : pair.second) {
      fprintf(stderr, " - %s\n", ft.c_str());
    }
  }

  fprintf(stderr, "Found %d fonts\n", fontsSize);

  gfree(fonts);
  gfree(seenObjs);

err:
  Object::memCheck(stderr);
  gMemReport(stderr);

  return fontInfo;
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

Image PdfLoader::pageToImage(int pageNum, int dpi) {
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

  doc->displayPages(splashOut, pageNum, pageNum, dpi, dpi, 0, gFalse, gTrue, gFalse);
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

// #include <iostream>

// using namespace std;

// int main() {
//   LoaderConfig config;
//   map<unsigned int, NamedFontSpec> fontSpecs;

//   PdfLoader *l = new PdfLoader(config, "skf.pdf");
//   vector<string> pageText = l->extractText();
//   vector<string> fontMap = l->extractFontMap(fontSpecs);

//   // fprintf(stderr, "Read %lu pages text, %lu pages fontmap\n", pageText.size(), fontMap.size());

//   int diff = 0;

//   for (int i = 0; i < pageText.size(); i++) {
//     diff += fabs((int)pageText[i].length() - (int)fontMap[i].length());
//     if (pageText[i].length() != fontMap[i].length()) {
//       fprintf(stderr, "Page %d mismatch: %lu text, %lu font\n", i, pageText[i].length(), fontMap[i].length());
//     }
//   }

//   fprintf(stderr, "Total diff %d\n", diff);

//   // cerr << pageText[0] << endl;

//   // for (auto pair : fontSpecs) {
//   //   fprintf(stderr, "Font id %d had name '%s', type '%s', size %d\n", pair.first, pair.second.fontName.c_str(), pair.second.fontType.c_str(), pair.second.fontSize);
//   // }
// }
