/*
 * bookr-modern: a graphics based document reader 
 * Copyright (C) 2019 pathway27 (Sree)
 * IS A MODIFICATION OF THE ORIGINAL
 * Bookr and bookr-mod for PSP
 * Copyright (C) 2005 Carlos Carrasco Martinez (carloscm at gmail dot com),
 *               2007 Christian Payeur (christian dot payeur at gmail dot com),
 *               2009 Nguyen Chi Tam (nguyenchitam at gmail dot com),
 * AND VARIOUS OTHER FORKS, See Forks in README.md
 * Licensed under GPLv3+, see LICENSE
*/

#include <cstring>

#include "popup.hpp"

namespace bookr {

static unsigned int VBO, VAO, EBO;
static int width, height, nrChannels;
  
static float vertices[] = {
    // positions
     0.5f,  0.5f, 0.0f, // top right
     0.5f, -0.5f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, // bottom left
    -0.5f,  0.5f, 0.0f  // top left
};
static unsigned int indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};


Popup::Popup(int m, string t) : mode(m), text(t),
  recShader("shaders/rectangle.vert", "shaders/rectangle.frag") {
	glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); 
}

Popup::~Popup() {
}

int Popup::update(unsigned int buttons) {
	int* b = Screen::ctrlReps();

	if (b[User::controls.cancel] == 1) {
			return BK_CMD_CLOSE_TOP_LAYER;
	}

	return 0;
}

void Popup::render() {
	string title;
	int bg1 = 0;
	int bg2 = 0;
	int fg = 0;
	if (mode == BKPOPUP_WARNING) {
		bg1 = 0xf02020a0;
		bg2 = 0xf06060ff;
		fg = 0xffffffff;
		title = "Warning";
	} else if (mode == BKPOPUP_INFO) {
		bg1 = 0xf0a02020;
		bg2 = 0xf0ff6060;
		fg = 0xffffffff;
		title = "Info";
	} else {
		bg1 = 0xf02020a0;
		bg2 = 0xf06060ff;
		fg = 0xffffffff;
		title = "Error";
	}
	drawPopup(text, title, bg1, bg2, fg);
	// glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
 	// glClear(GL_COLOR_BUFFER_BIT);

    recShader.Use();
    int vertexColorLocation = glGetUniformLocation(recShader.Program, "ourColor");
    glUniform4f(vertexColorLocation, 0.0f, 0.33f, 0.0f, 1.0f);
    
    glBindVertexArray(VAO);
  	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

Popup* Popup::create(int m, string t) {
	Popup* f = new Popup(m, t);
	Screen::resetReps();
	return f;
}

}