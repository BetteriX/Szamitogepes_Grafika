enum eVertexArrayObject
{
    NoVAO, // fixes 0 sized array problem
    VAOCount
};
enum eBufferObject
{
    NoBO, // fixes 0 sized array problem
    BOCount
};
enum eProgram
{
    ShaderProgram,
    ProgramCount
};
enum eTexture
{
    NoTexture, // fixes 0 sized array problem
    TextureCount
};
enum ePipeline
{
    FixedPipeline, // == 0
    ShaderPipeline // == 1
};

#include "common.cpp"
/** pipeline kapcsolja a fix csővezeték/shaderek használatát. */
/** pipeline switches the fix pipeline/shaders. */
ePipeline pipeline = FixedPipeline;
GLuint primitiveType = GL_POINTS;
GLchar windowTitle[] = "Drawing Primitives with Pipelines";

GLfloat color[2][3][3] = {
    // 2 sets, 3 colors, 3 components
    1.0f, 0.0f, 0.0f, // red
    0.0f, 1.0f, 0.0f, // green
    0.0f, 0.0f, 1.0f, // blue
    0.0f, 1.0f, 1.0f, // cyan
    1.0f, 0.0f, 1.0f, // magenta
    1.0f, 1.0f, 0.0f  // yellow
};
/** std::array használható a glm::vec3 színek tárolásához. */
/** std::array can be used to store glm::vec3 colors. */
/*
static array<vec3, 3> color[2] = { // 2 sets, 3 colors, 3 components
    {
        // R     G     B
        vec3(1.0f, 0.0f, 0.0f), // red
        vec3(0.0f, 1.0f, 0.0f), // green
        vec3(0.0f, 0.0f, 1.0f)  // blue
    },
    {
        vec3(0.0f, 1.0f, 1.0f), // cyan
        vec3(1.0f, 0.0f, 1.0f), // magenta
        vec3(1.0f, 1.0f, 0.0f)  // yellow
    }};
*/
void initShaderProgram()
{
    ShaderInfo shader_info[] = {
        {GL_FRAGMENT_SHADER, "./fragmentShader.glsl"},
        {GL_VERTEX_SHADER, "./vertexShader.glsl"},
        {GL_NONE, nullptr}};
    /** A vertex-fragment program elkészítése. */
    /** Creating the vertex-fragment program. */
    program[ShaderProgram] = LoadShaders(shader_info);
}

void display(GLFWwindow *window, double currentTime)
{
    /** Töröljük le a kiválasztott buffereket! */
    /** Let's clear the selected buffers! */
    glClear(GL_COLOR_BUFFER_BIT);
    /** Közvetlen módú rajzolás kezdete a megadott primitív típussal. Három pontot adunk meg három különbözõ színnel. */
    /** Start drawing in immediate mode with the given primitive type. Three points will be defined with the different colors. */
    glBegin(primitiveType);
    /** A rajzolási szín módosítása. */
    /** Set the drawing color. */
    glColor3fv(color[pipeline][0]);

    glColor3fv((const GLfloat *)&(color[pipeline][0]));
    /** A következõ pont koordinátáinak megadása. */
    /** Set the coordinates of the next point. */
    /*
    glVertex4f(0.25, -0.25, 0.0, 1.0); // given by homogenous coordinates https://en.wikipedia.org/wiki/Homogeneous_coordinates
    glVertex3f(0.25, -0.25, 0.0);      // given by 3D Euclidean space Cartesian coordinates https://en.wikipedia.org/wiki/Cartesian_coordinate_system
    glVertex2f(0.25, -0.25);           // given by 2D Euclidean space Cartesian coordinates https://en.wikipedia.org/wiki/Cartesian_coordinate_system
    glColor3fv(color[pipeline][1]);
    glColor3fv((const GLfloat *)&(color[pipeline][1]));
    glVertex2f(-0.25, -0.25);
    glColor3fv(color[pipeline][2]);
    glColor3fv((const GLfloat *)&(color[pipeline][2]));
    glVertex2f(0.00, 0.25);
    */
    /** Közvetlen módú rajzolás vége. */
    /** End drawing in immediate mode.  */

    /*
    glVertex2f(-1.0, -1.0);
    glVertex2f(-0.5, -1.0);

    glVertex2f(-1.0, 0.5);
    glVertex2f(0, -1.0);

    glVertex2f(-1.0, 0);
    glVertex2f(0.5, -1.0);

    glVertex2f(-1.0, 0.5);
    glVertex2f(1.0, -1.0);


    for (float i = 1; i <= 1; i += 0.04)
    {
        glColor3f(92.0f / 255.0f, 83.0f / 255.0f, 34.0f / 255.0f);
        glVertex2f(-1.00, -i);
        glVertex2f(i, -1.0);
    }
    */

    glVertex2f(-1.0, 0.8);
    glVertex2f(1.0, 0.8);

    glVertex2f(-1.0, -1);
    glVertex2f(1.0, -1);
    glVertex2f(1.0, 0.8);

    glVertex3f(1.0, 0.0, 0.0);
    glVertex2f(-1, 0.8);
    glVertex2f(0, 1);
    glVertex2f(1.0, 0.8);

    // glVertex3f();

    glEnd();
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    /** A minimalizálás nem fog fagyni a minimum 1 értékkel. */
    /** Minimize will not freeze with minimum value 1. */
    windowWidth = glm::max(width, 1);
    windowHeight = glm::max(height, 1);
    /** A kezelt képernyő beállítása a teljes (0, 0, width, height) területre. */
    /** Set the viewport for the full (0, 0, width, height) area. */
    glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
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
    if ((action == GLFW_PRESS) && (key == GLFW_KEY_P))
        primitiveType = GL_POINTS;
    if ((action == GLFW_PRESS) && (key == GLFW_KEY_L))
        primitiveType = GL_LINE_LOOP;
    if ((action == GLFW_PRESS) && (key == GLFW_KEY_K))
        primitiveType = GL_LINE_LOOP;
    if ((action == GLFW_PRESS) && (key == GLFW_KEY_T))
        primitiveType = GL_TRIANGLES;
    /** Aktiváljuk az alapértelmezett fix csõvezetéket. */
    /** Activate the default fix pipeline. */
    if ((action == GLFW_PRESS) && (key == GLFW_KEY_F))
        pipeline = FixedPipeline, glUseProgram(0);
    /** Aktiváljuk a shader-program objektumunkat az alapértelmezett fix csõvezeték helyett. */
    /** Activate our shader-program object instead of the default fix pipeline. */
    if ((action == GLFW_PRESS) && (key == GLFW_KEY_S))
        pipeline = ShaderPipeline, glUseProgram(program[ShaderProgram]);
}
/** Az egér mutató helyét kezelő függvény. */
/** Callback function for mouse position change. */
void cursorPosCallback(GLFWwindow *window, double xPos, double yPos)
{
}
/** Az egér gombjaihoz köthető események kezelése. */
/** Callback function for mouse button events. */
void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
}

int main(void)
{
    /** Az alkalmazáshoz kapcsolódó elõkészítõ lépések (paraméterek: major, minor OpenGL verziók, OpenGL pofile). */
    /** The first initialization steps of the program (params: major, minor OpenGL versions, OpenGL pofile). */
    init(3, 3, GLFW_OPENGL_COMPAT_PROFILE);
    /** A window oldalarányának megadása a számláló és az osztó segítségével. (pl. 16:9)*/
    /** Setting the aspect ratio using the numerator and the denominator values. (eg. 16:9) */
    glfwSetWindowAspectRatio(window, 1, 1);
    /** A rajzoláshoz használt shader programok betöltése. */
    /** Loading the shader programs for rendering. */
    initShaderProgram();
    /** Aktiváljuk a shader-program objektumunkat az alapértelmezett fix csõvezeték helyett ha pipeline == ShaderProgram. */
    /** Activate our shader-program object instead of the default fix pipeline if pipeline == ShaderProgram. */
    if (pipeline == ShaderPipeline)
        glUseProgram(program[ShaderProgram]);
    /** Karakterkódolás a szövegekhez. */
    /** Setting locale for characters of texts. */
    setlocale(LC_ALL, "");
    cout << "A core és a compatibility profilok alapelemei (QUAD, QUAD_STRIP, POLYGON!):" << endl;
    cout << "Primitive types in core and compatibility profiles (QUAD, QUAD_STRIP, POLYGON!):" << endl;
    cout << "https://www.labs.cs.uregina.ca/315/WebGL2/Lab2/#Primitives" << endl
         << endl;
    cout << "Keyboard control" << endl;
    cout << "ESC\texit" << endl;
    cout << "P\tpoints for primitive" << endl;
    cout << "L\tline loop for primitive" << endl;
    cout << "T\ttriangles for primitive" << endl;
    cout << "F\tfixed pipeline (visual feedback RGB)" << endl;
    cout << "S\tshader program (visual feedback CMYK and vec4(0.5, 0.5, 0.0, 0.0) offset)" << endl
         << endl;
    cout << "Weekly tasks (https://www.khronos.org/opengl/wiki/primitive)" << endl;
    cout << "Gyakorlat 1: Fedezzük fel a különböző primitívek rajzolását fix csővezetékkel!" << endl;
    cout << "Gyakorlat 2: Fedezzük fel a különböző primitívek rajzolását shaderekkel!" << endl;
    cout << "Gyakorlat 3: Rajzoljuk meg Jancsi és Juliska házikóját!" << endl;
    cout << "Activity  1: Experience the different primitive types at fix pipeline drawing!" << endl;
    cout << "Activity  2: Experience the different primitive types at shader drawing!" << endl;
    cout << "Activity  3: Draw a little house!" << endl;
    /** A megadott window struktúra "close flag" vizsgálata. */
    /** Checks the "close flag" of the specified window. */
    while (!glfwWindowShouldClose(window))
    {
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