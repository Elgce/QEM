#include <iostream>
#include <stdlib.h>
#include "display.h"
#include "inputManager.h"

Display display(DISPLAY_WIDTH, DISPLAY_HEIGHT, "OpenGL");	
Scene scn(glm::vec3(0.0f, 0.0f, -15.0f), CAM_ANGLE, relation, NEAR, FAR);

bool supportFile(char* name) {
	int i = 0;
	for (; name[i] != '\0'; i++);
	if (name[i - 1] == 'j' && name[i - 2] == 'b' && name[i - 3] == 'o' && name[i - 4] == '.') {
		return true;
	}
	if (name[i - 1] == 'l' && name[i - 2] == 't' && name[i - 3] == 's' && name[i - 4] == '.') {
		return true;
	}
	if (name[i - 1] == 'L' && name[i - 2] == 'T' && name[i - 3] == 'S' && name[i - 4] == '.') {
		return true;
	}
	return false;
}

bool isNumber(char* argv) {
	float val = atof(argv);
	if (val <= 0 || val > 1) {
		return false;
	}
	return true;
}

double getNumber(char* argv) {
	float val = atof(argv);
	return (double)val;
}

int main(int argc, char** argv)
{
	//initCallbacks(display);
	/*if (argc != 3 || !supportFile(argv[1]) || !isNumber(argv[2])) {
		printf("please input: MeshSimplification xxx.obj\\xxx.stl n\n(n is a double whose value is larger than 0 and less or equal to 1)\n");
		system("pause");
		return 0;
	}*/

	Shape* newShape = new Shape("../file/testSuccess.obj", 1, 0.5, "../res/output1.stl");
	// testboxNoUV - Start faces = 12
	//scn.addShape("../file/kms1.obj", glm::vec3(-5, 0, 0), 0);
	//scn.addShape("../file/kms2.obj", glm::vec3(5, 0, 0), 1);

	// monkey3 - Start faces = 4k
	//Shape* newShape = new Shape("./res/objs/monkey3.obj", 1, 0.5);
	//Shape* newShape = new Shape("./res/objs/cabin.obj", 1, 0.5);
	//scn.addShape("./res/objs/cabin.obj", glm::vec3(5, 0, 0), 1);
	//scn.addShape("./res/objs/cabin.obj", glm::vec3(5, 0, 0), 0);

	//
	//Shape* newShape = new Shape("./res/objs/walter.obj", 1, 1);
	//scn.addShape("./res/objs/walter.obj", glm::vec3(-5, 0, 0), 0);
	//scn.addShape("./res/objs/walter.obj", glm::vec3(5, 0, 0), 1);
	
	//scn.addShape("./res/objs/gun-pbribl.3dcool.net.obj", glm::vec3(-5, 0, 0), 0);
	//scn.addShape("./res/objs/gun-pbribl.3dcool.net.obj", glm::vec3(5, 0, 0), 1);
	// Wooden Crate - Start faces = 298
	//scn.addShape("./res/objs/Wooden Crate.obj", glm::vec3(-5, 0, 0), 0);
	//scn.addShape("./res/objs/Wooden Crate.obj", glm::vec3(5, 0, 0), 1);
	
//	scn.addShader("./res/shaders/basicShader");
//	scn.addShader("./res/shaders/pickingShader");

	/*while (!display.toClose())
	{
		display.Clear(0.7f, 0.7f, 0.7f, 1.0f);
		scn.draw(0, 0, true);
		display.SwapBuffers();
		display.pullEvent();
	}*/

/*
	glm::mat4 addQ1;
	addQ1[0][0] = 1;
	addQ1[0][1] = 5;
	addQ1[0][2] = 3;
	addQ1[0][3] = 7;
	addQ1[1][0] = 2;
	addQ1[1][1] = 8;
	addQ1[1][2] = 4;
	addQ1[1][3] = 3;
	addQ1[2][0] = 2;
	addQ1[2][1] = 3;
	addQ1[2][2] = 1;
	addQ1[2][3] = 5;
	addQ1[3][0] = 1;
	addQ1[3][1] = 5;
	addQ1[3][2] = 7;
	addQ1[3][3] = 4;
	glm::vec4 a = glm::vec4(1, 2, 3, 4);
	glm::vec4 vec1 = a * addQ1;
	double res1 = glm::dot(vec1, a);
	double res3 = glm::dot(a, vec1);

	glm::vec4 vec2 = addQ1 * a;
	double res2 = glm::dot(a, vec2);
*/

	printf("finish!\n");
	system("pause!");
	return 0;
}
