#ifndef PDFTOSTRING_H
#define PDFTOSTRING_H

#include <vector>

#include "gtypes.h"
#include "GlobalParams.h"
#include "TextOutputDev.h"
#include "ImageInfoDev.h"

typedef struct PIIConfig {
  GBool verbose = gFalse;
  GBool quiet = gFalse;
} PIIConfig;

typedef struct PageImageInfo {
  int pageNum;
  double width, height;
  std::vector<ImageInfo> images;
} PageImageInfo;

class PdfImageInfo {
public:
    PdfImageInfo(PIIConfig config);
    ~PdfImageInfo();
    std::vector<PageImageInfo> loadFile(const char *fileName);
};

#endif
