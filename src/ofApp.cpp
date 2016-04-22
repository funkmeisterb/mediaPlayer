#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
#ifndef TARGET_LINUX
    // massive video slowdown in Linux for some reason
    ofSetFullscreen(true);
#endif
#ifndef DEBUG
    ofHideCursor(); // not working in Linux
#endif
    ofSetLogLevel(ofLogLevel::OF_LOG_NOTICE);
    ofSetFrameRate(60);
    ofSetWindowTitle("FKMB Media Player");
    ofSetBackgroundColor(ofColor::black);

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

    // Shader 1
#ifdef TARGET_OPENGLES
    shader1.load("shadersES2/shader");
#else
    if(ofIsGLProgrammableRenderer()){
        shader1.load("shadersGL3/shader");
    }else{
        shader1.load("shadersGL2/shader");
    }
#endif
    int planeWidth = ofGetWidth();
    int planeHeight = ofGetHeight();
    int planeGridSize = 20;
    int planeColums = planeWidth / planeGridSize;
    int planeRows = planeHeight / planeGridSize;
    plane.set(planeWidth, planeHeight, planeColums, planeRows, OF_PRIMITIVE_TRIANGLES);
    
    // Shader 2
#ifdef TARGET_OPENGLES
    shaderBlurX.load("shadersES2/shaderBlurX");
    shaderBlurY.load("shadersES2/shaderBlurY");
#else
    if(ofIsGLProgrammableRenderer()){
        shaderBlurX.load("shadersGL3/shaderBlurX");
        shaderBlurY.load("shadersGL3/shaderBlurY");
    }else{
        shaderBlurX.load("shadersGL2/shaderBlurX");
        shaderBlurY.load("shadersGL2/shaderBlurY");
    }
#endif
}

//--------------------------------------------------------------
void ofApp::update(){
    currentMovie.update();
    ofLog(ofLogLevel::OF_LOG_NOTICE, "Frame rate = " + ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(ofColor::white);
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
    fboBlurOnePass.allocate(targetW, targetH);
    fboBlurTwoPass.allocate(targetW, targetH);

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
    else if (playbackMode == PBM_SHADER1)
    {
        // Fit to width, and center the video inside the app window
        currentMovie.draw(0, 0, targetW, targetH);
        
        shader1.begin();

        // center screen.
        float cx = ofGetWidth() / 2.0;
        float cy = ofGetHeight() / 2.0;

        // the plane is being position in the middle of the screen,
        // so we have to apply the same offset to the mouse coordinates before passing into the shader.
        float mx = mouseX - cx;
        float my = mouseY - cy;

        // we can pass in a single value into the shader by using the setUniform1 function.
        // if you want to pass in a float value, use setUniform1f.
        // if you want to pass in a integer value, use setUniform1i.
        shader1.setUniform1f("mouseRange", 150);

        // we can pass in two values into the shader at the same time by using the setUniform2 function.
        // inside the shader these two values are set inside a vec2 object.
        shader1.setUniform2f("mousePos", mx, my);

        // color changes from magenta to blue when moving the mouse from left to right.
        float percentX = mouseX / (float)ofGetWidth();
        percentX = ofClamp(percentX, 0, 1);
        ofFloatColor colorLeft = ofColor::magenta;
        ofFloatColor colorRight = ofColor::blue;
        ofFloatColor colorMix = colorLeft.getLerped(colorRight, percentX);

        // create a float array with the color values.
        float mouseColor[4] = {colorMix.r, colorMix.g, colorMix.b, colorMix.a};

        // we can pass in four values into the shader at the same time as a float array.
        // we do this by passing a pointer reference to the first element in the array.
        // inside the shader these four values are set inside a vec4 object.
        shader1.setUniform4fv("mouseColor", &mouseColor[0]);

        ofTranslate(cx, cy);
        
        plane.drawWireframe();

        shader1.end();
    }
    else if (playbackMode == PBM_SHADER2)
    {
        float blur = ofMap(mouseX, 0, ofGetWidth(), 0, 10, true);
        
        //----------------------------------------------------------
        fboBlurOnePass.begin();
        
        shaderBlurX.begin();
        shaderBlurX.setUniform1f("blurAmnt", blur);
        
        // Fit to width, and center the video inside the app window
        currentMovie.draw(0, 0, targetW, targetH);
        
        shaderBlurX.end();
        
        fboBlurOnePass.end();
        
        //----------------------------------------------------------
        fboBlurTwoPass.begin();
        
        shaderBlurY.begin();
        shaderBlurY.setUniform1f("blurAmnt", blur);
        
        fboBlurOnePass.draw(0, 0);
        
        shaderBlurY.end();
        
        fboBlurTwoPass.end();
        
        //----------------------------------------------------------
        ofSetColor(ofColor::white);
        fboBlurTwoPass.draw(0, 0);
    }
#ifdef DEBUG
    ofSetColor(ofColor::aquamarine);
    ofDrawBitmapString("Playback mode: " + ofToString(playbackMode), 20, 20);
#endif
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
