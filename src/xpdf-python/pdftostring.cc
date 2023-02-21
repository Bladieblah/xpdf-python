//========================================================================
//
// pdftotext.cc
//
// Copyright 1997-2013 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <sstream>

#include "gmem.h"
#include "gmempp.h"
#include "parseargs.h"
#include "GString.h"
#include "GList.h"
#include "GlobalParams.h"
#include "Object.h"
#include "Stream.h"
#include "Array.h"
#include "Dict.h"
#include "XRef.h"
#include "Catalog.h"
#include "Page.h"
#include "PDFDoc.h"
#include "TextOutputDev.h"
#include "CharTypes.h"
#include "UnicodeMap.h"
#include "TextString.h"
#include "Error.h"
#include "config.h"

static int firstPage = 1;
static int lastPage = 0;
static double fixedPitch = 0;
static GBool clipText = gFalse;
static GBool discardDiag = gTrue;
static GBool discardRotatedText = gTrue;
static char textEncName[128] = "";
static char textEOL[16] = "";
static GBool noPageBreaks = gFalse;
static GBool insertBOM = gFalse;
static double marginLeft = 0;
static double marginRight = 0;
static double marginTop = 0;
static double marginBottom = 0;
static char ownerPassword[33] = "\001";
static char userPassword[33] = "\001";
static GBool verbose = gFalse;
static GBool quiet = gFalse;
static char cfgFileName[256] = "";


static void outputToStringStream(void *stream, const char *text, int len) {
  ((std::stringstream *)stream)->write(text, len);
}

std::string fileToString(char *fileName) {
  PDFDoc *doc;
  GString *textFileName;
  GString *ownerPW, *userPW;
  TextOutputControl textOutControl;
  TextOutputDev *textOut;
  UnicodeMap *uMap;
  int exitCode;
  std::stringstream *stream;

  exitCode = 99;

  // read config file
  globalParams = new GlobalParams(cfgFileName);
  if (textEncName[0]) {
    globalParams->setTextEncoding(textEncName);
  }

  if (textEOL[0]) {
    if (!globalParams->setTextEOL(textEOL)) {
      fprintf(stderr, "Bad '-eol' value on command line\n");
    }
  }

  if (noPageBreaks) {
    globalParams->setTextPageBreaks(gFalse);
  }

  if (verbose) {
    globalParams->setPrintStatusInfo(verbose);
  }

  if (quiet) {
    globalParams->setErrQuiet(quiet);
  }

  // get mapping to output encoding
  if (!(uMap = globalParams->getTextEncoding())) {
    error(errConfig, -1, "Couldn't get text encoding");
    goto err1;
  }

  // open PDF file
  if (ownerPassword[0] != '\001') {
    ownerPW = new GString(ownerPassword);
  } else {
    ownerPW = NULL;
  }
  if (userPassword[0] != '\001') {
    userPW = new GString(userPassword);
  } else {
    userPW = NULL;
  }
  doc = new PDFDoc(fileName, ownerPW, userPW);
  if (userPW) {
    delete userPW;
  }
  if (ownerPW) {
    delete ownerPW;
  }
  if (!doc->isOk()) {
    exitCode = 1;
    goto err2;
  }

//   // check for copy permission
//   if (!doc->okToCopy()) {
//     error(errNotAllowed, -1,
// 	  "Copying of text from this document is not allowed.");
//     exitCode = 3;
//     goto err2;
//   }

  // construct text file name
  textFileName = new GString(fileName);

  // get page range
  if (firstPage < 1) {
    firstPage = 1;
  }
  if (lastPage < 1 || lastPage > doc->getNumPages()) {
    lastPage = doc->getNumPages();
  }

  textOutControl.mode = textOutTableLayout;
  textOutControl.fixedPitch = fixedPitch;

  textOutControl.clipText = clipText;
  textOutControl.discardDiagonalText = discardDiag;
  textOutControl.insertBOM = insertBOM;
  textOutControl.marginLeft = marginLeft;
  textOutControl.marginRight = marginRight;
  textOutControl.marginTop = marginTop;
  textOutControl.marginBottom = marginBottom;
  
  stream = new std::stringstream();
  textOut = new TextOutputDev(&outputToStringStream, stream, &textOutControl);

  if (textOut->isOk()) {
    doc->displayPages(textOut, firstPage, lastPage, 72, 72, 0,
		      gFalse, gTrue, gFalse);
  } else {
    delete textOut;
    exitCode = 2;
    goto err3;
  }
  delete textOut;

  exitCode = 0;

  // clean up
 err3:
  delete textFileName;
 err2:
  delete doc;
  uMap->decRefCnt();
 err1:
  delete globalParams;

  // check for memory leaks
  Object::memCheck(stderr);
  gMemReport(stderr);

  if (exitCode) {
    return std::string("");
  }

  return stream->str();
}

int main(int argc, char **argv) {
  firstPage = 1;
  lastPage = 1;

  if (argc == 2) {
    std::string result = fileToString(argv[1]);
    fprintf(stderr, "%s", result.c_str());
  }

  return 0;
}
