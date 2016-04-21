#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowTitle("FKMB Media Player");
    ofSetBackgroundColor(ofColor::black);

    // Video playback
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
}

//--------------------------------------------------------------
void ofApp::draw(){
    float windowW = ofGetWindowWidth();
    float windowH = ofGetWindowHeight();
    float videoW = currentMovie.getWidth();
    float videoH = currentMovie.getHeight();

    //float targetW = min(windowW, videoW);

    // Fit to width, and center the video inside the window
    currentMovie.draw(0, 0, windowW, videoH*windowH/videoH);
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
