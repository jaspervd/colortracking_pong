#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"

class ofApp : public ofBaseApp{
public:
    
    void setup();
    void update();
    void draw();
    void mouseReleased(int x, int y, int button);
    
    bool subtraction;
    bool dilate;
    bool erode;
    bool blur;
    
    float threshold;
    float range;
    ofVec2f ball;
    ofVec2f velocity;
    ofVec2f dir;
    
    class ofTrueTypeFont text;
    
    ofVec2f size;
    
    ofColor color;
    
    ofVideoGrabber grabber;
    
    int playerOne;
    int playerTwo;
    
    float playerOnePos;
    float playerTwoPos;
    
    ofxCvColorImage rgb;
    ofxCvGrayscaleImage background;
    ofxCvGrayscaleImage grayscale;
    ofxCvGrayscaleImage roi;
    ofxCvGrayscaleImage red;
    ofxCvGrayscaleImage green;
    ofxCvGrayscaleImage blue;
    ofxCvGrayscaleImage difference;
    
    ofxCvContourFinder contour;
    
    void updateBallPosition();
    void drawBall();
    
    void win();
    void drawType();
    
    void debugDraw();

};
