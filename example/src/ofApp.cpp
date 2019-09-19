#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofDisableArbTex();
	ofEnableAlphaBlending();
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(22, 22, 22, 255);

	fontNormal.load("Vera.ttf", 24);
	fontItalic.load("VeraMono-Bold.ttf", 48);
	fontBold.load("VeraMono-Bold.ttf", 48);
	fontJapanese.load("Arial Unicode.ttf", 48);
}

void ofApp::update() {

}

//--------------------------------------------------------------
void ofApp::draw() {

	ofPushStyle();
	
	float dx = 10, dy = 10;
	{
		dy += fontNormal.getLineHeight();
		ofSetColor(ofColor::white);
		fontNormal.drawString("The quick ", dx, dy);
	}

	{
		ofPushStyle();
		string str1 = "Ég get etið gler án þess að meiða mig.";
		ofSetColor(ofColor::white);
		dy += fontJapanese.getLineHeight();
		fontJapanese.drawString(str1, dx, dy);

		vector<ofRectangle> rect = fontJapanese.getStringSymbolsBoundingBoxes(str1, dx, dy, true);
		ofSetColor(ofColor::red);
		ofNoFill();
		for (size_t i = 0; i < rect.size(); i++) {
			ofDrawRectangle(rect[i]);
		}
		ofPopStyle();
	}

	{
		ofPushStyle();
		dy += fontJapanese.getLineHeight();
		string str2 = "私はガラスを食べられます。それは私を傷つけません。";
		ofSetColor(ofColor::red);
		fontJapanese.drawString(str2, dx, dy);

		vector<ofRectangle> rect = fontJapanese.getStringSymbolsBoundingBoxes(str2, dx, dy, false);
		ofSetColor(ofColor::white); 
		ofNoFill();
		for (size_t i = 0; i < rect.size(); i++) {
			ofDrawRectangle(rect[i]);
		}
		ofPopStyle();
	}

	ofPopStyle();
}

void ofApp::keyPressed(int k) {
}
