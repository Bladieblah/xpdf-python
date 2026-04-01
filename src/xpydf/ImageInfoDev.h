#ifndef PDF_IMAGE_INFO_H
#define PDF_IMAGE_INFO_H

#include <aconf.h>

#include <vector>

#include "gtypes.h"
#include "OutputDev.h"
#include "ImageOutputDev.h"


typedef struct ImageInfo {
  double width, height;
} ImageInfo;

class ImageInfoDev: public ImageOutputDev {
public:
  ImageInfoDev(
    char *fileRootA,
    GBool dumpJPEGA,
    GBool dumpJPXA,
		GBool dumpRawA,
    GBool uniqueA,
    GBool listA,
    GBool listOnlyA
  ) : ImageOutputDev(fileRootA, dumpJPEGA, dumpJPXA, dumpRawA, uniqueA, listA, listOnlyA) {};

  virtual void startPage(int pageNum, GfxState *state);
  
  virtual void drawImage(GfxState *state, Object *ref, Stream *str,
    int width, int height, GfxImageColorMap *colorMap,
    int *maskColors, GBool inlineImg, GBool interpolate);
  virtual void drawImageMask(GfxState *state, Object *ref, Stream *str,
    int width, int height, GBool invert,
    GBool inlineImg, GBool interpolate);
    
  void addImage(int width, int height, GfxState *state);

  int curPageNum;
  std::vector<ImageInfo> images;
};

#endif
