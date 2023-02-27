#ifndef PDF_IMAGE_INFO_H
#define PDF_IMAGE_INFO_H

#include <aconf.h>

#include "gtypes.h"
#include "OutputDev.h"
#include "ImageOutputDev.h"

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

#endif
