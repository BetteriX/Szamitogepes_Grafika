enum eVertexArrayObject {
	VAOVerticesData,
	VAOCount
};
enum eBufferObject {
	VBOVerticesData,
	VBOTextureData,
	BOCount
};
enum eProgram {
	QuadScreenProgram,
	ProgramCount
};
enum eTexture {
	BrickTexture,
	TextureCount
};

#include "common.cpp"

GLchar					windowTitle[] = "Texturing";
/** 6 darab háromszögbõl felépített gúla. */
/** Pyramid made of 6 triangles. */
/** https://www.khronos.org/opengl/wiki/Tutorial4:_Using_Indices_and_Geometry_Shaders_(C_/SDL) */
GLfloat verticesData[] = {
	// sides / a palást szemközti háromszöge
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	 0.0f,  1.0f,  0.0f,
	// jobb oldali háromszöge
	 1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 0.0f,  1.0f,  0.0f,
	// hátulsó oldali háromszöge
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 0.0f,  1.0f,  0.0f,
	// bal oldali háromszöge
	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 0.0f,  1.0f,  0.0f,
	// base rectangle / az alaplap egy négyzet
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
};
/** A modellünket alkotó valamennyi vertex számára megadjuk a textúra-koordináta értékeket.
Az itt megadott értékek a textúrázáshoz felhasznált kép pixeleire (texel) hivatkoznak, vagyis
itt definiáljuk, hogy a 3D modellünk (x,y,z) értékhármassal adott pontjának színét melyik
(s,t) koordinátákkal jelölt texel adja meg.*/
/** (s,t) texture coordinates for the (x,y,z) points of the triangles of the 3D model.*/
GLfloat texCoords[] = {
	0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
	0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
	0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
	0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f
};
mat4			rotateM, scaleM; // transformation matrices
vec3			cameraPosition	= vec3( 0.0f, 0.0f, 3.0f),
				cameraTarget	= vec3( 0.0f, 0.0f, 0.0f),
				cameraUpVector	= vec3( 0.0f, 1.0f, 0.0f),
				cameraMovingX	= vec3(-1.0f, 0.0f, 0.0f),
				cameraMovingY	= vec3( 0.0f, 1.0f, 0.0f);
/** Textúra objektum generálása, a lehetséges argumentumok listáját a SOIL2.h állományban szereplõ dokumentáció részletezi. */
/** Build up a texture object, parameters enlisted in SOIL2.h. */
GLuint loadTexture(const GLchar* texturePath) {
	/** SOIL_FLAG_INVERT_Y: A képformátumok általában X tengelyre tükrözöttek. */
	/** SOIL_FLAG_INVERT_Y: Stored image formats are usually horizontally mirrored. */
	GLuint		textureID = SOIL_load_OGL_texture(texturePath, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	/** Kilépünk hiba esetén. */
	/** Exit on error. */
	if (textureID == 0) {
		cout << "Error: could not find texture image" << endl;
		cleanUpScene(EXIT_FAILURE);
	}
	/** 2D textúrázáshoz a beolvasott kép textúra azonosítóját hozzárendeljük. */
	/** Bind the texture ID of the image read before to 2D texturing. */
	glBindTexture(GL_TEXTURE_2D, textureID);
	/** A texelek interpolálása, szûrési beállítások:
	1. TEXTURE_MAG_FILTER: Ha a poligonunk nagyobb a textúra képénél.
	2. TEXTURE_MIN_FILTER: Ha a poligonunk kisebb mint a textúra képe.
	Részletesebben itt olvashatunk utána: https://www.khronos.org/opengl/wiki/Sampler_Object */
	/** Texture filtering for texels:
	1. TEXTURE_MAG_FILTER: surface to texture is bigger than the texture.
	2. TEXTURE_MIN_FILTER: surface to texture is smaller than the texture.
	Description: https://www.khronos.org/opengl/wiki/Sampler_Object */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	/* Mipmap szintek legenerálása. */
	/* Generate mipmap levels. */
	glGenerateMipmap(GL_TEXTURE_2D);

	return textureID;
}

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
	/** Növeljük meg a textúra koordinátákat, hogy parkettázni lehessen. */
	/** Make texture coordinates bigger for tiling spaces. */
	for (int i = 0; i < sizeof(texCoords) / sizeof(GLfloat); i++)
		texCoords[i] *= 2.0f;
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	/** Engedélyezzük az imént definiált location = 0 attribútumot (vertexShader.glsl ). */
	/** Enable the previously defined location = 0 attributum (vertexShader.glsl ). */
	glEnableVertexAttribArray(0);
	/** Ugyanez a textúra koordináta attribútumhoz. */
	/** Same for texture coordinates attributum. */
	glBindBuffer(GL_ARRAY_BUFFER, BO[VBOTextureData]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)(0));
	glEnableVertexAttribArray(1);
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
	/** Fekete lesz a háttér. */
	/** Background is black. */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	/** Z-buffer engedélyezése. */
	/** Enable Z coordinate check for visibility. */
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	/* Textúra objektum létrehozása. */
	/* Creating texture object. */
	texture[BrickTexture] = loadTexture("brick.jpg");
	/* A fragment shaderünk számára biztosítjuk a textúra objektumhoz a hozzáférést (uniform sampler variable). */
	/* Fragment shader gets access to the texture object (uniform sampler variable). */
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[BrickTexture]);
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
	/** A textúra koordináták túlhivatkozhatóak a standard [0..1] tartományon. */
	/** Texture coordinates can be used beyond the standard [0..1] interval. */
	/** Az R billentyû lenyomása a textúra ismétlést kapcsolja be. */
	/** Pressing R induces texture repeat. */
	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	/** A B billentyû lenyomása a textúrát a határon vágja el. */
	/** Pressing B induces clamp to border for textures. */
	if (key == GLFW_KEY_B && action == GLFW_PRESS) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		GLfloat borderColor[4] = { GLclampf(212.0 / 255.0), GLclampf(175.0 / 255.0), GLclampf(55.0 / 255.0), 1.0 }; // gold
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	}
	/** Az E billentyû lenyomása a textúra határ ismétlését kapcsolja be. */
	/** Pressing E induces clamp to edge for textures. */
	if (key == GLFW_KEY_E && action == GLFW_PRESS) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	/** Az M billentyû lenyomása a textúra tükrözést kapcsolja be. */
	/** Pressing M induces mirrored repeat for textures. */
	if (key == GLFW_KEY_M && action == GLFW_PRESS) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	}
}

void computeModelMatrix() {
	rotateM		= rotate(mat4(1.0f), radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
	scaleM		= scale(mat4(1.0f), vec3(1.0f, 0.5f, 1.0f));
	matModel	= rotateM * scaleM;
}

void computeCameraMatrix() {
	matView		= lookAt(cameraPosition, cameraTarget, cameraUpVector);
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

	computeModelMatrix();
	computeCameraMatrix();
	/** Kiszámolt adatok átadása a shadereknek. */
	/** Pass parameters to the shaders. */
	glUniformMatrix4fv(locationMatModel, 1, GL_FALSE, value_ptr(matModel));
	glUniformMatrix4fv(locationMatView, 1, GL_FALSE, value_ptr(matView));
	/** A gúla megrajzolása. */
	/** Draw the pyramid. */
	glDrawArrays(GL_TRIANGLES, 0, sizeof(verticesData) / (3 * sizeof(GLfloat))); // 3 components for vertices
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

void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
}

int main(void) {
	/** Az alkalmazáshoz kapcsolódó elõkészítõ lépések (paraméterek: major, minor OpenGL verziók, OpenGL pofile). */
	/** The first initialization steps of the program (params: major, minor OpenGL versions, OpenGL pofile). */
	init(4, 2, GLFW_OPENGL_CORE_PROFILE);	// 4.2 for binding in fragment shader
	/** A shader programok betöltése. */
	/** Loading the shader programs for rendering. */
	initShaderProgram();
	/** A window elkészítése után a képernyő beállítása. */
	/** Setup screen after window creation. */
	framebufferSizeCallback(window, windowWidth, windowHeight);
	/** Karakterkódolás a szövegekhez. */
	/** Setting locale for characters of texts. */
	setlocale(LC_ALL, "hu_HU");
	cout << "Texturing" << endl << endl;
	cout << "Keyboard control" << endl;
	cout << "ESC\t\texit" << endl;
	cout << "O\t\tinduces orthographic projection" << endl;
	cout << "P\t\tinduces perspective projection" << endl;
	cout << "R\t\tinduces texture repeat" << endl;
	cout << "B\t\tinduces clamp to border for textures" << endl;
	cout << "E\t\tinduces clamp to edge for textures" << endl;
	cout << "M\t\tinduces mirrored repeat for textures" << endl;
	cout << "WASD and arrows\tcamera control" << endl << endl;
	cout << "Weekly tasks" << endl;
	cout << "Gyakorlat 1: Kombináljunk több textúrát!" << endl;
	cout << "Gyakorlat 2: Adjunk fényforrást a helyszínhez!" << endl;
	cout << "Gyakorlat 3: Használjunk 3D textúrát!" << endl;
	cout << "Activity  1: Combine together more texture!" << endl;
	cout << "Activity  2: Add lighting to the scene!" << endl;
	cout << "Activity  3: Use 3D texture for the model!" << endl;
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