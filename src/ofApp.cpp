#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
#ifndef TARGET_LINUX
    // massive video slowdown in Linux for some reason
    //ofSetFullscreen(false);
#endif
#ifndef DEBUG
    //ofHideCursor(); // not working in Linux
#endif
    ofSetLogLevel(ofLogLevel::OF_LOG_NOTICE);
    ofSetFrameRate(60);
    ofSetWindowTitle("FKMB Media Player");
    ofSetBackgroundColor(ofColor::black);

    // Video playback
    playbackMode = PBM_SHADER_BRU;
    currentMovieIndex = 0;
    ofSetVerticalSync(true);
    allMovies.push_back(ofFile("movies/fingers.mov"));
    allMovies.push_back(ofFile("movies/big_buck_bunny.ogv"));
    std::string firstMovieFileName = allMovies[currentMovieIndex].getAbsolutePath();
    currentMovie.load(allMovies[currentMovieIndex].getAbsolutePath());
    currentMovie.setLoopState(OF_LOOP_NORMAL);
    currentMovie.play();
    videoChanged();

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
    if(ofIsGLProgrammableRenderer())
	{
        shaderBlurX.load("shadersGL3/shaderBlurX");
        shaderBlurY.load("shadersGL3/shaderBlurY");
    }
	else
	{
        shaderBlurX.load("shadersGL2/shaderBlurX");
        shaderBlurY.load("shadersGL2/shaderBlurY");
    }
#endif
    image.load("images/bikers.jpg");

    // Shader 3
    shaderBru.load("shadersGL3/shaderBru");
}

//--------------------------------------------------------------
void ofApp::update(){
    currentMovie.update();
    //ofLog(ofLogLevel::OF_LOG_NOTICE, "Frame rate = " + ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(ofColor::white);
    if (playbackMode == PBM_NORMAL)
    {
        // Fit to width, and center the video inside the app window
        currentMovie.draw(0, 0, targetW, targetH);
    }
    else if (playbackMode == PBM_DOTS)
    {
        // replace by circles
        ofPixels& pixels = currentMovie.getPixels();

        int vidWidth = pixels.getWidth();
        int vidHeight = pixels.getHeight();
        int nChannels = pixels.getNumChannels();

        float horizontalScaling = targetW / currentMovie.getWidth();
        float verticalScaling = targetH / currentMovie.getHeight();

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
    }
    else if (playbackMode == PBM_SHADER_SPHERE)
    {
        // Fit to width, and center the video inside the app window
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
    else if (playbackMode == PBM_SHADER_BLUR)
    {
        float blur = ofMap(mouseX, 0, ofGetWidth(), 0, 10, true);

        //----------------------------------------------------------
        fboBlurOnePass.begin();

        shaderBlurX.begin();
        shaderBlurX.setUniform1f("blurAmnt", blur);

        //currentMovie.draw(0, 0, targetW, targetH);
        image.draw(0, 0, targetW, targetH);

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
    else if (playbackMode == PBM_SHADER_BRU)
    {
        // bind our texture. in our shader this will now be tex0 by default
        // so we can just go ahead and access it there.
        currentMovie.getTextureReference().bind();

        // start our shader, in our OpenGL3 shader this will automagically set
        // up a lot of matrices that we want for figuring out the texture matrix
        // and the modelView matrix
        shaderBru.begin();

        // center screen.
        float cx = ofGetWidth() / 2.0;
        float cy = ofGetHeight() / 2.0;
        float mx = mouseX - cx;
        float my = mouseY - cy;
        shaderBru.setUniform2f("mousePos", mx, my);

		shaderBru.setUniform1f("videoW", currentMovie.getWidth());
		shaderBru.setUniform1f("videoH", currentMovie.getHeight());
		shaderBru.setUniform1f("windowW", (float)(ofGetWindowWidth()));
		shaderBru.setUniform1f("windowH", (float)(ofGetWindowHeight()));

        ofPushMatrix();
        ofTranslate(cx, cy);
        //plane.drawWireframe();
		plane.draw();
        ofPopMatrix();

        shaderBru.end();
        currentMovie.getTextureReference().unbind();
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
        videoChanged();
    }
    else if (key == 'a')
    {
        playbackMode = (playbackMode - 1)% PBM_COUNT;
		ofLogNotice("Current mode: " + ofToString(playbackMode));
    }
    else if (key == 'd')
    {
        playbackMode = (playbackMode + 1)% PBM_COUNT;
		ofLogNotice("Current mode: " + ofToString(playbackMode));
    }
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

//--------------------------------------------------------------
std::string ofApp::getVideoDetails(ofVideoPlayer& video)
{
    std::string result = "Current video is now '" + allMovies[currentMovieIndex].getFileName() + "', W=" + ofToString(currentMovie.getWidth()) + ", H=" + ofToString(currentMovie.getHeight());
    return result;
}

//--------------------------------------------------------------
void ofApp::videoChanged()
{
    // Things to do when the source video changes
    ofLog(ofLogLevel::OF_LOG_NOTICE, getVideoDetails(currentMovie));

    float windowW = ofGetWindowWidth();
    float windowH = ofGetWindowHeight();
    float videoW = currentMovie.getWidth();
    float videoH = currentMovie.getHeight();

    bool landscapeVideo = videoW > videoH;
    bool fitToWidth = landscapeVideo;
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

    plane.mapTexCoordsFromTexture(currentMovie.getTextureReference());
}
