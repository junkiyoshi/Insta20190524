#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(30);
	ofSetWindowTitle("openFrameworks");

	ofBackground(239);

	this->cap_size = cv::Size(1280, 720);
	this->cap.open("D:\\video\\image2.mp4");
	this->image.allocate(this->cap_size.width, this->cap_size.height, OF_IMAGE_COLOR);
	this->frame = cv::Mat(cv::Size(this->image.getWidth(), this->image.getHeight()), CV_MAKETYPE(CV_8UC3, this->image.getPixels().getNumChannels()), this->image.getPixels().getData(), 0);

	this->fbo.allocate(ofGetWidth(), ofGetHeight());
	this->shader.load("shader/shader.vert", "shader/shader.frag");

	this->location = glm::vec2(ofRandom(ofGetWidth() * 0.2, ofGetWidth() * 0.8), ofRandom(ofGetHeight() * 0.2, ofGetHeight() * 0.8));
	this->percent = 0.0;
	this->noise_seed = ofRandom(10000);
	this->noise_scale = ofRandom(3, 8);
	this->color.setHsb(ofRandom(255), 255, 255);
}

//--------------------------------------------------------------
void ofApp::update() {

	// Read mp4 file.
	cv::Mat src;
	this->cap >> src;
	if (src.empty()) {

		this->cap.set(CV_CAP_PROP_POS_FRAMES, 1);
		return;
	}
	cv::resize(src, this->frame, this->cap_size);
	cv::cvtColor(this->frame, this->frame, CV_BGR2RGB);
	this->image.update();

	// Draw image.
	if (this->percent < 1.0) {

		this->percent += 0.30;
	}
	else {

		this->location = glm::vec2(ofRandom(ofGetWidth()), ofRandom(ofGetHeight()));
		this->percent = 0.f;
		this->noise_seed = ofRandom(10000);
		this->noise_scale = ofRandom(3, 8);
		this->color.setHsb(ofRandom(255), 255, 255);
	}

	this->fbo.begin();
	float base_radius = 80;

	ofPushMatrix();
	ofTranslate(this->location);

	vector<glm::vec2> vertices;
	for (int i = 0; i < 3600; i++) {

		float deg = i * 0.1;
		auto noise_point = glm::vec2(cos(deg * DEG_TO_RAD), sin(deg * DEG_TO_RAD));
		auto noise_value = ofNoise(this->noise_seed, noise_point.x * this->noise_scale, noise_point.y * this->noise_scale);
		float grow_radius = base_radius + pow((1 + noise_value), 7) * this->percent;

		auto point = glm::vec2(grow_radius * cos(deg * DEG_TO_RAD), grow_radius * sin(deg * DEG_TO_RAD));
		vertices.push_back(point);
	}

	ofSetColor(this->color);

	ofBeginShape();
	ofVertices(vertices);
	ofEndShape();

	ofPopMatrix();

	this->fbo.end();
}

//--------------------------------------------------------------
void ofApp::draw() {

	this->shader.begin();
	this->shader.setUniform1f("time", ofGetElapsedTimef());
	this->shader.setUniform2f("resolution", ofGetWidth(), ofGetHeight());
	this->shader.setUniformTexture("tex1", this->image, 1);
	this->shader.setUniformTexture("tex2", this->fbo.getTexture(), 2);

	ofFill();
	ofSetColor(255);
	ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
	this->shader.end();
}

//--------------------------------------------------------------
int main() {

	ofGLWindowSettings settings;
	settings.setGLVersion(3, 2);
	settings.setSize(1280, 720);
	ofCreateWindow(settings);
	ofRunApp(new ofApp());
}