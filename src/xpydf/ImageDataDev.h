#ifndef PDF_IMAGE_DATA_H
#define PDF_IMAGE_DATA_H

#include <aconf.h>

#include <vector>

#include "gtypes.h"
#include "OutputDev.h"
#include "ImageOutputDev.h"


enum IMAGE_TYPES {P1 = 1, P2, P3, P4, P5, P6};

typedef struct Image {
  unsigned int image_type;
  unsigned int width, height, size;
  unsigned char *data;
} Image;

class ImageDataDev: public ImageOutputDev {
public:
  ImageDataDev(char *fileRootA, GBool dumpJPEGA, GBool dumpRawA,
    GBool listA) : ImageOutputDev(fileRootA, dumpJPEGA, dumpRawA, listA) {
      imgNum = 0;
    };

  virtual void startPage(int pageNum, GfxState *state);
  
  virtual void drawImage(GfxState *state, Object *ref, Stream *str,
    int width, int height, GfxImageColorMap *colorMap,
    int *maskColors, GBool inlineImg, GBool interpolate);
  virtual void drawImageMask(GfxState *state, Object *ref, Stream *str,
    int width, int height, GBool invert,
    GBool inlineImg, GBool interpolate);
    
  void addImage(int width, int height, GfxState *state);

  int curPageNum;
  std::vector<Image> images;
  int imgNum;			// current image number
};

#endif
