
#include "bkplaintext.h"

BKPlainText::BKPlainText() { }
BKPlainText::~BKPlainText() { }

BKPlainText* BKPlainText::create(string& file) {
	BKPlainText* r = new BKPlainText();
	r->fileName = file;
	r->runs = new BKRun[1];
	r->nRuns = 1;
	r->runs[0].text = "nieeeeeeeeee nieeeeeeeeeeeeeeeeeeeee nieeeeeeeeeeeeeeeeeeeeeeeeeeeee nieeeeeeeeeeeeeeeeeeeeeee";
	r->runs[0].n = strlen(r->runs[0].text);
	r->reflow(480);
	return r;
}

void BKPlainText::getFileName(string& fn) {
	fn = fileName;
}

void BKPlainText::getTitle(string& t) {
	t = "FIX PLAIN TEXT TITLES";
}

void BKPlainText::getType(string& t) {
	t = "Plain text";
}

