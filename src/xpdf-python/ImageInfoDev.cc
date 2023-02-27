#include <aconf.h>

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
  printInfo(width, height, state, colorMap);
}

void ImageInfoDev::printInfo(
  int width, int height, GfxState *state,
  GfxImageColorMap *colorMap
) {
  double x0, y0, x1, y1;

  state->transformDelta(1, 0, &x0, &y0);
  state->transformDelta(0, 1, &x1, &y1);

  fprintf(stderr, "page=%d width=%d height=%d loc=[(%f, %f), (%f, %f)]\n",
	 curPageNum, width, height, x0, y0, x1, y1);
}

void ImageInfoDev::startPage(int pageNum, GfxState *state) {
  curPageNum = pageNum;
}
