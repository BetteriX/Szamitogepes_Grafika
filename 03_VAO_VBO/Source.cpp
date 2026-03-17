enum eVertexArrayObject {
	VAOVerticesData,
	VAOCount
};
enum eBufferObject {
	VBOVerticesData,	
	VBOColorsData,
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

#include <common.cpp>

/** std::array használható a glm::vec2 csúcspontok tárolásához. */
/** std::array can be used to store glm::vec2 vertices. */
/*
static array<vec2, 5> vertices = {
	//     X      Y
	vec2(-0.5f, -0.5f),
	vec2( 0.5f, -0.5f),
	vec2( 0.0f,  0.5f),
	vec2(-0.7f,  0.7f),
	vec2(-0.7f,  -0.7f),
};
*/

//static array<vec2, 362> vertices;
//static array<vec3, 362> colors;

static array<vec2, 20> vertices = {
	vec2(-1.0, -1.0),
	vec2(-1.0, 0.8),
	vec2(1.0, 0.8),
	vec2(-1.0, -1.0),
	vec2(1.0, -1.0),
	vec2(1.0, 0.8),

	vec2(-1.0, 0.8),
	vec2(0.0, 1.0),
	vec2(1.0, 0.8),

	vec2(0.2, -1),
	vec2(0.2, 0.5),
	vec2(0.0, 0.5),
	vec2(0.8, -1),
	vec2(0.8, 0.5),

	vec2(-0.8, -0.2),
	vec2(-0.8, 0.7),
	vec2(-0.1, 0.7),
	vec2(-0.8, -0.2),
	vec2(-0.1, -0.2),
	vec2(-0.1, 0.7),
};

static array<vec3, 20> colors = {
	vec3(1.0, 1.0, 0.0),
	vec3(1.0, 1.0, 0.0),
	vec3(1.0, 1.0, 0.0),
	vec3(1.0, 1.0, 0.0),
	vec3(1.0, 1.0, 0.0),
	vec3(1.0, 1.0, 0.0),

	vec3(1.0, 0.0, 0.0),
	vec3(1.0, 0.0, 0.0),
	vec3(1.0, 0.0, 0.0),

	vec3(1.0, 1.0, 0.0),
	vec3(1.0, 1.0, 0.0),
	vec3(1.0, 1.0, 0.0),
	vec3(1.0, 1.0, 0.0),
	vec3(1.0, 1.0, 0.0),

	vec3(1.0, 0.0, 1.0),
	vec3(1.0, 0.0, 1.0),
	vec3(1.0, 0.0, 1.0),
	vec3(1.0, 0.0, 1.0),
	vec3(1.0, 0.0, 1.0),
	vec3(1.0, 0.0, 1.0),
};


GLchar		windowTitle[]	= "VAO and VBO";

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
	/** Copy the data to t
	he buffer! First parameter is the currently attached buffer, second is the size of the buffer to be copied,
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
	/** Aktiváljuk a shader-program objektumunkat az alapértelmezett fix csõvezeték helyett. */
	/** Activate our shader-program object instead of the default fix pipeline. */
	glUseProgram(program[QuadScreenProgram]);

	glBindBuffer(GL_ARRAY_BUFFER, BO[VBOColorsData]);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec3), colors.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
}

void display(GLFWwindow* window, double currentTime) {
	/* Töröljük le a kiválasztott buffereket! */
	/* Let's clear the selected buffers! */
	glClear(GL_COLOR_BUFFER_BIT);
	/** A megadott adatok segítségével pontokat rajzolunk. */
	/** We draw points with the defined arrays. */
	//glDrawArrays(GL_POINTS, 0, 5);
	/** A megadott adatok segítségével háromszöget rajzolunk. */
	/** We draw triangle with the defined arrays. */
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	//glDrawArrays(GL_LINES, 3, 2);

	//glDrawArrays(GL_TRIANGLE_FAN, 0, 362);
	glDrawArrays(GL_TRIANGLES, 0, 20);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	/** A minimalizálás nem fog fagyni a minimum 1 értékkel. */
	/** Minimize will not freeze with minimum value 1. */
	windowWidth = glm::max(width, 1);
	windowHeight = glm::max(height, 1);
	/** A kezelt képernyõ beállítása a teljes (0, 0, width, height) területre. */
	/** Set the viewport for the full (0, 0, width, height) area. */
	glViewport(0, 0, windowWidth, windowHeight);
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

	//vertices[0] = vec2(0.0f, 0.0f);
	//colors[0] = vec3(0.0f, 0.0f, 1.0f);
	
	//init(3, 3, GLFW_OPENGL_CORE_PROFILE);	// Makes big points square instead of circle.
	/** A rajzoláshoz használt shader programok betöltése. */
	/** Loading the shader programs for rendering. */
	/*
	for (int i = 0; i <= 360; i++) {
		float angle = (2 * M_PI * i) / 360;
		float x = cos(angle);
		float y = sin(angle);
		vertices[i+1] = vec2(x, y);
		colors[i+1] = vec3(0.0f, 1.0f, 0.0f);
	}
	*/
	initShaderProgram();
	/** Karakterkódolás a szövegekhez. */
	/** Setting locale for characters of texts. */
	setlocale(LC_ALL, "");
	cout << "VAO (Vertex Array Object) and VBO (Vertex Buffer Object) usage" << endl;
	cout << "Special attention to (VNCVNCVNCVNC) interleaved structure" << endl;	
	cout << "https://www.khronos.org/opengl/wiki/Vertex_Specification_Best_Practices#Formatting_VBO_Data" << endl << endl;
	cout << "Keyboard control" << endl;
	cout << "ESC\texit" << endl << endl;
	cout << "Weekly tasks (https://www.khronos.org/opengl/wiki/primitive)" << endl;
	cout << "Gyakorlat 1: Készítsünk színátmenetes kört triangle fan segítségével!" << endl;
	cout << "Gyakorlat 2: Vegyünk fel std:array tömböt a színeknek!" << endl;
	cout << "Gyakorlat 3: Vegyünk fel második VBO-t a színeknek és használjuk!" << endl;
	cout << "Activity  1: Create circle by using a triangle fan having color transition!" << endl;
	cout << "Activity  2: Create a std:array for colors!" << endl;
	cout << "Activity  3: Define a second VBO for the colors and use it!" << endl;
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