#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFullscreen(true);
    ofSetLogLevel(ofLogLevel::OF_LOG_NOTICE);
    ofSetFrameRate(60);
    ofSetWindowTitle("FKMB Media Player");
    ofSetBackgroundColor(ofColor::black);
    ofHideCursor(); // not working in Linux

    //ofRemoveListener(ofEvents().mouseMoved, this, &ofApp::mouseMoved);
    //ofUnregisterMouseEvents(this);

    // Video playback
    playbackMode = PBM_NORMAL;
    currentMovieIndex = 0;
    ofSetVerticalSync(true);
    allMovies.push_back(ofFile("movies/fingers.mov"));
    allMovies.push_back(ofFile("movies/big_buck_bunny.ogv"));
    std::string firstMovieFileName = allMovies[currentMovieIndex].getAbsolutePath();
    currentMovie.load(allMovies[currentMovieIndex].getAbsolutePath());
    currentMovie.setLoopState(OF_LOOP_NORMAL);
    currentMovie.play();
    ofLog(ofLogLevel::OF_LOG_NOTICE, getVideoDetails(currentMovie));
}

//--------------------------------------------------------------
void ofApp::update(){
    currentMovie.update();
    ofLog(ofLogLevel::OF_LOG_NOTICE, "Frame rate = " + ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void ofApp::draw(){
    float windowW = ofGetWindowWidth();
    float windowH = ofGetWindowHeight();
    float videoW = currentMovie.getWidth();
    float videoH = currentMovie.getHeight();

    bool landscapeVideo = videoW > videoH;
    bool fitToWidth = landscapeVideo;

    float targetW, targetH = 0.0f;
    if (fitToWidth)
    {
        targetW = windowW;
        targetH = videoH*windowH/videoH;
    }
    else
    {
        targetH = windowH;
        targetW = videoW*windowH/videoW;
    }

    if (playbackMode == PBM_NORMAL)
    {
        // Fit to width, and center the video inside the app window
        currentMovie.draw(0, 0, targetW, targetH);
    }
    else if (playbackMode == PBM_DOTS)
    {
        // replace by circles
        ofSetColor(ofColor::white);
        ofPixels& pixels = currentMovie.getPixels();

        int vidWidth = pixels.getWidth();
        int vidHeight = pixels.getHeight();
        int nChannels = pixels.getNumChannels();

        float horizontalScaling = targetW / videoW;
        float verticalScaling = targetH / videoH;

        // let's move through the "RGB(A)" char array
        // using the red pixel to control the size of a circle.
        int byteOfInterest = 3;
        for (int i = byteOfInterest; i < vidWidth; i+=8){
            for (int j = byteOfInterest; j < vidHeight; j+=8){
                unsigned char r = pixels[(j * currentMovie.getWidth() + i)*nChannels];
                float val = 1.0 - ((float)r / 255.0f);
                ofDrawCircle(i * horizontalScaling, j * verticalScaling, 10*val);
            }
        }
        ofSetColor(ofColor::white);
        //ofDrawBitmapString("frame: " + ofToString(currentMovie.getCurrentFrame()) + "/"+ofToString(currentMovie.getTotalNumFrames()),20,380);
        //ofDrawBitmapString("duration: " + ofToString(currentMovie.getPosition()*currentMovie.getDuration(),2) + "/"+ofToString(currentMovie.getDuration(),2),20,400);
        //ofDrawBitmapString("speed: " + ofToString(currentMovie.getSpeed(),2),20,420);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == ' ') {
        currentMovie.stop();
        currentMovieIndex = (currentMovieIndex + 1) % allMovies.size();
        currentMovie.load(allMovies[currentMovieIndex].getAbsolutePath());
        currentMovie.play();
        ofLog(ofLogLevel::OF_LOG_NOTICE, getVideoDetails(currentMovie));
    }
    else if (key == 'a')
    {
        playbackMode = (playbackMode - 1)% PBM_COUNT;
    }
    else if (key == 'd')
    {
        playbackMode = (playbackMode + 1)% PBM_COUNT;
    }
}

std::string ofApp::getVideoDetails(ofVideoPlayer& video)
{
    std::string result = "Current video is now '" + allMovies[currentMovieIndex].getFileName() + "', W=" + ofToString(currentMovie.getWidth()) + ", H=" + ofToString(currentMovie.getHeight());
    return result;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
