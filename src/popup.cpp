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
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "popup.hpp"
#include "graphics/shader.hpp"
#include "resource_manager.hpp"

namespace bookr {

static unsigned int VBO, VAO, EBO;
static int width, height, nrChannels;
  
static float vertices[] = {
    // positions
     1.0f,  1.0f, 0.0f, // top right
     1.0f, -1.0f, 0.0f, // bottom right
    -1.0f, -1.0f, 0.0f, // bottom left
    -1.0f,  1.0f, 0.0f  // top left
};
static unsigned int indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

Popup::Popup(int m, string t) : mode(m), text(t)
{
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

	// glm::mat4 model = glm::mat4(1.0f);
	// glm::vec2 size = glm::vec2(1.0f);
	// GLfloat rotate = 0.0f;
	// model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));  // First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)

	// model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // Move origin of rotation to center of quad
	// model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f)); // Then rotate
	// model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // Move origin back

	// model = glm::scale(model, glm::vec3(1.0f)); // Last scale

	ResourceManager::GetShader("rectangle").Use();

  //ResourceManager::GetShader("polygon").Use();
	//ResourceManager::GetShader("polygon").SetVector4f("model", 100.0f, 100.0f, 0.0f, 0.0f);
	//ResourceManager::GetShader("polygon").SetVector4f("ourColor", 0.0f, 0.4f, 0.4f, 1.0f);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

Popup* Popup::create(int m, string t) {
	Popup* f = new Popup(m, t);
	Screen::resetReps();
	return f;
}

}