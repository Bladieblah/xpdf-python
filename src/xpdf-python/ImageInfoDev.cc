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

#include "ImageInfoDev.h"


void ImageInfoDev::drawImage(GfxState *state, Object *ref, Stream *str,
  int width, int height,
  GfxImageColorMap *colorMap,
  int *maskColors, GBool inlineImg,
  GBool interpolate
) {
  addImage(width, height, state);
}

void ImageInfoDev::drawImageMask(GfxState *state, Object *ref, Stream *str,
    int width, int height, GBool invert,
    GBool inlineImg, GBool interpolate
) {
  addImage(width, height, state);
}

void ImageInfoDev::addImage(int width, int height, GfxState *state) {
  double x0, y0, x1, y1;

  state->transformDelta(1, 0, &x0, &y0);
  state->transformDelta(0, 1, &x1, &y1);

  images.push_back((ImageInfo) {
      fmax(fabs(x0), fabs(x1)),
      fmax(fabs(y0), fabs(y1)),
  });
}

void ImageInfoDev::startPage(int pageNum, GfxState *state) {
  curPageNum = pageNum;
  images.clear();
}
