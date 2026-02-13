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

#include "common.cpp"

GLchar	windowTitle[] = "Materials and Z-buffer";
GLuint	cameraPositionLoc;
vec3	cameraPosition	= vec3( 0.0f, 0.0f, 1.5f),
		cameraTarget	= vec3( 0.0f, 0.0f, 0.0f),
		cameraUpVector	= vec3( 0.0f, 1.0f, 0.0f),
		cameraMovingX	= vec3(-1.0f, 0.0f, 0.0f),
		cameraMovingY	= vec3( 0.0f, 1.0f, 0.0f);
GLuint	lightPositionLoc;
vec3	lightPosition;			// it is a headlight, it will follow the camera
GLuint	inverseTransposeMatrixLoc;
mat3	inverseTransposeMatrix;	// for normal vectors of lighting

GLfloat verticesData[] = {
	// positions		  // colors			 // normals
	-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f,  0.0f,  1.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f
};

void initShaderProgram() {
	ShaderInfo shader_info[ProgramCount][3] = { {
		{ GL_VERTEX_SHADER,		"./vertexShader.glsl" },
		{ GL_FRAGMENT_SHADER,	"./fragmentShader.glsl" },
		{ GL_NONE, nullptr } } };

	for (int programItem = 0; programItem < ProgramCount; programItem++) {
		program[programItem] = LoadShaders(shader_info[programItem]);
		/** Shader változó location lekérdezése. */
		/** Getting shader variable location. */
		locationMatModel = glGetUniformLocation(program[programItem], "matModel");
		locationMatView = glGetUniformLocation(program[programItem], "matView");
		locationMatProjection = glGetUniformLocation(program[programItem], "matProjection");
	}
	/** Csatoljuk a vertex array objektumunkat a paraméterhez. */
	/** glBindVertexArray binds the vertex array object to the parameter. */
	glBindVertexArray(VAO[VAOVerticesData]);
	/** A GL_ARRAY_BUFFER nevesített csatolóponthoz kapcsoljuk a vertex buffert (ide kerülnek a csúcspont adatok). */
	/** We attach the vertex buffer to the GL_ARRAY_BUFFER node (vertices are stored here). */
	glBindBuffer(GL_ARRAY_BUFFER, BO[VBOVerticesData]);
	/** Másoljuk az adatokat a bufferbe! Megadjuk az aktuálisan csatolt buffert, azt hogy hány byte adatot másolunk,
		a másolandó adatot, majd a feldolgozás módját is meghatározzuk: most az adat nem változik a feltöltés után. */
	/** Copy the data to the buffer! First parameter is the currently attached buffer, second is the size of the buffer to be copied,
		third is the array of data, fourth is working mode: now the data can not be modified after this step. */
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesData), verticesData, GL_STATIC_DRAW);
	/** Ezen adatok szolgálják a location = 0 vertex attribútumot (itt: pozíció).
		Elsõként megadjuk ezt az azonosítószámot (vertexShader.glsl).
		Utána az attribútum méretét (vec3, láttuk a shaderben).
		Harmadik az adat típusa.
		Negyedik az adat normalizálása, ez maradhat FALSE jelen példában.
		Az attribútum értékek hogyan következnek egymás után? Milyen lépésköz után találom a következõ vertex adatait?
		Végül megadom azt, hogy honnan kezdõdnek az értékek a pufferben. Most rögtön, a legelejétõl veszem õket. */
	/** These values are for location = 0 vertex attribute (position).
		First is the location (vertexShader.glsl).
		Second is attribute size (vec3, as in the shader).
		Third is the data type.
		Fourth defines whether data shall be normalized or not, this is FALSE for the examples of the course.
		Fifth is the distance in bytes to the next vertex element of the array.
		Last is the offset of the first vertex data of the buffer. Now it is the start of the array. */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
	/** Engedélyezzük az imént definiált location = 0 attribútumot (vertexShader.glsl ). */
	/** Enable the previously defined location = 0 attributum (vertexShader.glsl ). */
	glEnableVertexAttribArray(0);
	/** Ugyanez a szín attribútumhoz. */
	/** Same for color attributum. */
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	/** Ugyanez a normál vektor attribútumhoz. */
	/** Same for normal vector attributum. */
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	/** Az aktuális példaprogram bekapcsolása. */
	/** Switch on the current sample program. */
	glUseProgram(program[QuadScreenProgram]);
	/** Mátrixok kezdőértékének beállítása. */
	/** Set the matrices to the initial values. */
	projectionType = Perspective;
	matModel = mat4(1.0);
	matView = lookAt(
		cameraPosition,		// the position of your camera, in world space
		cameraTarget,		// where you want to look at, in world space
		cameraUpVector);	// upVector, probably glm::vec3(0,1,0), but (0,-1,0) would make you looking upside-down, which can be great too
	glUniformMatrix4fv(locationMatModel, 1, GL_FALSE, value_ptr(matModel));
	glUniformMatrix4fv(locationMatView, 1, GL_FALSE, value_ptr(matView));
	glUniformMatrix4fv(locationMatProjection, 1, GL_FALSE, value_ptr(matProjection));
	/** Shader változó location lekérdezése. */
	/** Getting shader variable location. */
	inverseTransposeMatrixLoc = glGetUniformLocation(program[QuadScreenProgram], "inverseTransposeMatrix");
	lightPositionLoc = glGetUniformLocation(program[QuadScreenProgram], "lightPosition");
	cameraPositionLoc = glGetUniformLocation(program[QuadScreenProgram], "cameraPosition");
	/** Fekete lesz a háttér. */
	/** Background is black. */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	/** Z-buffer engedélyezése. */
	/** Enable Z coordinate check for visibility. */
	glEnable(GL_DEPTH_TEST);
}

void computeModelMatrix() {
	/** A modellezés jelenleg egy skálázás. */
	/** Modellig is just a scaling now. */
	matModel = scale(mat4(1.0f), vec3(1.0f, 0.5f, 1.0f));
	glUniformMatrix4fv(locationMatModel, 1, GL_FALSE, value_ptr(matModel));
}

void computeCameraMatrix() {
	/** Kiszámoljuk a kamera mátrixokat, a paraméterek rendre: az új koordinátarendszerünk középpontja (hol a kamera), merre néz a kamera, mit tekintünk ,,fölfele" iránynak. */
	/** Calculate lookAt matrix by cameraPosition, cameraTarget and cameraUpVector parameters. */
	matView = lookAt(cameraPosition, cameraTarget, cameraUpVector);
	glUniformMatrix4fv(locationMatView, 1, GL_FALSE, value_ptr(matView));
}

void display(GLFWwindow* window, double currentTime) {
	/** Töröljük le a kiválasztott buffereket! Fontos lehet minden egyes alkalommal törölni! */
	/** Let's clear the selected buffers! Usually importand to clear it each time! */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	static GLdouble	lastFrame = 0.0f;						// static initalized once to zero, time stamp of last frame
	GLdouble		deltaTime = currentTime - lastFrame;	// time between current frame and last frame
	GLfloat			cameraSpeed = 5.0f * deltaTime;			// cameraSpeed proportional to deltaTime

	lastFrame = currentTime;
	/** A kamera mozgatása WASD-vel és nyilakkal. */
	/** Moving camera by WASD or arrows. */
	if ((keyboard[GLFW_KEY_W]) || (keyboard[GLFW_KEY_UP]))
		cameraPosition += cameraSpeed * cameraMovingY;
	if ((keyboard[GLFW_KEY_A]) || (keyboard[GLFW_KEY_LEFT]))
		cameraPosition += cameraSpeed * cameraMovingX;
	if ((keyboard[GLFW_KEY_S]) || (keyboard[GLFW_KEY_DOWN]))
		cameraPosition -= cameraSpeed * cameraMovingY;
	if ((keyboard[GLFW_KEY_D]) || (keyboard[GLFW_KEY_RIGHT]))
		cameraPosition -= cameraSpeed * cameraMovingX;
	/** A fényforrás a kamerában van, olyan lesz, mint egy fejlámpa. */
	/** Light follow the user viewing the scene, behaves as a headlamp. */
	lightPosition = cameraPosition;
	computeModelMatrix();
	computeCameraMatrix();
	/** Kiszámolt adatok átadása a shadereknek. */
	/** Pass parameters to the shaders. */
	inverseTransposeMatrix = mat3(inverseTranspose(matModel));
	glUniformMatrix3fv(inverseTransposeMatrixLoc, 1, GL_FALSE, value_ptr(inverseTransposeMatrix));
	glUniform3fv(lightPositionLoc, 1, value_ptr(lightPosition));
	glUniform3fv(cameraPositionLoc, 1, value_ptr(cameraPosition));
	/** A téglatest megrajzolása. */
	/** Draw the brick. */
	glDrawArrays(GL_TRIANGLES, 0, sizeof(verticesData) / (9 * 4)); // 9 component, 4 bytes = sizeof(GLfloat)
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	/** A minimalizálás nem fog fagyni a minimum 1 értékkel. */
	/** Minimize will not freeze with minimum value 1. */
	windowWidth = glm::max(width, 1);
	windowHeight = glm::max(height, 1);

	GLfloat aspectRatio = (GLfloat)windowWidth / (GLfloat)windowHeight;
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
	/** Uniform változók beállítása. */
	/** Setup uniform variables. */
	glUniformMatrix4fv(locationMatProjection, 1, GL_FALSE, value_ptr(matProjection));
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	modifiers = mods;
	/** ESC billentyûre kilépés. */
	/** Exit on ESC key. */
	if ((action == GLFW_PRESS) && (key == GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, GLFW_TRUE);
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
}

void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
}

int main(void) {
	/** Az alkalmazáshoz kapcsolódó elõkészítõ lépések (paraméterek: major, minor OpenGL verziók, OpenGL pofile). */
	/** The first initialization steps of the program (params: major, minor OpenGL versions, OpenGL pofile). */
	init(3, 3, GLFW_OPENGL_COMPAT_PROFILE);
	/** A shader programok betöltése. */
	/** Loading the shader programs for rendering. */
	initShaderProgram();
	/** A window elkészítése után a képernyő beállítása. */
	/** Setup screen after window creation. */
	framebufferSizeCallback(window, windowWidth, windowHeight);
	/** Karakterkódolás a szövegekhez. */
	/** Setting locale for characters of texts. */
	setlocale(LC_ALL, "hu_HU");
	cout << "Materials and Z-buffer" << endl;
	cout << "https://www.khronos.org/opengl/wiki/how_lighting_works" << endl;
	cout << "https://learnopengl.com/Lighting/Materials" << endl << endl;
	cout << "Keyboard control" << endl;
	cout << "ESC\t\texit" << endl;
	cout << "O\t\tinduces orthographic projection" << endl;
	cout << "P\t\tinduces perspective projection" << endl;
	cout << "WASD and arrows\tcamera control" << endl << endl;
	cout << "Weekly tasks" << endl;
	cout << "Gyakorlat 1: Adjunk a shader kódhoz kibocsátott fényt az anyagoknál!" << endl;
	cout << "Gyakorlat 2: Adjuk át uniform tömbbel a material szín komponenseit a shadernek!" << endl;
	cout << "Gyakorlat 3: Tegyük kezelhetővé az OpenGL szabvány legalább 8 független fényforrását anyagokkal együtt!" << endl;
	cout << "Activity  1: Add emitted light for materials at the shader code!" << endl;
	cout << "Activity  2: Pass the material components as uniform array to the shader code!" << endl;
	cout << "Activity  3: Make work at least 8 independent light sources with materials according to the OpenGL standard!" << endl;
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