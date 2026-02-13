enum eVertexArrayObject {
	VAOQuadScreenVertices,
	VAOCount
};
enum eVertexBufferObject {
	VBOQuadScreenVertices,
	BOCount
};
enum eProgram {
	ColorShiftComputeProgram,
	QuadScreenProgram,
	ProgramCount
};
enum eTexture {
	ColorShiftTexture,
	TextureCount
};

#include <common.cpp>

GLchar			windowTitle[] = "Color Shift with Compute Shader";
/** Az 1024-es textúra méret illeszkedik a GPU felépítéséhez. */
/** Texture size 1024 fits the GPU native structure. */
const GLuint	TEXTURE_WIDTH = 1024, TEXTURE_HEIGHT = 1024;
/** A shader változók location-jei és a CPU oldali változóik. */
/** Shader variable locations and the corresponding CPU variables. */
GLuint			locationTime;

GLfloat quadScreenVertices[] = {
	// positions  // texture coords
	-1.0f,  1.0f, 0.0f, 1.0f,
	-1.0f, -1.0f, 0.0f, 0.0f,
	 1.0f,  1.0f, 1.0f, 1.0f,
	 1.0f, -1.0f, 1.0f, 0.0f,
};

void initComputeShader() {
	ShaderInfo shader_info[] = {
		{ GL_COMPUTE_SHADER, "./ColorShiftComp.glsl" },
		{ GL_NONE, nullptr }
	};

	program[ColorShiftComputeProgram] = LoadShaders(shader_info);
	glUseProgram(program[ColorShiftComputeProgram]);
	/** Shader változók beállítása. */
	/** Shader variable setting. */
	locationTime = glGetUniformLocation(program[ColorShiftComputeProgram], "time");
	glUniform1i(glGetUniformLocation(program[ColorShiftComputeProgram], "textureSampler2D"), 0);
	/** A texture objektum generálása és beállítása. */
	/** Generate and setup texture object. */
	glGenTextures(TextureCount, texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[ColorShiftTexture]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
	glBindImageTexture(0, texture[ColorShiftTexture], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[ColorShiftTexture]);
}

void initShaderProgram() {
	ShaderInfo shader_info[] = {
		{ GL_VERTEX_SHADER, "./QuadScreenVert.glsl" },
		{ GL_FRAGMENT_SHADER, "./QuadScreenFrag.glsl" },
		{ GL_NONE, nullptr }
	};

	program[QuadScreenProgram] = LoadShaders(shader_info);
	/** Shader változó location lekérdezése. */
	/** Getting shader variable location. */
	locationMatProjection = glGetUniformLocation(program[QuadScreenProgram], "matProjection");
	locationMatModelView = glGetUniformLocation(program[QuadScreenProgram], "matModelView");
	/** Csatoljuk a vertex array objektumunkat a shader programhoz. */
	/** Attach the vertex array object to the shader program. */
	glBindVertexArray(VAO[VAOQuadScreenVertices]);
	/** A GL_ARRAY_BUFFER nevesített csatolóponthoz kapcsoljuk a vertex buffert (ide kerülnek a csúcspont adatok). */
	/** We attach the vertex buffer to the GL_ARRAY_BUFFER node (vertices are stored here). */
	glBindBuffer(GL_ARRAY_BUFFER, BO[VBOQuadScreenVertices]);
	/** Másoljuk az adatokat a bufferbe! Megadjuk az aktuálisan csatolt buffert, azt hogy hány byte adatot másolunk,
	a másolandó adatot, majd a feldolgozás módját is meghatározzuk: most az adat nem változik a feltöltés után. */
	/** Copy the data to the buffer! First parameter is the currently attached buffer, second is the size of the buffer to be copied,
	third is the array of data, fourth is working mode: now the data can not be modified after this step. */
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadScreenVertices), &quadScreenVertices, GL_STATIC_DRAW);
	/** Engedélyezzük az imént definiált location = 0 attribútumot (vertexShader.glsl). */
	/** Enable the previously defined location = 0 attributum (vertexShader.glsl). */
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
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	/** Ezen adatok szolgálják a location = 1 textúra koordináta attribútumot. */
	/** These values are for location = 1 texture coordinate attribute. */
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
}

void display(GLFWwindow* window, GLdouble currentTime) {
	static GLint	frameCounter	= 0;						// static initalized once to zero, frame counter
	static GLdouble	lastFrame		= 0.0f;						// static initalized once to zero, time stamp of last frame
	GLdouble		deltaTime		= currentTime - lastFrame;	// time between current frame and last frame
	/** Nincs buffer törlés a teljes újrarajzolás miatt. */
	/** No buffer clearing, because full redraw. */
	if (frameCounter > 10)
		cout << "\rFPS: " << 1.0f / deltaTime,					// print statistics
		frameCounter = 0;
	else 
		frameCounter++;
	lastFrame = currentTime;
	/** Compute program bekapcsolása és a feladat elindítása 32x32-es csoportokban. */
	/** Switch on compute program and start it with 32x32 group sizes. */
	glUseProgram(program[ColorShiftComputeProgram]);
	glUniform1f(locationTime, (GLfloat)currentTime);
	glDispatchCompute((unsigned int)TEXTURE_WIDTH / 32, (unsigned int)TEXTURE_HEIGHT / 32, 1);
	/** Kiolvasás előtt megvárjuk a feladat befejezését. */
	/** Make sure writing to image has finished before read. */
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	/** A képet a képernyőre másoljuk, hogy megtekintsük. */
	/** Render image to quad in order to see it. */
	glUseProgram(program[QuadScreenProgram]);
	/** 1x1 XY koordinátás négyzetet rajzolunk (Normalized Device Coordinates). */
	/** Renders a 1x1 XY quad in NDC (Normalized Device Coordinates). */
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	/** A minimalizálás nem fog fagyni a minimum 1 értékkel. */
	/** Minimize will not freeze with minimum value 1. */
	windowWidth = glm::max(width, 1);
	windowHeight = glm::max(height, 1);

	GLdouble aspectRatio = (GLdouble)windowWidth / windowHeight;
	/** A kezelt képernyõ beállítása a teljes (0, 0, width, height) területre. */
	/** Set the viewport for the full (0, 0, width, height) area. */
	glViewport(0, 0, windowWidth, windowHeight);
	/** Orthographic projekció beállítása, worldSize lesz a szélesség és magasság közül a kisebbik. */
	/** Set up orthographic projection, worldSize will equal the smaller value of width or height. */
	if (windowWidth < windowHeight)
		matProjection = ortho(-worldSize, worldSize, -worldSize / aspectRatio, worldSize / aspectRatio, -1.0, 1.0);
	else
		matProjection = ortho(-worldSize * aspectRatio, worldSize * aspectRatio, -worldSize, worldSize, -1.0, 1.0);
	matModelView = mat4(1.0);
	/** Uniform változók beállítása. */
	/** Setup uniform variables. */
	glUseProgram(program[QuadScreenProgram]);
	glUniformMatrix4fv(locationMatProjection, 1, GL_FALSE, value_ptr(matProjection));
	glUniformMatrix4fv(locationMatModelView, 1, GL_FALSE, value_ptr(matModelView));
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
	init(4, 3, GLFW_OPENGL_CORE_PROFILE);	// Compute Shader core since version 4.3
	/** A shader programok betöltése. */
	/** Loading the shader programs for rendering. */
	initComputeShader();
	initShaderProgram();
	/** A window elkészítése után a képerny? beállítása. */
	/** Setup screen after window creation. */
	framebufferSizeCallback(window, windowWidth, windowHeight);
	/** Karakterkódolás a szövegekhez. */
	/** Setting locale for characters of texts. */
	setlocale(LC_ALL, "hu_HU");
	cout << "Color Shift with Compute Shader" << endl << endl;
	cout << "Keyboard control" << endl;
	cout << "ESC\t\texit" << endl << endl;
	cout << "Weekly tasks" << endl;
	cout << "Gyakorlat 1: Vizualizáljuk a Mandelbrot halmazt!" << endl;
	cout << "Gyakorlat 2: Implementálunk nagyítást! (+-)" << endl;
	cout << "Gyakorlat 3: 2x nagyításkor és kicsinyítéskor hasznosítsuk újra a kép negyedét!" << endl;
	cout << "Activity  1: Let's visualize the Mandelbrot set!" << endl;
	cout << "Activity  2: Implement zoom! (+-)" << endl;
	cout << "Activity  3: By 2x zoom in or out reuse the quarter of the existing image!" << endl << endl;
	/** A GPU limitációninak lekérdezése. */
	/** Query limitations of the GPU. */
	GLint	max_compute_work_group_count[3];
	GLint	max_compute_work_group_size[3];
	GLint	max_compute_work_group_invocations;
	GLchar	dimensions[] = "XYZ";

	cout << "OpenGL implementation limitations" << endl;
	for (int idx = 0; idx < 3; idx++) {
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, idx, &max_compute_work_group_count[idx]);
		cout << "Maximum number of work groups in " << dimensions[idx] << " dimension:\t\t\t\t\t\t\t" << max_compute_work_group_count[idx] << endl;
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, idx, &max_compute_work_group_size[idx]);
		cout << "Maximum size of a work group in " << dimensions[idx] << " dimension:\t\t\t\t\t\t\t" << max_compute_work_group_size[idx] << endl;
	}
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &max_compute_work_group_invocations);
	cout << "Number of invocations in a single local work group that may be dispatched to a compute shader:\t" << max_compute_work_group_invocations << endl;
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