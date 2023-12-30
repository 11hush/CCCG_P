// references: https://www.songho.ca/opengl/gl_sphere.html
// implemented by myself, follow the idea of sphere in the website

#ifndef _SPHERE_H_
#define _SPHERE_H_


#include <glad/glad.h>

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <math.h>

#define PI 3.1415926

class Sphere {
private:
	std::vector<float> vertex;
	std::vector<int> index;

	float r = 1.0; // the radius of the sphere

	int stkCnt = 180; // modify this to change stack count
	int secCnt = 360; // modify this to change sector count
	
	// variables for gl
	unsigned int VAO{ 0 };
	unsigned int VBO{ 0 };
	unsigned int EBO{ 0 };
public:
	Sphere(float radius) :r(radius) {
		// first get the vertex
		float x;
		float y;
		float z;
		float r_cos_fi;

		float tex_x;
		float tex_y;

		float stkStep = (PI / (float)stkCnt);
		float secStep = (2 * PI / (float)secCnt);
		
		for (int i = 0; i <= stkCnt;i++) {
			float fi = PI / 2 - i * stkStep;
			r_cos_fi = radius * cosf(fi);
			z = radius * sinf(fi);
			for (int j = 0; j <= secCnt; j++) {
				// determine x and y
				float theta = j * secStep;
				x = r_cos_fi * cosf(theta);
				y = r_cos_fi * sinf(theta);
				
				// put the coordinate
				vertex.emplace_back(x);
				vertex.emplace_back(y);
				vertex.emplace_back(z);

				// normals
				vertex.emplace_back(x);
				vertex.emplace_back(y);
				vertex.emplace_back(z);
				// put the vertex coordinate
				tex_x = (float)j / secCnt;
				tex_y = (float)i / stkCnt;
				vertex.emplace_back(tex_x);
				vertex.emplace_back(tex_y);
			}
		}
		// then get the index
		
		for (int i = 0; i < stkCnt; i++) {
			int k1 = i * (secCnt + 1);
			int k2 = (i + 1) * (secCnt + 1);
			for (int j = 0; j < secCnt; j++) {
				if (i) {
					// push the first one
					index.emplace_back(k1);
					index.emplace_back(k2);
					index.emplace_back(k1+1);
				}
				if (i != (stkCnt - 1)) {
					// push the second one
					index.emplace_back(k1 + 1);
					index.emplace_back(k2);
					index.emplace_back(k2 + 1);
				}
				k1++;
				k2++;
			}
		}
		// finally ask gl to bind it
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(float), vertex.data(), GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(int), index.data(), GL_DYNAMIC_DRAW);
		
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		

		// unbind
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void draw() {
		// draw the sphere to the screen
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES,index.size(),GL_UNSIGNED_INT,0);
		glBindVertexArray(0);
	}

	~Sphere() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}
};


#endif