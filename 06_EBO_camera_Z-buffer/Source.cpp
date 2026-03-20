enum eVertexArrayObject {
	VAOVerticesData,
	VAOCount
};
enum eBufferObject {
	VBOVerticesData,
	EBOIndices,
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
enum eEBOvsIndices {
	EBO,			// EBO holds indices in GPU memory
	Indices			// indices are in CPU memory array
};

#include <common.cpp>

GLchar			windowTitle[] = "Camera with EBO and Z-buffer";
/** A shader változók location-jei és a CPU oldali változóik. */
/** Shader variable locations and the corresponding CPU variables. */
eEBOvsIndices	glDrawElementsMode = EBO;
GLuint			transformLoc;
GLdouble		deltaTime, lastTime = glfwGetTime();
GLfloat			cameraSpeed;
/* Vegyük fel a kamera pozicót tároló változót, illetve a tengelyekhezz szükséges vektorokat. */
vec3			cameraPosition	= vec3(0.0f,  0.0f,  3.0f),
				cameraDirection	= vec3(0.0f,  0.0f, -1.0f), // direction for camera
				cameraUpVector	= vec3(0.0f,  1.0f,  0.0f),
				cameraTarget;
/* Tömb a kocka pontjainak és azok színeinek tárolásához. */
/* Array for storing the points and colors of a cube. */
static array<GLfloat, 6> verticesData[8] = {
	//position        //colors
	 1.0,  1.0, -1.0, 0.0, 0.0, 0.0,
	-1.0,  1.0, -1.0, 1.0, 1.0, 0.0,
	 1.0, -1.0, -1.0, 0.0, 1.0, 1.0,
	-1.0, -1.0, -1.0, 1.0, 0.0, 1.0,
	 1.0,  1.0,  1.0, 0.0, 1.0, 0.0,
	-1.0,  1.0,  1.0, 1.0, 0.0, 0.0,
	-1.0, -1.0,  1.0, 1.0, 1.0, 1.0,
	 1.0, -1.0,  1.0, 0.0, 0.0, 1.0
};
// Draw order for vertices in GL_TRIANGLE_STRIP mode.
GLuint vertexDrawIndices[14] = {  
	3, 2, 6, 7, 4, 2, 0, 3, 1, 6, 5, 4, 1, 0
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
	/** A GL_ELEMENT_ARRAY_BUFFER nevesített csatolóponthoz kapcsoljuk az element array buffert (ide kerülnek a csúcspontok indexei). */
	/** We attach the element array buffer to the GL_ELEMENT_ARRAY_BUFFER node (indices of vertices are stored here). */
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BO[EBOIndices]);
	/** Másoljuk az adatokat a bufferbe! Megadjuk az aktuálisan csatolt buffert, azt hogy hány byte adatot másolunk,
		a másolandó adatot, majd a feldolgozás módját is meghatározzuk: most az adat nem változik a feltöltés után. */
	/** Copy the data to the buffer! First parameter is the currently attached buffer, second is the size of the buffer to be copied,
		third is the array of data, fourth is working mode: now the data can not be modified after this step. */
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertexDrawIndices), vertexDrawIndices, GL_STATIC_DRAW);
	/** A GL_ARRAY_BUFFER nevesített csatolóponthoz kapcsoljuk a vertex buffert (ide kerülnek a csúcspont adatok). */
	/** We attach the vertex buffer to the GL_ARRAY_BUFFER node (vertices are stored here). */
	glBindBuffer(GL_ARRAY_BUFFER, BO[VBOVerticesData]);
	/** Másoljuk az adatokat a bufferbe! Megadjuk az aktuálisan csatolt buffert, azt hogy hány byte adatot másolunk,
		a másolandó adatot, majd a feldolgozás módját is meghatározzuk: most az adat nem változik a feltöltés után. */
	/** Copy the data to the buffer! First parameter is the currently attached buffer, second is the size of the buffer to be copied,
		third is the array of data, fourth is working mode: now the data can not be modified after this step. */
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesData), &verticesData, GL_STATIC_DRAW);
	/** Engedélyezzük az imént definiált location = 0 attribútumot (vertexShader.glsl ). */
	/** Enable the previously defined location = 0 attributum (vertexShader.glsl ). */
	glEnableVertexAttribArray(0);
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
	/** Ezen adatok szolgálják a location = 1 color attribútumot. */
	/** These values are for location = 1 color attribute. */
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	/** Drótvázmodell, mutatjuk a hátsó oldalakat is. */
	/** Wireframe drawing showing even backsides. */
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	/** Mélységbuffer bekapcsolása a helyes láthatóságért. */
	/** Enable depth test for visual ordering. */
	glEnable(GL_DEPTH_TEST);
	/** Az aktuális példaprogram bekapcsolása. */
	/** Switch on the current sample program. */
	glUseProgram(program[QuadScreenProgram]);
	/** Mátrixok kezdőértékének beállítása. */
	/** Set the matrices to the initial values. */
	matModel = mat4(1.0);
	matView = lookAt(
		vec3(0.0f, 0.0f, 9.0f),		// the position of your camera, in world space
		vec3(0.0f, 0.0f, 0.0f),		// where you want to look at, in world space
		vec3(0.0f, 1.0f, 0.0f));	// upVector, probably glm::vec3(0,1,0), but (0,-1,0) would make you looking upside-down, which can be great too
	glUniformMatrix4fv(locationMatModel, 1, GL_FALSE, value_ptr(matModel));
	glUniformMatrix4fv(locationMatView, 1, GL_FALSE, value_ptr(matView));
	glUniformMatrix4fv(locationMatProjection, 1, GL_FALSE, value_ptr(matProjection));
	projectionType = Perspective;
}
/** Kiszámoljuk a kamera mátrixot. */
/** Compute the camera matrix. */
void computeCameraMatrix() {
	cameraTarget = cameraPosition + cameraDirection;
	/** A paraméterek rendre: az új koordinátarendszerünk középpontja (pont),
		mit néz a kamera (pont), mit tekintünk felfele iránynak (vektor). */
	/** Parameters: new origo for coordinate system (point),
		position what we look at (point), up direction of camera (vector). */
	matView = lookAt(cameraPosition, cameraTarget, cameraUpVector);
	glUniformMatrix4fv(locationMatView, 1, GL_FALSE, value_ptr(matView));
}

void display(GLFWwindow* window, double currentTime) {
	/** Töröljük le a kiválasztott buffereket! Fontos lehet minden egyes alkalommal törölni! */
	/** Let's clear the selected buffers! Usually importand to clear it each time! */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/** Időarányos kompenzáció, hogy az animáció akadásmentes legyen! */
	/** Let's compensate for the different time that is needed for calculating a frame! */
	deltaTime	= currentTime - lastTime;
	lastTime	= currentTime;
	/** Folyamatos lesz az animáció, ha a kamera sebességét arányosítjuk az eltelt idővel. */
	/** This makes the animation continous, because camera speed will be proportional with the elapsed time. */
	cameraSpeed = 2.5f * (GLfloat)deltaTime;
	/** Két készletet használunk a mozgásra: WASD és a nyíl billentyűk. */
	/** We will use two set ups for moving: WASD and the arrow keys. */
	if ((keyboard[GLFW_KEY_W]) || (keyboard[GLFW_KEY_UP]))
		cameraPosition += cameraSpeed * cameraDirection;
	if ((keyboard[GLFW_KEY_S]) || (keyboard[GLFW_KEY_DOWN]))
		cameraPosition -= cameraSpeed * cameraDirection;
	if ((keyboard[GLFW_KEY_A]) || (keyboard[GLFW_KEY_LEFT]))
		cameraPosition -= cameraSpeed * normalize(cross(cameraDirection, cameraUpVector)); // compute side moving vector
	if ((keyboard[GLFW_KEY_D]) || (keyboard[GLFW_KEY_RIGHT]))
		cameraPosition += cameraSpeed * normalize(cross(cameraDirection, cameraUpVector)); // compute side moving vector
	if ((keyboard[GLFW_KEY_Q]))
		cameraPosition += cameraSpeed * cameraUpVector;
	if ((keyboard[GLFW_KEY_R]))
		cameraPosition -= cameraSpeed * cameraUpVector;
	computeCameraMatrix();
	/** A megadott adatok segítségével megrajzoljuk az objektumot. */
	/** We draw the object with the defined array. */
	if (glDrawElementsMode == EBO) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BO[EBOIndices]);
		glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_INT, nullptr);
		cout << "E";
	} else { // glDrawElementsMode == Indices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_INT, vertexDrawIndices);
		cout << "I";
	}
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
	/** Uniform változók beállítása. */
	/** Setup uniform variables. */
	glUniformMatrix4fv(locationMatProjection, 1, GL_FALSE, value_ptr(matProjection));
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
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
	/** Az E billentyû lenyomására EBO-ból szedjük az indexeket GPU memóriából közvetlenül. */
	/** Pressing E makes EBO to hold indices in GPU memory directly. */
	if (key == GLFW_KEY_E && action == GLFW_PRESS)
		glDrawElementsMode = EBO;
	/** Az I billentyû lenyomására CPU memóriából jönnek az indexek buszon utaztatva. */
	/** Pressing I indices are in CPU memory array and travel on inner buses each time used. */
	if (key == GLFW_KEY_I && action == GLFW_PRESS)
		glDrawElementsMode = Indices;
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
	init(3, 3, GLFW_OPENGL_CORE_PROFILE);
	/** A shader programok betöltése. */
	/** Loading the shader programs for rendering. */
	initShaderProgram();
	/** A window elkészítése után a képernyő beállítása. */
	/** Setup screen after window creation. */
	framebufferSizeCallback(window, windowWidth, windowHeight);
	/** Karakterkódolás a szövegekhez. */
	/** Setting locale for characters of texts. */
	setlocale(LC_ALL, "");
	cout << "EBO with camera [lookAt()] usage having Z-buffer" << endl;
	cout << "https://stackoverflow.com/questions/60537484/can-i-texture-this-cube-that-is-rendered-in-gl-triangle-strip-mode" << endl;
	cout << "http://www.songho.ca/opengl/gl_vertexarray.html" << endl;
	cout << "https://stackoverflow.com/questions/33863426/vaos-and-element-buffer-objects" << endl;
	cout << "https://github.com/JoeyDeVries/LearnOpenGL/blob/bc41d2c0192220fb146c5eabf05f3d8317851200/src/1.getting_started/5.1.transformations/transformations.cpp" << endl;
	cout << "https://learnopengl.com/Code-repository" << endl << endl;
	cout << "Keyboard control" << endl;
	cout << "ESC\t\texit" << endl;
	cout << "O\t\tinduces orthographic projection" << endl;
	cout << "P\t\tinduces perspective projection" << endl;
	cout << "E\t\tEBO holds indices in GPU memory" << endl;
	cout << "I\t\tindices are in CPU memory array" << endl;
	cout << "WASD and arrows\tcamera control" << endl << endl;
	cout << "Weekly tasks" << endl;
	cout << "Gyakorlat 1: Csináljuk szilárdtest modellre a kockát!" << endl;
	cout << "Gyakorlat 2: Oldjuk meg a függőleges mozgást!" << endl;
	cout << "Gyakorlat 3: Cseréljük a kockát tetraéderre!" << endl;
	cout << "Activity  1: Change wireframe to solid model!" << endl;
	cout << "Activity  2: Implement vertical movement!" << endl;
	cout << "Activity  3: Change cube to tetrahedron!" << endl;
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