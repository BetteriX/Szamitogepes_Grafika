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
	NoTexture,		// fixes 0 sized array problem
	TextureCount
};
enum eAnimationType {
	RotateFirst,
	TranslateFirst,
	Bouncing
};

#include <common.cpp>

/** std::array használható a glm::vec2 csúcspontok tárolásához. */
/** std::array can be used to store glm::vec2 vertices. */
static array<vec2, 3> vertices = {
	//     X      Y
	vec2(-0.5f, -0.5f),
	vec2(0.5f, -0.5f),
	vec2(0.0f,  0.5f),
};

GLchar			windowTitle[] = "Animation by MVP and Uniform Variables";

GLuint			XoffsetLocation;
GLuint			YoffsetLocation;

GLuint			redLocation;
GLuint			greenLocation;
GLuint			blueLocation;
GLuint			uniformColorLocation;

GLfloat			x				= 0.00f;
GLfloat			y				= 0.00f;
GLfloat			increment		= 0.01f;
GLboolean		xDir			= GL_TRUE;
GLboolean		yDir			= GL_FALSE;
mat4			translateMatrix	= translate(mat4(1.0f), vec3(-0.5, 0.5, 0.0));
mat4			rotateMatrix	= rotate(mat4(1.0f), (GLfloat)glfwGetTime(), vec3(0.0f, 0.0f, 1.0f));
eAnimationType	animationType	= Bouncing;

void initShaderProgram() {
	ShaderInfo shader_info[] = {
		{ GL_FRAGMENT_SHADER,	"./fragmentShader.glsl" },
		{ GL_VERTEX_SHADER,		"./vertexShader.glsl" },
		{ GL_NONE,				nullptr }
	};
	/** A vertex-fragment program elkészítése. */
	/** Creating the vertex-fragment program. */
	program[QuadScreenProgram] = LoadShaders(shader_info);
	/** A GL_ARRAY_BUFFER nevesített csatolóponthoz kapcsoljuk a vertex buffert (ide kerülnek a csúcspont adatok). */
	/** We attach the vertex buffer to the GL_ARRAY_BUFFER node (vertices are stored here). */
	glBindBuffer(GL_ARRAY_BUFFER, BO[VBOVerticesData]);
	/** Másoljuk az adatokat a bufferbe! Megadjuk az aktuálisan csatolt buffert, azt hogy hány byte adatot másolunk,
		a másolandó adatot, majd a feldolgozás módját is meghatározzuk: most az adat nem változik a feltöltés után. */
	/** Copy the data to the buffer! First parameter is the currently attached buffer, second is the size of the buffer to be copied,
		third is the array of data, fourth is working mode: now the data can not be modified after this step. */
	/** std::array megadása paraméternek.                         pointer to first element */
	/** std::array as parameter.  size in bytes                                    GPU memory organization */
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec2), vertices.data(), GL_STATIC_DRAW);
	/** Csatoljuk a vertex array objektumunkat a shader programhoz. */
	/** Attach the vertex array object to the shader program. */
	glBindVertexArray(VAO[VAOVerticesData]);
	/** Ezen adatok szolgálják a location = 0 vertex attribútumot (itt: pozíció).
		Elsõként megadjuk ezt az azonosítószámot (vertexShader.glsl).
		Utána az attribútum méretét (vec2, láttuk a shaderben).
		Harmadik az adat típusa.
		Negyedik az adat normalizálása, ez maradhat FALSE jelen példában.
		Az attribútum értékek hogyan következnek egymás után? Milyen lépésköz után találom a következõ vertex adatait?
		Végül megadom azt, hogy honnan kezdõdnek az értékek a pufferben. Most rögtön, a legelejétõl veszem õket. */
	/** These values are for location = 0 vertex attribute (position).
		First is the location (vertexShader.glsl).
		Second is attribute size (vec2, as in the shader).
		Third is the data type.
		Fourth defines whether data shall be normalized or not, this is FALSE for the examples of the course.
		Fifth is the distance in bytes to the next vertex element of the array.
		Last is the offset of the first vertex data of the buffer. Now it is the start of the array. */
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	/** Engedélyezzük az imént definiált location = 0 attribútumot (vertexShader.glsl). */
	/** Enable the previously defined location = 0 attributum (vertexShader.glsl). */
	glEnableVertexAttribArray(0);
	/** A shader változók location-jeinek lekérdezése. */
	/** Get shader variable locations. */
	locationMatProjection = glGetUniformLocation(program[QuadScreenProgram], "matProjection");
	locationMatModelView = glGetUniformLocation(program[QuadScreenProgram], "matModelView");
	/** Lekérdezzük offsetX és offsetY attribútumok helyét (vertexShader.glsl). */
	/** Get the location of offsetX and offsetY attributums (vertexShader.glsl). */
	XoffsetLocation = glGetUniformLocation(program[QuadScreenProgram], "offsetX");
	YoffsetLocation = glGetUniformLocation(program[QuadScreenProgram], "offsetY");

	redLocation = glGetUniformLocation(program[QuadScreenProgram], "redColor");
	greenLocation = glGetUniformLocation(program[QuadScreenProgram], "greenColor");
	blueLocation = glGetUniformLocation(program[QuadScreenProgram], "blueColor");

	uniformColorLocation = glGetUniformLocation(program[QuadScreenProgram], "uniformColor");

	/** Aktiváljuk a shader-program objektumunkat az alapértelmezett fix csõvezeték helyett. */
	/** Activate our shader-program object instead of the default fix pipeline. */
	glUseProgram(program[QuadScreenProgram]);
}

void display(GLFWwindow* window, double currentTime) {
	/** Töröljük le a kiválasztott buffereket! Fontos lehet minden egyes alkalommal törölni! */
	/** Let's clear the selected buffers! Usually importand to clear it each time! */
	glClear(GL_COLOR_BUFFER_BIT);
	switch (animationType) {
	case RotateFirst:		// without offset
		glProgramUniform1f(program[QuadScreenProgram], XoffsetLocation, 0.0f);
		glProgramUniform1f(program[QuadScreenProgram], YoffsetLocation, 0.0f);
		// A forgást az eltelt idő hajtja a Z tengely körül.
		// Time is driving the rotation around axis Z.
		rotateMatrix = rotate(glm::mat4(1.0f), (GLfloat)currentTime, vec3(0.0f, 0.0f, 1.0f));
		// !!!FIGYELEM!!! A transzform'ci=k sorrendje jobbról balra halad! P' = Projection * View * Model * P
		// !!!BEWARE!!! Transformation order is from right to left! P' = Projection * View * Model * P
		matModel = translateMatrix * rotateMatrix;
		break;
	case TranslateFirst:	// without offset
		glProgramUniform1f(program[QuadScreenProgram], XoffsetLocation, 0.0f);
		glProgramUniform1f(program[QuadScreenProgram], YoffsetLocation, 0.0f);
		// A forgást az eltelt idő hajtja a Z tengely körül.
		// Time is driving the rotation around axis Z.
		rotateMatrix = rotate(glm::mat4(1.0f), (GLfloat)currentTime, vec3(0.0f, 0.0f, 1.0f));
		// !!!FIGYELEM!!! A transzform'ci=k sorrendje jobbról balra halad! P' = Projection * View * Model * P
		// !!!BEWARE!!! Transformation order is from right to left! P' = Projection * View * Model * P
		matModel = rotateMatrix * translateMatrix;
		break;
	case Bouncing:
		matModel = mat4(1.0);
		if (xDir) {
			/** Mozgassuk a háromszöget az x-tengely mentén! */
			/** Let's move the triangle amoing x axis! */
			x += increment;
			y += increment;
			/** A falakról pattanjunk vissza. */
			/** Let's bounce back from the walls. */
			if ((x > 0.5f) || (x < -0.5f)) increment = -increment;
			/** Küldjük el az "x" értékét az "offsetX" számára. */
			/** Send value of "x" to "offsetX" in the shader. */
			glProgramUniform1f(program[QuadScreenProgram], XoffsetLocation, x);
			glProgramUniform1f(program[QuadScreenProgram], YoffsetLocation, y);
		}
		else if (yDir) {
			/** Mozgassuk a háromszöget az y-tengely mentén! */
			/** Let's move the triangle amoing y axis! */
			y += increment;
			/** A falakról pattanjunk vissza. */
			/** Let's bounce back from the walls. */
			if ((y > 0.5f) || (y < -0.5f)) increment = -increment;
			/** Küldjük el az "y" értékét az "offsetY" számára. */
			/** Send value of "y" to "offsetY" in the shader. */
			glProgramUniform1f(program[QuadScreenProgram], YoffsetLocation, y);
		}
		break;
	}

	glProgramUniform1f(program[QuadScreenProgram], redLocation, 1.0f);
	glProgramUniform1f(program[QuadScreenProgram], greenLocation, 0.0f);
	glProgramUniform1f(program[QuadScreenProgram], blueLocation, 0.0f);

	vec3 uniformColor(1.0f, 0.0f, 0.0f);
	glProgramUniform3fv(program[QuadScreenProgram], uniformColorLocation, 1, value_ptr(uniformColor));

	/** A megadott adatok segítségével háromszöget rajzolunk. */
	/** We draw triangle with the defined arrays. */
	matModelView = matView * matModel;
	glUniformMatrix4fv(locationMatModelView, 1, GL_FALSE, value_ptr(matModelView));
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	/** A minimalizálás nem fog fagyni a minimum 1 értékkel. */
	/** Minimize will not freeze with minimum value 1. */
	windowWidth = glm::max(width, 1);
	windowHeight = glm::max(height, 1);

	float aspectRatio = (float)windowWidth / (float)windowHeight;
	/** A kezelt képernyõ beállítása a teljes (0, 0, width, height) területre. */
	/** Set the viewport for the full (0, 0, width, height) area. */
	glViewport(0, 0, windowWidth, windowHeight);
	/** Orthographic projekció beállítása, worldSize lesz a szélesség és magasság közül a kisebbik. */
	/** Set up orthographic projection, worldSize will equal the smaller value of width or height. */
	if (projectionType == Orthographic)
		if (windowWidth < windowHeight)
			matProjection = ortho(-worldSize, worldSize, -worldSize / aspectRatio, worldSize / aspectRatio, -100.0, 100.0);
		else
			matProjection = ortho(-worldSize * aspectRatio, worldSize * aspectRatio, -worldSize, worldSize, -100.0, 100.0);
	else
		matProjection = perspective(
			radians(45.0f),	// The vertical Field of View, in radians: the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
			aspectRatio,	// Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar?
			0.1f,			// Near clipping plane. Keep as big as possible, or you'll get precision issues.
			100.0f			// Far clipping plane. Keep as little as possible.
		);
	matModel = mat4(1.0);
	matView = lookAt(
		vec3(0.0f, 0.0f, 9.0f),		// the position of your camera, in world space
		vec3(0.0f, 0.0f, 0.0f),		// where you want to look at, in world space
		vec3(0.0f, 1.0f, 0.0f));	// upVector, probably glm::vec3(0,1,0), but (0,-1,0) would make you looking upside-down, which can be great too
	matModelView = matView * matModel;
	glUniformMatrix4fv(locationMatModelView, 1, GL_FALSE, value_ptr(matModelView));
	glUniformMatrix4fv(locationMatProjection, 1, GL_FALSE, value_ptr(matProjection));
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	/** ESC billentyûre kilépés. */
	/** Exit on ESC key. */
	if ((action == GLFW_PRESS) && (key == GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	/** A billentyûk lenyomásának és felengedésének regisztrálása. Lehetõvé teszi gombkombinációk használatát. */
	/** Let's register press and release events for keys. Enables the usage of key combinations. */
	if (action == GLFW_PRESS)
		keyboard[key] = GL_TRUE;
	else if (action == GLFW_RELEASE)
		keyboard[key] = GL_FALSE;
	/** Az O billentyû lenyomása az orthographic leképezést kapcsolja be. */
	/** Pressing O induces orthographic projection. */
	if (key == GLFW_KEY_O && action == GLFW_PRESS) {
		projectionType = Orthographic;
		framebufferSizeCallback(window, windowWidth, windowHeight);
	}
	/** A P billentyû lenyomása a perspective leképezést kapcsolja be. */
	/** Pressing P induces perspective projection. */
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		projectionType = Perspective;
		framebufferSizeCallback(window, windowWidth, windowHeight);
	}
	/** Az R billentyû lenyomása a forgatást hajtja végre először. */
	/** Pressing R induces rotate first. */
	if (key == GLFW_KEY_R && action == GLFW_PRESS) animationType = RotateFirst;
	/** A T billentyû lenyomása az eltolást hajtja végre először. */
	/** Pressing T induces translate first. */
	if (key == GLFW_KEY_T && action == GLFW_PRESS) animationType = TranslateFirst;
	/** A B billentyû lenyomása visszapattanó mozgást indukál. */
	/** Pressing B induces bouncing movement. */
	if (key == GLFW_KEY_B && action == GLFW_PRESS) animationType = Bouncing;
	/** A V billentyû lenyomása vertikális mozgást indukál. */
	/** Pressing V induces vertical movement. */
	if (key == GLFW_KEY_V && action == GLFW_PRESS) {
		xDir = false;
		yDir = true;
	}
	/** A H billentyû lenyomása horizontális mozgást indukál. */
	/** Pressing H induces horizontal movement. */
	if (key == GLFW_KEY_H && action == GLFW_PRESS) {
		xDir = true;
		yDir = false;
	}
}

void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {
	/** Az egér mutató helyét kezelõ függvény. */
	/** Callback function for mouse position change. */
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	/** Az egér gombjaihoz köthetõ események kezelése. */
	/** Callback function for mouse button events. */
}

int main(void) {
	/** Az alkalmazáshoz kapcsolódó elõkészítõ lépések (paraméterek: major, minor OpenGL verziók, OpenGL pofile). */
	/** The first initialization steps of the program (params: major, minor OpenGL versions, OpenGL pofile). */
	init(3, 3, GLFW_OPENGL_COMPAT_PROFILE);
	//init(3, 3, GLFW_OPENGL_CORE_PROFILE);	// Makes big points square instead of circle.
	/** A rajzoláshoz használt shader programok betöltése. */
	/** Loading the shader programs for rendering. */
	initShaderProgram();
	/** Első értékadás a uniform változóknak (pl: MVP változók). */
	/** First parameter value setting for uniform variables (e.g.: MVP variables). */
	framebufferSizeCallback(window, windowWidth, windowHeight);
	/** Karakterkódolás a szövegekhez. */
	/** Setting locale for characters of texts. */
	setlocale(LC_ALL, "");
	cout << "Animation by MVP and uniform variables" << endl;
	cout << "https://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/#the-model-view-and-projection-matrices" << endl;
	cout << "https://www.khronos.org/opengl/wiki/Uniform_(GLSL)" << endl << endl;
	cout << "Keyboard control" << endl;
	cout << "ESC\texit" << endl;
	cout << "O\tinduces orthographic projection" << endl;
	cout << "P\tinduces perspective projection" << endl;
	cout << "R\tfor rotate first and AFTER that translate" << endl;
	cout << "T\tfor translate first and AFTER that rotate" << endl;
	cout << "B\tfor bouncing" << endl;
	cout << " H\tfor horizontal bouncing" << endl;
	cout << " V\tfor horizontal bouncing" << endl << endl;
	cout << "Weekly tasks" << endl;
	cout << "Gyakorlat 1: Vegyünk fel std:array tömböt a színeknek!" << endl;
	cout << "Gyakorlat 2: Vegyünk fel második VBO-t a színeknek és használjuk!" << endl;
	cout << "Gyakorlat 3: Használjuk az offset értékeket a színek módosításához!" << endl;
	cout << "Activity  1: Create a std:array for colors!" << endl;
	cout << "Activity  2: Define a second VBO for the colors and use it!" << endl;
	cout << "Activity  3: Use the offset values for modifying the colors!" << endl;
	/** A megadott window struktúra "close flag" vizsgálata. */
	/** Checks the "close flag" of the specified window. */
	while (!glfwWindowShouldClose(window)) {
		/** A kód, amellyel rajzolni tudunk a GLFWwindow objektumunkba. */
		/** Call display function which will draw into the GLFWwindow object. */
		display(window, glfwGetTime());
		/** Double buffered mûködés. */
		/** Double buffered working = swap the front and back buffer here. */
		glfwSwapBuffers(window);
		/** Események kezelése az ablakunkkal kapcsolatban, pl. gombnyomás. */
		/** Handle events related to our window, e.g.: pressing a key or moving the mouse. */
		glfwPollEvents();
	}
	/** Felesleges objektumok törlése. */
	/** Clenup the unnecessary objects. */
	cleanUpScene(EXIT_SUCCESS);
	/** Kilépés EXIT_SUCCESS kóddal. */
	/** Stop the software and exit with EXIT_SUCCESS code. */
	return EXIT_SUCCESS;
}