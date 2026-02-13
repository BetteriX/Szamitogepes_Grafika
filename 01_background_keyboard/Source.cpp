#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

using namespace std;
/* Az OpenGL ablak szélesség és magasság értéke. */
/* Width and height of the OpenGL window. */
GLint windowWidth = 600;
GLint windowHeight = 600;
GLchar windowTitle[] = "Hello OpenGL keyboard!";
GLFWwindow *window = nullptr;
/** A normál billentyûk a [0..255] tartományban vannak, a nyilak és a speciális billentyûk pedig a [256..511] tartományban helyezkednek el. */
/** Normal keys are fom [0..255], arrow and special keys are from [256..511]. */
GLboolean keyboard[512] = {GL_FALSE};

/** A kód, amellyel rajzolni tudunk a GLFWwindow objektumunkba. */
/** Call display function which will draw into the GLFWwindow object. */
void display()
{
    /** Töröljük le a színbuffert! */
    /** Let's clear the color buffer! */
    glClear(GL_COLOR_BUFFER_BIT);
}
/** Felesleges objektumok törlése. */
/** Clenup the unnecessary objects. */
void cleanUpScene(int returnCode)
{
    /** Töröljük a GLFW ablakot. Leállítjuk a GLFW-t. */
    /** Destroy the GLFW window. Stop the GLFW system. */
    glfwTerminate();
    /** Kilépés returnCode kóddal. */
    /** Stop the software and exit with returnCode code. */
    exit(returnCode);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    /** ESC billentyûre kilépés. */
    /** Exit on ESC key. */
    if ((action == GLFW_PRESS) && (key == GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    /** A billentyûk lenyomásának és felengedésének regisztrálása. Lehetővé teszi gombkombinációk használatát. */
    /** Let's register press and release events for keys. Enables the usage of key combinations. */
    if (action == GLFW_PRESS)
        keyboard[key] = GL_TRUE;
    else if (action == GLFW_RELEASE)
        keyboard[key] = GL_FALSE;
    if ((action == GLFW_PRESS) && (key == GLFW_KEY_G))
        glClearColor(GLclampf(212.0 / 255.0), GLclampf(175.0 / 255.0), GLclampf(55.0 / 255.0), 1.0); // gold
    if ((action == GLFW_PRESS) && (key == GLFW_KEY_B))
        glClearColor(GLclampf(0.0 / 255.0), GLclampf(0.0 / 255.0), GLclampf(0.0 / 255.0), 1.0); // black
    if ((action == GLFW_PRESS) && (key == GLFW_KEY_R))
        glClearColor(GLclampf(255.0 / 255.0), GLclampf(0.0 / 255.0), GLclampf(0.0 / 255.0), 1.0); // red
    if ((action == GLFW_PRESS) && (key == GLFW_KEY_T))
        glClearColor(GLclampf(0.0 / 255.0), GLclampf(255.0 / 255.0), GLclampf(0.0 / 255.0), 1.0); // green
    if ((action == GLFW_RELEASE) && (key == GLFW_KEY_Z))
        glClearColor(GLclampf(0.0 / 255.0), GLclampf(0.0 / 255.0), GLclampf(255.0 / 255.0), 1.0); // blue
    if ((action == GLFW_REPEAT) && (key == GLFW_KEY_W))
        glClearColor(GLclampf(255.0 / 255.0), GLclampf(255.0 / 255.0), GLclampf(255.0 / 255.0), 1.0); // white
}

int main(void)
{
    /** Próbáljuk meg inicializálni a GLFW-t! */
    /** Try to initialize GLFW! */
    if (!glfwInit())
        cleanUpScene(EXIT_FAILURE);
    /** A használni kívánt OpenGL verzió: 3.3. */
    /** The needed OpenGL version: 3.3. */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#ifdef __APPLE__ // To make macOS happy; should not be needed.
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    /** Próbáljuk meg létrehozni az ablakunkat. */
    /** Let's try to create a window for drawing. */
    /** GLFWwindow* glfwCreateWindow(int width, int height, const char* title, GLFWmonitor * monitor, GLFWwindow * share) */
    if ((window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, nullptr, nullptr)) == nullptr)
    {
        cerr << "Failed to create GLFW window." << endl;
        cleanUpScene(EXIT_FAILURE);
    }
    /** Válasszuk ki az ablakunk OpenGL kontextusát, hogy használhassuk. */
    /** Select the OpenGL context (window) for drawing. */
    glfwMakeContextCurrent(window);
    /** Incializáljuk a GLEW-t, hogy elérhetõvé váljanak az OpenGL függvények, probléma esetén kilépés EXIT_FAILURE értékkel. */
    /** Initalize GLEW, so the OpenGL functions will be available, on problem exit with EXIT_FAILURE code. */
    if (glewInit() != GLEW_OK)
    {
        cerr << "Failed to init the GLEW system." << endl;
        cleanUpScene(EXIT_FAILURE);
    }
    /** 0 = v-sync kikapcsolva, 1 = v-sync bekapcsolva, n = n db képkockányi idõt várakozunk */
    /** 0 = v-sync off, 1 = v-sync on, n = n pieces frame time waiting */
    glfwSwapInterval(1);
    /** Billentyûzethez köthetõ események kezelése. */
    /** Callback function for keyboard events. */
    glfwSetKeyCallback(window, keyCallback);
    /** Elsődleges monitor és annak üzemmód lekérdezése. */
    /** Query primary monitor and its working resolution. */
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    /** Ablak középre helyezése. */
    /** Putting window in the center. */
    glfwSetWindowPos(window, (mode->width - windowWidth) / 2, (mode->height - windowHeight) / 2);
    /** A window oldalarányának megadása a számláló és az osztó segítségével. (pl. 16:9)*/
    /** Setting the aspect ratio using the numerator and the denominator values. (eg. 16:9) */
    glfwSetWindowAspectRatio(window, 1, 1);
    /** Törlési szín beállítása pirosra 1.0 áttetszõség mellett. (red, green, blue, alpha) [0.0, 1.0] zárt tartomány */
    /** Set the clearing color for red at 1.0 transparency. (red, green, blue, alpha) [0.0, 1.0] closed domain */
    /** https://www.rapidtables.com/web/color/Gold_Color.html */
    glClearColor(GLclampf(212.0 / 255.0), GLclampf(175.0 / 255.0), GLclampf(55.0 / 255.0), 1.0); // gold
    /** Karakterkódolás a szövegekhez. */
    /** Setting locale for characters of texts. */
    setlocale(LC_ALL, "");
    cout << "Keyboard control" << endl;
    cout << "ESC\texit" << endl;
    cout << "R\tfor red" << endl;
    cout << "T\tfor green" << endl;
    cout << "Y\tfor blue" << endl;
    cout << "G\tfor gold" << endl;
    cout << "B\tfor black" << endl;
    cout << "W\tfor white" << endl
         << endl;
    cout << "Weekly tasks (https://www.rapidtables.com/web/color/)" << endl;
    cout << "Gyakorlat 1: Használjuk fel az RGB paletta alapszíneit a billentyűzetes háttérszín váltáshoz!" << endl;
    cout << "Gyakorlat 2: Használjuk fel a CMYK paletta alapszíneit a billentyűzetes háttérszín váltáshoz!" << endl;
    cout << "Gyakorlat 3: Adjuk hozzá a kedvenc színeinket a billentyűzetes háttérszín váltáshoz!" << endl;
    cout << "Activity  1: Set the basic colors of RGB palette for the background color change by keyboard!" << endl;
    cout << "Activity  2: Set the basic colors of CMYK palette for the background color change by keyboard!" << endl;
    cout << "Activity  3: Add your favorite colors to the background color change by keyboard!" << endl;
    /** A megadott window struktúra "close flag" vizsgálata. */
    /** Checks the "close flag" of the specified window. */
    while (!glfwWindowShouldClose(window))
    {
        /** A kód, amellyel rajzolni tudunk a GLFWwindow objektumunkba. */
        /** Call display function which will draw into the GLFWwindow object. */
        display();
        /** Double buffered mûködés. */
        /** Double buffered working = swap the front and back buffer here. */
        glfwSwapBuffers(window);
        /** Események kezelése az ablakunkkal kapcsolatban, pl. gombnyomás. */
        /** Handle events related to our window, e.g.: pressing a key or moving the mouse. */
        glfwPollEvents();
    }
    /** Felesleges objektumok törlése. */
    /** Cleanup the unnecessary objects. */
    cleanUpScene(EXIT_SUCCESS);
    /** Kilépés EXIT_SUCCESS kóddal. */
    /** Stop the software and exit with EXIT_SUCCESS code. */
    return EXIT_SUCCESS;
}