enum eVertexArrayObject {
	VAOVerticesData,
	VAOCount
};
enum eBufferObject {
	VBOVerticesData,
	BOCount
};
enum eProgram {
	SubdivisionProgram,
	BuildHouseGeometryProgram,
	ProgramCount
};
enum eTexture {
	NoTexture,		// fixes 0 sized array problem
	TextureCount
};

#include <common.cpp>

GLchar			windowTitle[] = "Subdivision or Houses with Geometry Shader";
/** A shader változók location-jei és a CPU oldali változóik. */
/** Shader variable locations and the corresponding CPU variables. */
GLint			locationGeometryDetail;
GLint			geometryDetail = 0;
eProgram		sampleProgram = SubdivisionProgram;
/** std::array használható a csúcspontok tárolásához. */
/** std::array can be used to store vertices. */
static array<GLfloat, 5> verticesData[4] = {
	// positions  // color
	-0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // top-left
	 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // top-right
	 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom-right
	-0.5f, -0.5f, 1.0f, 1.0f, 0.0f  // bottom-left
};

void initGeometryShader() {
	ShaderInfo shader_info[ProgramCount][4] = { {
		{ GL_GEOMETRY_SHADER,	"./SubdivisionGeom.glsl" },
		{ GL_VERTEX_SHADER,		"./GeometryVert.glsl" },
		{ GL_FRAGMENT_SHADER,	"./GeometryFrag.glsl" },
		{ GL_NONE, nullptr } },
		{
		{ GL_GEOMETRY_SHADER,	"./BuildHouseGeom.glsl" },
		{ GL_VERTEX_SHADER,		"./GeometryVert.glsl" },
		{ GL_FRAGMENT_SHADER,	"./GeometryFrag.glsl" },
		{ GL_NONE, nullptr } } };

	for (int programItem = 0; programItem < ProgramCount; programItem++) {
		program[programItem] = LoadShaders(shader_info[programItem]);
		/** Shader változó location lekérdezése. */
		/** Getting shader variable location. */
		locationMatProjection = glGetUniformLocation(program[programItem], "matProjection");
		locationMatModelView = glGetUniformLocation(program[programItem], "matModelView");
		locationGeometryDetail = glGetUniformLocation(program[programItem], "geometryDetail"); 
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesData), &verticesData, GL_STATIC_DRAW);
	/** Engedélyezzük az imént definiált location = 0 attribútumot (BuildHouseVert.glsl). */
	/** Enable the previously defined location = 0 attributum (BuildHouseVert.glsl). */
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
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
	/** Ezen adatok szolgálják a location = 1 color attribútumot. */
	/** These values are for location = 1 color attribute. */
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
}

void display(GLFWwindow* window, GLdouble currentTime) {
	glClear(GL_COLOR_BUFFER_BIT);
	switch (sampleProgram) {
	/** Kör rajzolása háromszögből kiindulva subdivision segítségével. */
	/** Draw circle starting from a triangle using subdivision. */
	case SubdivisionProgram:
		glDrawArrays(GL_TRIANGLES, 0, 3);
		break;
	/** Ház(ak) rajzolása spirálisan 4 helyről kiindulva. */
	/** Draw house(s) in a spiral arrangement starting from 4 positions. */
	case BuildHouseGeometryProgram:
		glDrawArrays(GL_POINTS, 0, 4);
		break;
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
	matModel = mat4(1.0);
	matView = lookAt(
		vec3(0.0f, 0.0f, 9.0f),		// the position of your camera, in world space
		vec3(0.0f, 0.0f, 0.0f),		// where you want to look at, in world space
		vec3(0.0f, 1.0f, 0.0f));	// upVector, probably glm::vec3(0,1,0), but (0,-1,0) would make you looking upside-down, which can be great too
	matModelView = matView * matModel;
	/** Az aktuális példaprogram bekapcsolása. */
	/** Switch on the current sample program. */
	glUseProgram(program[sampleProgram]);
	/** Uniform változók beállítása. */
	/** Setup uniform variables. */
	glUniformMatrix4fv(locationMatProjection, 1, GL_FALSE, value_ptr(matProjection));
	glUniformMatrix4fv(locationMatModelView, 1, GL_FALSE, value_ptr(matModelView));
	glUniform1i(locationGeometryDetail, geometryDetail);
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

	if ((action == GLFW_PRESS) && ((key == GLFW_KEY_KP_ADD) || (key == GLFW_KEY_EQUAL))) geometryDetail++;
	if ((action == GLFW_PRESS) && ((key == GLFW_KEY_KP_SUBTRACT) || (key == GLFW_KEY_MINUS)) && (geometryDetail > 0)) geometryDetail--;
	if ((action == GLFW_PRESS) && (key == GLFW_KEY_S)) sampleProgram = SubdivisionProgram;
	if ((action == GLFW_PRESS) && (key == GLFW_KEY_H)) sampleProgram = BuildHouseGeometryProgram;
	/** Az aktuális példaprogram bekapcsolása. */
	/** Switch on the current sample program. */
	glUseProgram(program[sampleProgram]);
	/** Uniform változók beállítása. */
	/** Setup uniform variables. */
	glUniformMatrix4fv(locationMatProjection, 1, GL_FALSE, glm::value_ptr(matProjection));
	glUniformMatrix4fv(locationMatModelView, 1, GL_FALSE, glm::value_ptr(matModelView));
	glUniform1i(locationGeometryDetail, geometryDetail);
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
	init(3, 3, GLFW_OPENGL_CORE_PROFILE);	// Geometry Shader core since version 3.2, 3.3 needed for in-out system
	/** A shader programok betöltése. */
	/** Loading the shader programs for rendering. */
	initGeometryShader();
	/** A window elkészítése után a képernyő beállítása. */
	/** Setup screen after window creation. */
	framebufferSizeCallback(window, windowWidth, windowHeight);
	/** Karakterkódolás a szövegekhez. */
	/** Setting locale for characters of texts. */
	setlocale(LC_ALL, "");
	cout << "Geometry shaders for subdivision and drawing houses" << endl;
	cout << "https://www.khronos.org/opengl/wiki/Geometry_Shader" << endl;
	cout << "https://www.khronos.org/opengl/wiki/Tutorial4:_Using_Indices_and_Geometry_Shaders_(C_/SDL)" << endl;
	cout << "https://math.stackexchange.com/questions/2297213/finding-center-and-radius-of-circumscribed-triangle" << endl;
	cout << "https://learnopengl.com/Advanced-OpenGL/Geometry-Shader" << endl << endl;
	cout << "Keyboard control" << endl;
	cout << "ESC\texit" << endl;
	cout << "O\tinduces orthographic projection" << endl;
	cout << "P\tinduces perspective projection" << endl;
	cout << "S\tsubdivision for circle" << endl;
	cout << "H\thouses in spiral arrengement" << endl;
	cout << "+\tincrease geometry detail" << endl;
	cout << "-\tdecrease geometry detail" << endl << endl;
	cout << "Weekly tasks" << endl;
	cout << "Gyakorlat 1: Cseréljük a triangle strip-et points-ra a házaknál!" << endl;
	cout << "Gyakorlat 2: Cseréljük a triangle strip-et line strip-re a házaknál!" << endl;
	cout << "Gyakorlat 3: Készítsünk egy bemeneti pontból egy kört triangle strip-el!" << endl;
	cout << "Activity  1: Change triangle strip to points at the houses!" << endl;
	cout << "Activity  2: Change triangle strip to line strip at the houses!" << endl;
	cout << "Activity  3: Create a circle using triangle strip for one input point!" << endl;
	/** A GPU limitációninak lekérdezése. */
	/** Query limitations of the GPU. */
	cout << endl << "Limitations of the GPU:" << endl;
	GLint	param;
	glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES, &param);
	cout << "GL_MAX_GEOMETRY_OUTPUT_VERTICES:\t\t" << param << endl;
	glGetIntegerv(GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS, &param);
	cout << "GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS:\t" << param << endl;
	glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_COMPONENTS, &param);
	cout << "GL_MAX_GEOMETRY_OUTPUT_COMPONENTS :\t\t" << param << endl;
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