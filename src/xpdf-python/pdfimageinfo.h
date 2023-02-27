#ifndef PDFTOSTRING_H
#define PDFTOSTRING_H


#include "gtypes.h"
#include "GlobalParams.h"
#include "TextOutputDev.h"

typedef struct PIIConfig {
  GBool verbose = gFalse;
  GBool quiet = gFalse;
} PIIConfig;

class PdfImageInfo {
public:
    PdfImageInfo(PIIConfig config);
    ~PdfImageInfo();
    void loadFile(const char *fileName);
};

#endif
