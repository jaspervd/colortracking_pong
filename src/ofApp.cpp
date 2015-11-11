#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetWindowTitle("Ultra Awesome Deluxe Pong XXL 3000");
    ofBackground(33);
    
    // Set params
    
    threshold = 30;
    range = 30;
    subtraction = false;
    dilate = false;
    erode = true;
    blur = true;
    
    // size
    size.x = 20; // width
    size.y = 100; // height
    
    text.loadFont("vag.ttf", 32);
    
    playerOnePos = (ofGetWidth()/4*3) / 2;
    playerTwoPos = (ofGetWidth()/4*3) / 2;

    // Initialize grabber and
    // allocate OpenCv images
    
    grabber.initGrabber(320, 240);
    
    rgb.allocate(grabber.getWidth(), grabber.getHeight());
    background.allocate(grabber.getWidth(), grabber.getHeight());
    grayscale.allocate(grabber.getWidth(), grabber.getHeight());
    roi.allocate(grabber.getWidth(), grabber.getHeight());
    red.allocate(grabber.getWidth(), grabber.getHeight());
    green.allocate(grabber.getWidth(), grabber.getHeight());
    blue.allocate(grabber.getWidth(), grabber.getHeight());
    difference.allocate(grabber.getWidth(), grabber.getHeight());
    
    ball.set(ofGetWidth()*0.5, 20);
    velocity.set(5, 5);
    dir.set(1, 1);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    // Grab the camera feed
    // and check for new frames
    
    grabber.update();
    if(grabber.isFrameNew()){
        
        // Get pixel data and
        // pre-process the image
        
        rgb.setFromPixels(grabber.getPixels(), grabber.getWidth(), grabber.getHeight());
        rgb.mirror(false, true);
        
        if(dilate) rgb.dilate();
        if(erode) rgb.erode();
        if(blur) rgb.blur(11);

        // Create a grayscale image
        // for each color channel
        
        rgb.convertToGrayscalePlanarImages(red, green, blue);

        // Convert to grayscale
        
        grayscale = rgb;

        // Perform background subtraction
        // We only track colors inside our ROI
        
        roi.absDiff(background, grayscale);
        roi.threshold(threshold);

        // Loop through all pixels
        // and calculate difference
        
        ofColor min = ofColor(color - range);
        ofColor max = ofColor(color + range);
        
        for(int i=0; i<grabber.getWidth()*grabber.getHeight(); i++){
            
            bool hasR = ofInRange(red.getPixels()[i], min.r, max.r);
            bool hasG = ofInRange(green.getPixels()[i], min.g, max.g);
            bool hasB = ofInRange(blue.getPixels()[i], min.b, max.b);
            
            int result = hasR && hasG && hasB ? 255 : 0;
            
            // If the result is not inside
            // our ROI, color the pixel black
            
            if(subtraction) result = roi.getPixels()[i] == 0 ? 0 : result;
            
            difference.getPixels()[i] = result;
            
        }
        
        difference.flagImageChanged();

        // Find blobs in the difference image
        // input, min_size, max_size, how many?, find holes?
        
        contour.findContours(difference, 250, grabber.getWidth()*grabber.getHeight(), 2, false);
        
    }
    
    if(contour.nBlobs == 2) {
        updateBallPosition();
    }

}

void ofApp::updateBallPosition(){
    // Add velocity and acceleration
    
    ball += velocity * dir;
    
    // Set boundaries
    
    if(ball.y > ofGetHeight() - 10 || ball.y < 10) dir.y *= -1;
    
    if(ball.x == (size.y / 2 + size.x / 2 + 10)) {
        if(ball.y > (playerOnePos - size.y / 2) && ball.y < (playerOnePos + size.y / 2)) {
            dir.x *= -1;
        }
    }
    
    if(ball.x == (ofGetWidth() - (size.y / 2 + size.x / 2 + 10))) {
        if(ball.y > (playerTwoPos - size.y / 2) && ball.y < (playerTwoPos + size.y / 2)) {
            dir.x *= -1;
        }
    }
    
    // Detect win
    
    if(ball.x > ofGetWidth() - 10) {
        playerOne ++;
        win();
    }
    
    if(ball.x < 10) {
        playerTwo ++;
        win();
    }
    
}

void ofApp::draw(){
    
    // Map aspect ratio of the
    // video feed to the screen
    
    int width = ofGetWidth();
    int height = ofGetWidth()/4*3;
    
    ofPushMatrix();
    ofPushStyle();
    
    ofSetColor(255, 255, 255, 100);
    grabber.draw(width, 0, -width, height);
    ofSetColor(255);
    
    // Check which player is left and right and make them player one or two
    
    for(int i = 0; i < contour.nBlobs; i++) {
        int yPos = ofMap(contour.blobs[i].centroid.y, 0, grabber.getHeight(), 0, height - size.y);
        if(contour.blobs[i].centroid.x < (width / 8)) {
            playerOnePos = yPos;
        } else {
            playerTwoPos = yPos;
        }
    }
    
    if(contour.nBlobs == 0) {
        ofDrawBitmapString("Choose a (distinct/neon) color as your controller to start the game.", 30, ofGetHeight() - 50);
    } else if(contour.nBlobs == 1) {
        ofDrawBitmapString("The game can only start or resume when there are two controllers.", 30, ofGetHeight() - 50);
    } else {
        drawBall();
    }
    
    // Draw bars
    
    ofSetColor(255);
    ofRect(size.y / 2, playerOnePos - size.y / 2, size.x, size.y);
    ofRect(width - size.x - size.y / 2, playerTwoPos - size.y / 2, size.x, size.y);
    
    ofPopStyle();
    ofPopMatrix();
    
    debugDraw();
    
    drawType();
}
       
void ofApp::drawType(){
    ofSetColor(255, 255, 255);
    text.drawString("ULTRA AWESOME DELUXE PONG XXL 3000", ofGetWidth()/2 - 400,100);
    text.drawString("--------------------------------------------------", ofGetWidth()/2 - 400,130);
    text.drawString(ofToString(playerOne) + " : " + ofToString(playerTwo), ofGetWidth()/2 - 50,170);
}

void ofApp::win(){
    ball.set(ofGetWidth()*0.5, 20);
    dir.x *= -1;
}

void ofApp::drawBall(){
    ofPushMatrix();
    ofSetCircleResolution(72);
    ofSetColor(255, 255, 0);
    ofCircle(ball.x, ball.y, 10);
    ofPopMatrix();
    
}

void ofApp::debugDraw(){
    ofPushMatrix();
    ofTranslate(20, 30);
    ofDrawBitmapString("FPS: " + ofToString(ofGetFrameRate()), 0, 0);
    
    ofPushStyle();
    ofSetColor(color);
    ofRect(180, 160, 160, 120);
    ofPopStyle();
    
    difference.draw(180, 300, 160, 120);

    ofPopMatrix();
    
}

void ofApp::mouseReleased(int x, int y, int button){
    // Map mouse positions to match
    // the rgb-image from the camera
    
    x = ofMap(float(x), 0, ofGetWidth(), 0, grabber.getWidth());
    y = ofMap(float(y), 0, ofGetWidth()/4*3, 0, grabber.getHeight());
    
    // Bail out if the mapping
    // falls outsode the image
    
    if(x>grabber.getWidth()) return;
    if(y>grabber.getHeight()) return;

    // Get color values
    // and cache them
    
    int r = red.getPixels()[y*grabber.getWidth() + x];
    int g = green.getPixels()[y*grabber.getWidth() + x];
    int b = blue.getPixels()[y*grabber.getWidth() + x];
    
    color = ofColor(r, g, b);
    
}
