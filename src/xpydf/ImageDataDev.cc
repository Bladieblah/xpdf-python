#include <aconf.h>

#include <math.h>
#include <sstream>
#include <stddef.h>
#include <stdlib.h>

#include "GlobalParams.h"
#include "gmem.h"
#include "gmempp.h"
#include "config.h"
#include "Error.h"
#include "GfxState.h"
#include "Object.h"
#include "PDFDoc.h"
#include "Stream.h"

#include "ImageDataDev.h"


void ImageDataDev::drawImage(GfxState *state, Object *ref, Stream *str,
  int width, int height,
  GfxImageColorMap *colorMap,
  int *maskColors, GBool inlineImg,
  GBool interpolate
) {
  ImageStream *imgStr;
  Guchar *p;
  GfxRGB rgb;
  int x, y;
  int n = 0;
  
  Image image = {
    static_cast<unsigned int>(IMAGE_TYPES::P6),
    static_cast<unsigned int>(width),
    static_cast<unsigned int>(height),
    static_cast<unsigned int>(3 * width * height),
    nullptr
  };
  image.data = (unsigned char *)malloc(image.size * sizeof(unsigned char));
  imgNum++;

  // initialize stream
  imgStr = new ImageStream(str, width, colorMap->getNumPixelComps(), colorMap->getBits());
  imgStr->reset();

  for (y = 0; y < height; ++y) {
    if ((p = imgStr->getLine())) {
      for (x = 0; x < width; ++x) {
        colorMap->getRGB(p, &rgb, state->getRenderingIntent());
        image.data[n++] = colToByte(rgb.r);
        image.data[n++] = colToByte(rgb.g);
        image.data[n++] = colToByte(rgb.b);
        p += colorMap->getNumPixelComps();
      }
    } else {
      for (x = 0; x < width; ++x) {
        image.data[n++] = 0;
        image.data[n++] = 0;
        image.data[n++] = 0;
      }
    }
  }

  imgStr->close();
  delete imgStr;
  
  images->push_back(image);
}

void ImageDataDev::drawImageMask(GfxState *state, Object *ref, Stream *str,
    int width, int height, GBool invert,
    GBool inlineImg, GBool interpolate
) {
  char buf[4096];
  int size, n, i, m;
  Image image = {
    static_cast<unsigned int>(IMAGE_TYPES::P4),
    static_cast<unsigned int>(width),
    static_cast<unsigned int>(height),
    static_cast<unsigned int>(height * ((width + 7) / 8)),
    nullptr
  };
  
  size = image.size;
  image.data = (unsigned char *)malloc(image.size * sizeof(unsigned char));
  imgNum++;

  str->reset();
  m = 0;
  while (size > 0) {
    i = size < (int)sizeof(buf) ? size : (int)sizeof(buf);
    n = str->getBlock(((char *)image.data) + m, i);
    
    if (n < i) {
      break;
    }
    
    size -= n;
    m += n;
  }

  str->close();
  
  images->push_back(image);
}

void ImageDataDev::startPage(int pageNum, GfxState *state) {
  curPageNum = pageNum;
  images->clear();
}
