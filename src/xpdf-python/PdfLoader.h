#ifndef PDFTOSTRING_H
#define PDFTOSTRING_H

#include <string>
#include <vector>

#include "gtypes.h"
#include "GlobalParams.h"
#include "TextOutputDev.h"

typedef struct PTSConfig {
  GBool clipText = gFalse;
  GBool discardDiag = gTrue;
  GBool discardRotatedText = gTrue;
  GBool noPageBreaks = gFalse;
  GBool insertBOM = gFalse;
  GBool verbose = gFalse;
  GBool quiet = gFalse;
} PTSConfig;

class PdfLoader {
public:
    PdfLoader(PTSConfig config, const char *fileName);
    ~PdfLoader();
    std::vector<std::string> extractText();
private:
  TextOutputControl textOutControl;
  PDFDoc *doc;
  GString *textFileName;
};

#endif