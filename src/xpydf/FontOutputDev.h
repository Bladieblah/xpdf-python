#ifndef FONT_OUTPUT_DEV_H
#define FONT_OUTPUT_DEV_H

#include "TextOutputDev.h"

typedef TextChar* (*CharConstructor)(Unicode cA, int charPosA, int charLenA,
  double xMinA, double yMinA, double xMaxA, double yMaxA,
  int rotA, GBool rotatedA, GBool clippedA, GBool invisibleA,
  TextFontInfo *fontA, double fontSizeA,
  double colorRA, double colorGA, double colorBA);

class TextPageFont: public TextPage {
public:
  TextPageFont(TextOutputControl *controlA) : TextPage(controlA) {};
  
  void TextPageFont::addChar(GfxState *state, double x, double y,
    double dx, double dy,
    CharCode c, int nBytes, Unicode *u, int uLen);

  TextChar *TextCharType(Unicode cA, int charPosA, int charLenA,
    double xMinA, double yMinA, double xMaxA, double yMaxA,
    int rotA, GBool rotatedA, GBool clippedA, GBool invisibleA,
    TextFontInfo *fontA, double fontSizeA,
    double colorRA, double colorGA, double colorBA) {
    return new TextChar(cA, charPosA, charLenA, xMinA, yMinA, xMaxA, yMaxA,
      rotA, rotatedA, clippedA, invisibleA, fontA, fontSizeA,
      colorRA, colorGA, colorBA);
  }
};

class FontOutputDev: public TextOutputDev {
public:
  FontOutputDev(TextOutputFunc func, void *stream,
    TextOutputControl *controlA) : TextOutputDev(func, stream, controlA) {
    delete text;
    text = new TextPageFont(&control);
  }
};

#endif
