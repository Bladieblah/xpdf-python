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
  GfxColorSpaceMode csMode;
  ImageStream *imgStr;
  Guchar *p;
  GfxRGB rgb;
  GfxGray gray;
  Image image;
  int x, y;
  int n = 0;

  csMode = colorMap->getColorSpace()->getMode();
  if (csMode == csIndexed) {
    csMode = ((GfxIndexedColorSpace *)colorMap->getColorSpace())
             ->getBase()->getMode();
  }

  // initialize stream
  imgStr = new ImageStream(str, width, colorMap->getNumPixelComps(), colorMap->getBits());
  imgStr->reset();
  imgNum++;

  if (colorMap->getNumPixelComps() == 1 && (csMode == csDeviceGray || csMode == csCalGray)) {
    image = {
      static_cast<unsigned int>(IMAGE_TYPES::P5),
      static_cast<unsigned int>(width),
      static_cast<unsigned int>(height),
      static_cast<unsigned int>(width * height),
      nullptr
    };
    image.data = (unsigned char *)malloc(image.size * sizeof(unsigned char));

    for (y = 0; y < height; ++y) {
      if ((p = imgStr->getLine())) {
        for (x = 0; x < width; ++x) {
          colorMap->getGray(p, &gray, state->getRenderingIntent());
          image.data[n++] = colToByte(gray);
          ++p;
        }
      } else {
        for (x = 0; x < width; ++x) {
          image.data[n++] = 0;
        }
      }
    }
  } else {
    image = {
      static_cast<unsigned int>(IMAGE_TYPES::P6),
      static_cast<unsigned int>(width),
      static_cast<unsigned int>(height),
      static_cast<unsigned int>(3 * width * height),
      nullptr
    };
    image.data = (unsigned char *)malloc(image.size * sizeof(unsigned char));

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
  }


  imgStr->close();
  delete imgStr;
  
  images->push_back(image);
}

void ImageDataDev::drawImageMask(GfxState *state, Object *ref, Stream *str,
    int width, int height, GBool invert,
    GBool inlineImg, GBool interpolate
) {
  unsigned int size, bytesRead, pixelsRead;
  int rowCount = 0;
  char buf;

  Image image = {
    static_cast<unsigned int>(IMAGE_TYPES::P4),
    static_cast<unsigned int>(width),
    static_cast<unsigned int>(height),
    static_cast<unsigned int>(width * height),
    nullptr
  };
  
  size = height * ((width + 7) / 8);
  image.data = (unsigned char *)malloc(image.size * sizeof(unsigned char));
  imgNum++;

  str->reset();
  pixelsRead = 0;
  while (size > 0) {
    bytesRead = str->getBlock(&buf, 1);

    if (bytesRead == 0) {
      break;
    }

    for (unsigned int i = 0; i < sizeof(char) * 8; i++) {
      image.data[pixelsRead++] = buf & (128 >> i) ? 0xff : 0;
      rowCount++;

      if (rowCount == width || pixelsRead >= image.size) {
        rowCount = 0;
        break;
      }
    }
    
    size -= bytesRead;
  }

  str->close();
  
  images->push_back(image);
}

void ImageDataDev::startPage(int pageNum, GfxState *state) {
  curPageNum = pageNum;
  images->clear();
}
