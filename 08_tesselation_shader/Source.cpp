enum eVertexArrayObject {
	VAOCurveData,
	VAOCount
};
enum eVertexBufferObject {
	VBOHermiteData,
	VBOBezierData,
	BOCount
};
enum eProgram {
	CurveTesselationProgram,
	QuadScreenProgram,
	ProgramCount
};
enum eTexture {
	NoTexture,		// fixes 0 sized array problem
	TextureCount
};

#include "common.cpp"

#define	HERMITE_GMT			1
#define	BEZIER_GMT			2
#define	BEZIER_BERNSTEIN	3


GLchar	windowTitle[] = "Hermite and Bezier Curves with Tesselation Shader (GMT Formula and Bernstein Polynoms)";
/* Tömb a görbe kontroll pontjainak és tangenseinek tárolásához. */
/* Array for storing the control points and tangent vectors of a curve. */
GLfloat	hermite_data[][3] = {
	// control points		
	{ -0.2f, -0.3f, 0.0f }, {  0.3f,  0.2f, 0.0f },
	// tangent vectors
	{ -5.0f,  5.0f, 0.0f }, { -5.0f,  5.0f, 0.0f }
};
/* Tömb a görbe kontroll pontjainak tárolásához. */
/* Array for storing the control points of a curve. */
GLfloat	bezier_control_points[][3] = {
	{ -0.5f, -0.5f, 0.0f }, { -0.5f,  0.5f, 0.0f },
	{  0.5f,  0.5f, 0.0f }, {  0.5f, -0.5f, 0.0f }
};
/** A shader változók location-jei és a CPU oldali változóik. */
/** Shader variable locations and the corresponding CPU variables. */
GLuint			locationTessMatProjection, locationTessMatModelView, locationCurveType, locationControlPointsNumber;
GLuint			curveType = HERMITE_GMT, controlPointsNumber = 4;

void initTesselationShader() {
	ShaderInfo shader_info[] = {
		{ GL_FRAGMENT_SHADER,			"./CurveFragShader.glsl" },
		{ GL_TESS_CONTROL_SHADER,		"./CurveTessContShader.glsl" },
		{ GL_TESS_EVALUATION_SHADER,	"./CurveTessEvalShader.glsl" },
		{ GL_VERTEX_SHADER,				"./CurveVertShader.glsl" },
		{ GL_NONE,						nullptr }
	};
	/** Görberajzoló program elkészítése. */
	/** Creating the curve drawing program. */
	program[CurveTesselationProgram] = LoadShaders(shader_info);
	/** A görbe adatainak beállítása. */
	/** Set the data for the curve. */
	/** Csatoljuk a vertex array objektumunkat a shader programhoz. */
	/** Attach the vertex array object to the shader program. */
	glBindVertexArray(VAO[VAOCurveData]);
	/** A GL_ARRAY_BUFFER nevesített csatolóponthoz kapcsoljuk a vertex buffert (ide kerülnek a csúcspont adatok). */
	/** We attach the vertex buffer to the GL_ARRAY_BUFFER node (vertices are stored here). */
	glBindBuffer(GL_ARRAY_BUFFER, BO[VBOHermiteData]);
	/** Másoljuk az adatokat a bufferbe! Megadjuk az aktuálisan csatolt buffert, azt hogy hány byte adatot másolunk,
		a másolandó adatot, majd a feldolgozás módját is meghatározzuk: most az adat nem változik a feltöltés után. */
	/** Copy the data to the buffer! First parameter is the currently attached buffer, second is the size of the buffer to be copied,
		third is the array of data, fourth is working mode: now the data can not be modified after this step. */
	glBufferData(GL_ARRAY_BUFFER, sizeof(hermite_data), hermite_data, GL_STATIC_DRAW);
	/** Same for Bezier. */
	glBindBuffer(GL_ARRAY_BUFFER, BO[VBOBezierData]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bezier_control_points), bezier_control_points, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, BO[VBOHermiteData]); // default
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	/** Engedélyezzük az imént definiált location = 0 attribútumot (HermiteCurveVertShader.glsl). */
	/** Enable the previously defined location = 0 attributum (HermiteCurveVertShader.glsl). */
	glEnableVertexAttribArray(0);
	/** A shader változók location-jeinek lekérdezése és változók beállítása. */
	/** Get shader variable locations and set the corresponding variables. */
	locationCurveType = glGetUniformLocation(program[CurveTesselationProgram], "curveType");
	locationControlPointsNumber = glGetUniformLocation(program[CurveTesselationProgram], "controlPointsNumber");
	locationTessMatProjection = glGetUniformLocation(program[CurveTesselationProgram], "matProjection");
	locationTessMatModelView = glGetUniformLocation(program[CurveTesselationProgram], "matModelView");
	glUseProgram(program[CurveTesselationProgram]);
	glUniform1i(locationCurveType, curveType);
	glUniform1i(locationControlPointsNumber, controlPointsNumber);
}

void initShaderProgram() {
	ShaderInfo shader_info[] = {
		{ GL_FRAGMENT_SHADER,			"./QuadScreenFragShader.glsl" },
		{ GL_VERTEX_SHADER,				"./QuadScreenVertShader.glsl" },
		{ GL_NONE,						nullptr }
	};
	/** A vertex-fragment program elkészítése. */
	/** Creating the vertex-fragment program. */
	program[QuadScreenProgram] = LoadShaders(shader_info);
	locationMatProjection = glGetUniformLocation(program[QuadScreenProgram], "matProjection");
	locationMatModelView = glGetUniformLocation(program[QuadScreenProgram], "matModelView");
}

void display(GLFWwindow* window, double currentTime) {
	/** Töröljük le a kiválasztott buffereket! Fontos lehet minden egyes alkalommal törölni! */
	/** Let's clear the selected buffers! Usually importand to clear it each time! */
	glClear(GL_COLOR_BUFFER_BIT);
	/** Aktiváljuk a shader-program objektumunkat az alapértelmezett fix csõvezeték helyett. */
	/** Activate our shader-program object instead of the default fix pipeline. */
	glUseProgram(program[CurveTesselationProgram]);
	/** A megadott adatok segítségével megrajzoljuk a görbét. */
	/** We draw a curve with the defined array. */
	switch (curveType) {
	case HERMITE_GMT:
	case BEZIER_GMT:
		glPatchParameteri(GL_PATCH_VERTICES, 4);
		glDrawArrays(GL_PATCHES, 0, 4);
		break;
	case BEZIER_BERNSTEIN:
		glPatchParameteri(GL_PATCH_VERTICES, controlPointsNumber);
		glDrawArrays(GL_PATCHES, 0, controlPointsNumber);
		break;
	}
	/** Aktiváljuk a shader-program objektumunkat az alapértelmezett fix csõvezeték helyett. */
	/** Activate our shader-program object instead of the default fix pipeline. */
	glUseProgram(program[QuadScreenProgram]);
	/** A megadott adatok segítségével megrajzoljuk a görbét és végpontjait. */
	/** We draw the curve and its end points. */
	switch (curveType) {
	case BEZIER_GMT:
		glDrawArrays(GL_LINE_STRIP, 0, 4);
	case HERMITE_GMT:
		glDrawArrays(GL_POINTS, 0, 4);
		break;
	case BEZIER_BERNSTEIN:
		glDrawArrays(GL_LINE_STRIP, 0, controlPointsNumber);
		glDrawArrays(GL_POINTS, 0, controlPointsNumber);
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
	glUseProgram(program[QuadScreenProgram]);
	/** Uniform változók beállítása. */
	/** Setup uniform variables. */
	glUniformMatrix4fv(locationMatModelView, 1, GL_FALSE, glm::value_ptr(matModelView));
	glUniformMatrix4fv(locationMatProjection, 1, GL_FALSE, glm::value_ptr(matProjection));
	/** Az aktuális példaprogram bekapcsolása. */
	/** Switch on the current sample program. */
	glUseProgram(program[CurveTesselationProgram]);
	/** Uniform változók beállítása. */
	/** Setup uniform variables. */
	glUniformMatrix4fv(locationTessMatModelView, 1, GL_FALSE, glm::value_ptr(matModelView));
	glUniformMatrix4fv(locationTessMatProjection, 1, GL_FALSE, glm::value_ptr(matProjection));
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	glUseProgram(program[CurveTesselationProgram]);
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
	if ((action == GLFW_PRESS) && ((key == GLFW_KEY_KP_ADD) || (key == GLFW_KEY_EQUAL)) && (controlPointsNumber < 4)) controlPointsNumber++;
	if ((action == GLFW_PRESS) && ((key == GLFW_KEY_KP_SUBTRACT) || (key == GLFW_KEY_MINUS)) && (controlPointsNumber > 0)) controlPointsNumber--;
	/** A H billentyû lenyomása Hermite ívet rajzol GMT képlettel. */
	/** Pressing H induces Hermite curve with GMT formula. */
	if (key == GLFW_KEY_H && action == GLFW_PRESS) {
		curveType = HERMITE_GMT;
		glBindBuffer(GL_ARRAY_BUFFER, BO[VBOHermiteData]);
	}
	/** A B billentyû lenyomása Bezier görbét rajzol GMT képlettel. */
	/** Pressing B induces Bezier curve with GMT formula. */
	if (key == GLFW_KEY_B && action == GLFW_PRESS) {
		curveType = BEZIER_GMT;
		glBindBuffer(GL_ARRAY_BUFFER, BO[VBOBezierData]);
	}
	/** Az A (ARRAY) billentyû lenyomása Bezier görbét rajzol Bernstein polynommal. */
	/** Pressing A (ARRAY) induces Bezier curve with Bernstein polynoms. */
	if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		curveType = BEZIER_BERNSTEIN;
		glBindBuffer(GL_ARRAY_BUFFER, BO[VBOBezierData]);
	}
	/** Új paraméterek átvitele a program[CurveTesselationProgram] shaderbe. */
	/** Passing new parameters to program[CurveTesselationProgram] shader. */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glUniform1i(locationCurveType, curveType);
	glUniform1i(locationControlPointsNumber, controlPointsNumber);
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
	init(4, 0, GLFW_OPENGL_COMPAT_PROFILE); // Tesselation shader core since 4.0
	/** A rajzoláshoz használt shader programok betöltése. */
	/** Loading the shader programs for rendering. */
	initTesselationShader();
	initShaderProgram();
	/** Karakterkódolás a szövegekhez. */
	/** Setting locale for characters of texts. */
	setlocale(LC_ALL, "");
	cout << "Hermite and Bezier Curves with Tesselation Shader (GMT Formula and Bernstein Polynoms)" << endl;
	cout << "https://www.khronos.org/opengl/wiki/Rendering_Pipeline_Overviewn" << endl;
	cout << "https://www.khronos.org/opengl/wiki/Tessellation#Tessellation_primitive_generation" << endl;
	cout << "https://computeranimations.wordpress.com/2015/03/16/rasterization-of-parametric-curves-using-tessellation-shaders-in-glsl/" << endl;
	cout << "https://voxels.blogspot.com/2011/09/tesselation-shader-tutorial-with-source.html" << endl;
	cout << "https://web.engr.oregonstate.edu/~mjb/cs519/Handouts/tessellation.1pp.pdf" << endl << endl;
	cout << "Keyboard control" << endl;
	cout << "ESC\texit" << endl;
	cout << "O\tinduces orthographic projection" << endl;
	cout << "P\tinduces perspective projection" << endl;
	cout << "H\tHermite curve with GMT" << endl;
	cout << "B\tBezier curve with GMT" << endl;
	cout << "A\tArrayed Bezier curve with Bernstein polynoms" << endl;
	cout << "+\tincrease control point number" << endl;
	cout << "-\tdecrease control point number" << endl << endl;
	cout << "Weekly tasks" << endl;
	cout << "Gyakorlat 1: Használjunk drag-and-drop-ot a görbék pontjainak a mozgatásához!" << endl;
	cout << "Gyakorlat 2: Vizualizáljuk a Hermite ív tangenseit!" << endl;
	cout << "Gyakorlat 3: Használjunk drag-and-drop-ot a Hermite ív tangenseinek a mozgatásához!" << endl;
	cout << "Activity  1: Use drag-and-drop for moving the control points of the curves!" << endl;
	cout << "Activity  2: Let's visualize the tangential vectors of the Hermite curve!" << endl;
	cout << "Activity  3: Use drag-and-drop for moving the tangential vectors of the Hermite curve!" << endl;
	/** A tesselation shader rendszer lekérdezése. */
	/** Getting information on tesselation shader system. */
	cout << endl << "Limitations of the GPU:" << endl;
	GLint	param;
	GLfloat	params[4];
	glGetIntegerv(GL_MAX_PATCH_VERTICES, &param);						// The maximum number of vertices in a patch primitive.
	cout << "GL_MAX_PATCH_VERTICES, at least 32:\t\t\t\t" << param << endl;
	glGetIntegerv(GL_MAX_TESS_CONTROL_INPUT_COMPONENTS, &param);		// The number of components for tessellation control shader per - vertex inputs.
	cout << "GL_MAX_TESS_CONTROL_INPUT_COMPONENTS, at least 128:\t\t" << param << endl;
	glGetIntegerv(GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS, &param);		// The number of components for tessellation control shader per - vertex outputs.
	cout << "GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS, at least 128:\t\t" << param << endl;
	glGetIntegerv(GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS, &param);	// The number of components for tessellation control shader per - patch outputs.
	cout << "GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS, at least 4096:\t" << param << endl;
	glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &param);						// The maximum level supported by the tessellation primitive generator.
	cout << "GL_MAX_TESS_GEN_LEVEL, at least 64:\t\t\t\t" << param << endl;
	glGetIntegerv(GL_MAX_TESS_PATCH_COMPONENTS, &param);				// The number of components for tessellation control shader per - patch outputs.
	cout << "GL_MAX_TESS_PATCH_COMPONENTS, at least 120:\t\t\t" << param << endl;
	glGetFloatv(GL_PATCH_DEFAULT_INNER_LEVEL, params);					// The default inner tessellation level without a control shader.
	cout << "GL_PATCH_DEFAULT_INNER_LEVEL initially(1.0, 1.0):\t\t" << params[0] << ", " << params[1] << endl;
	glGetFloatv(GL_PATCH_DEFAULT_OUTER_LEVEL, params);					// The default outer tessellation level without a control shader.
	cout << "GL_PATCH_DEFAULT_OUTER_LEVEL initially(1.0, 1.0, 1.0, 1.0):\t" << params[0] << ", " << params[1] << ", " << params[2] << ", " << params[3] << endl;
	glGetIntegerv(GL_PATCH_VERTICES, &param);							// The number of vertices in the input patch.
	cout << "GL_MAX_PATCH_VERTICES, initially 3:\t\t\t\t" << param << endl;
	glGetIntegerv(GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS, &param);		// The number of components for tesselation evaluation shader per - vertex inputs.
	cout << "GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS, at least 128:\t\t" << param << endl;
	glGetIntegerv(GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS, &param);	// The number of components for tesselation evaluation shader per - vertex outputs.
	cout << "GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS, at least 128:\t\t" << param << endl;
	/** A képernyõ átméretezés kezelése. */
	/** Callback function for window size change. */
	framebufferSizeCallback(window, windowWidth, windowHeight);
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