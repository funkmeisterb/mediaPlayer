#pragma once

#include "ofMain.h"

#define PBM_NORMAL 0
#define PBM_DOTS 1
#define PBM_SHADER1 2
#define PBM_SHADER2 3
#define PBM_COUNT 4

class ofApp : public ofBaseApp {
	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

    private:
        std::string         getVideoDetails(ofVideoPlayer& video);

        ofVideoPlayer 		currentMovie;
        std::vector<ofFile> allMovies;
        unsigned int        currentMovieIndex;
        unsigned int        playbackMode;
    
        ofShader            shader1;
        ofPlanePrimitive    plane;
    
        ofShader shaderBlurX;
        ofShader shaderBlurY;
        ofFbo fboBlurOnePass;
        ofFbo fboBlurTwoPass;
};
