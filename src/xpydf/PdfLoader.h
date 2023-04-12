#ifndef PDFTOSTRING_H
#define PDFTOSTRING_H

#include <string>
#include <vector>

#include "gtypes.h"
#include "GlobalParams.h"
#include "PDFDoc.h"
#include "TextOutputDev.h"

#include "ImageInfoDev.h"

typedef struct LoaderConfig {
  GBool clipText = gFalse;
  GBool discardDiag = gTrue;
  GBool discardRotatedText = gTrue;
  GBool noPageBreaks = gFalse;
  GBool insertBOM = gFalse;
  GBool verbose = gFalse;
  GBool quiet = gTrue;
  unsigned int mode = 0;
} LoaderConfig;

typedef struct PageImageInfo {
  int pageNum;
  double width, height;
  std::vector<ImageInfo> images;
} PageImageInfo;

class PdfLoader {
public:
    PdfLoader(LoaderConfig config, char *fileName, char *userName = NULL, char *password = NULL);
    ~PdfLoader();
    std::vector<std::string> extractText();
    std::vector<PageImageInfo> extractImages();
    bool isOk();
private:
  TextOutputControl textOutControl;
  PDFDoc *doc;
  GString *textFileName;
};

#endif