#pragma once

#include "ofMain.h"
#include "ofxFontStash.h"

class ofApp : public ofBaseApp{
	ofxFontStash fontNormal;
	ofxFontStash fontItalic;
	ofxFontStash fontBold;
	ofxFontStash fontJapanese;
    
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int);
};

