enum eVertexArrayObject {
	VAOVerticesData,
	VAOCount
};
enum eBufferObject {
	VBOVerticesData,
	BOCount
};
enum eProgram {
	QuadScreenProgram,
	ProgramCount
};
enum eTexture {
	NoTexture,
	TextureCount
};

#include <common.cpp>

GLchar		windowTitle[] = "Bezier curve";
GLfloat		aspectRatio;
GLuint		isLine;

/* Kontrollpontok a Bézier-görbéhez. */
/* Control points for the Bezier curve. */
static vector<vec2> myControlPoints = {
	vec2(-0.5f, -0.5f),
	vec2(-0.5f,  0.5f),
	vec2(0.5f, -0.5f),
	vec2(0.5f,  0.5f),
};

/* A görbe pontjait tároló vektor. */
/* Vector storing the computed curve points. */
static vector<vec2> pointToDraw;

/*
The definition of N choose R is to compute the two products and divide one with the other,
(N * (N - 1) * (N - 2) * ... * (N - R + 1)) / (1 * 2 * 3 * ... * R)
However, the multiplications may become too large really quick and overflow existing data type.The implementation trick is to reorder the multiplication and divisions as,
(N) / 1 * (N - 1) / 2 * (N - 2) / 3 * ... * (N - R + 1) / R
It's guaranteed that at each step the results is divisible (for n continuous numbers, one of them must be divisible by n, so is the product of these numbers).
For example, for N choose 3, at least one of the N, N - 1, N - 2 will be a multiple of 3, and for N choose 4, at least one of N, N - 1, N - 2, N - 3 will be a multiple of 4.
C++ code given below.
*/
int NCR(int n, int r) {
	/*
	binomial coefficient
	*/
	if (r == 0) return 1;

	/*
	 Extra computation saving for large R,
	 using property:
	 N choose R = N choose (N - R)
	*/
	if (r > n / 2) return NCR(n, n - r);

	long res = 1;

	for (int k = 1; k <= r; ++k) {
		res *= n - k + 1;
		res /= k;
	}

	return res;
}

/*
It will be the Bernstein basis polynomial of degree n.
*/
GLfloat blending(GLint n, GLint i, GLfloat t) {
	return NCR(n, i) * pow(t, i) * pow(1.0f - t, n - i);
}

/*
 * Feltölti a pointToDraw vektort a Bézier-görbe pontjaival.
 * Fills pointToDraw with the sampled Bezier curve points.
 */
void drawBezierCurve(const vector<vec2>& controlPoints) {
	pointToDraw.clear();

	/*
	https://en.wikipedia.org/wiki/B%C3%A9zier_curve#Explicit_definition
	*/

	glm::vec3	nextPoint;
	GLfloat		t = 0.0f, B;
	GLfloat		increment = 1.0f / 100.0f; /* hány darab szakaszból rakjuk össze a görbénket? */

	while (t <= 1.0f) {
		nextPoint = glm::vec3(0.0f, 0.0f, 0.0f);
		for (int i = 0; i < controlPoints.size(); i++) {
			B = blending(controlPoints.size() - 1, i, t);
			nextPoint.x += B * controlPoints.at(i).x;
			nextPoint.y += B * controlPoints.at(i).y;
		}

		pointToDraw.push_back(nextPoint);
		t += increment;
	}
}

void initShaderProgram() {
	ShaderInfo shader_info[ProgramCount][3] = { {
		{ GL_VERTEX_SHADER,   "./vertexShader.glsl"   },
		{ GL_FRAGMENT_SHADER, "./fragmentShader.glsl" },
		{ GL_NONE, nullptr } } };

	for (int programItem = 0; programItem < ProgramCount; programItem++) {
		program[programItem] = LoadShaders(shader_info[programItem]);
		locationMatModel = glGetUniformLocation(program[programItem], "matModel");
		locationMatView = glGetUniformLocation(program[programItem], "matView");
		locationMatProjection = glGetUniformLocation(program[programItem], "matProjection");
		isLine = glGetUniformLocation(program[programItem], "isLine");
	}

	/* Bézier-görbe pontjainak kiszámítása. */
	/* Compute Bezier curve points. */
	drawBezierCurve(myControlPoints);

	/* VAO és VBO bekötése, adatok feltöltése. */
	/* Bind VAO and VBO, upload data. */
	glBindVertexArray(VAO[VAOVerticesData]);
	glBindBuffer(GL_ARRAY_BUFFER, BO[VBOVerticesData]);
	glBufferData(GL_ARRAY_BUFFER, pointToDraw.size() * sizeof(vec2), pointToDraw.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), 0);

	glUseProgram(program[QuadScreenProgram]);

	/* Mátrixok kezdőértéke. / Initial matrix values. */
	matModel = mat4(1.0);
	matView = lookAt(
		vec3(0.0f, 0.0f, 9.0f),
		vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f));

	glUniformMatrix4fv(locationMatModel, 1, GL_FALSE, value_ptr(matModel));
	glUniformMatrix4fv(locationMatView, 1, GL_FALSE, value_ptr(matView));
	glUniformMatrix4fv(locationMatProjection, 1, GL_FALSE, value_ptr(matProjection));
	glUniform1i(isLine, 1);
}

void display(GLFWwindow* window, double currentTime) {
	glClear(GL_COLOR_BUFFER_BIT);

	/* Bézier-görbe rajzolása összekötött szakaszokkal. */
	/* Draw the Bezier curve as a connected line strip. */
	glUniform1i(isLine, 1);
	glLineWidth(2.0f);
	glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)pointToDraw.size());
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	windowWidth = glm::max(width, 1);
	windowHeight = glm::max(height, 1);
	aspectRatio = (float)windowWidth / (float)windowHeight;

	glViewport(0, 0, windowWidth, windowHeight);

	if (projectionType == Orthographic) {
		if (windowWidth < windowHeight)
			matProjection = ortho(-worldSize, worldSize, -worldSize / aspectRatio, worldSize / aspectRatio, -100.0, 100.0);
		else
			matProjection = ortho(-worldSize * aspectRatio, worldSize * aspectRatio, -worldSize, worldSize, -100.0, 100.0);
	}
	else {
		matProjection = perspective(
			radians(45.0f),
			aspectRatio,
			0.1f,
			100.0f);
	}

	glUniformMatrix4fv(locationMatProjection, 1, GL_FALSE, value_ptr(matProjection));
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (action == GLFW_PRESS)  keyboard[key] = GL_TRUE;
	else if (action == GLFW_RELEASE) keyboard[key] = GL_FALSE;

	if (key == GLFW_KEY_O && action == GLFW_PRESS) {
		projectionType = Orthographic;
		framebufferSizeCallback(window, windowWidth, windowHeight);
	}
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		projectionType = Perspective;
		framebufferSizeCallback(window, windowWidth, windowHeight);
	}
}

void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {}
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {}


int main(void) {
	init(3, 3, GLFW_OPENGL_COMPAT_PROFILE);
	initShaderProgram();
	framebufferSizeCallback(window, windowWidth, windowHeight);
	setlocale(LC_ALL, "");

	cout << "Bezier curve" << endl << endl;
	cout << "Keyboard control" << endl;
	cout << "ESC\t\texit" << endl;
	cout << "O\t\tOrthographic projection" << endl;
	cout << "P\t\tPerspective projection" << endl;

	while (!glfwWindowShouldClose(window)) {
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	cleanUpScene(EXIT_SUCCESS);
	return EXIT_SUCCESS;
}