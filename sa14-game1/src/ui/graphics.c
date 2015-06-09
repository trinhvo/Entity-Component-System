/*------------------------------------------------------------------------------
 * File: graphics.c
 * Created: June 9, 2015
 * Last changed: June 9, 2015
 *
 * Author(s): Philip Arvidsson (philip@philiparvidsson.com)
 *
 * Description:
 *   Erbjuder grafikfunktioner.
 *
 *----------------------------------------------------------------------------*/

/*------------------------------------------------
 * INCLUDES
 *----------------------------------------------*/

#include "graphics.h"

#include "core/debug.h"

#include "ui/window.h"

/* Windows kr�ver att vi inkluderar Windows.h innan gl.h. */
#ifdef _WIN32
    #include <Windows.h>
#endif

#include <GL/gl.h>

/*------------------------------------------------
 * TYPES
 *----------------------------------------------*/

/*--------------------------------------
 * Type: graphicsT_
 *
 * Description:
 *   Typ som representerar ett grafik f�r ett visst f�nster. Den publika delen
 *   �r ok att exponera ut�t, medan den privata endast anv�nds internt.
 *------------------------------------*/
typedef struct {
    /* --- Public --- */

    windowT *window; /* F�nstret som grafikobjektet �r initierat f�r. */

    /* --- Private --- */

    HDC     hdc;
    HGLRC   hglrc;
} graphicsT_;

/*------------------------------------------------
 * GLOBALS
 *----------------------------------------------*/

/*--------------------------------------
 * Variable: current
 *
 * Description:
 *   Det grafikobjekt som vi ritar till "just nu."
 *------------------------------------*/
static graphicsT_ *current = NULL;

/*------------------------------------------------
 * FUNCTIONS
 *----------------------------------------------*/

/*--------------------------------------
 * Function: initPixelFormat()
 * Parameters:
 *   window  Det f�nster som pixelformatet ska initieras till grafikl�ge f�r.
 *   hdc     Den DC (device context) soma ska anv�ndas i f�nstret.
 *
 * Description:
 *   Initierar grafikl�ge f�r det specificerade f�nstret.
 *------------------------------------*/
static void initPixelFormat(windowT *window, HDC hdc) {
    PIXELFORMATDESCRIPTOR pfd;
    pfd.nSize      = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion   = 1;
    pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cAlphaBits = 8;
    pfd.cDepthBits = 32;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int pixel_format = ChoosePixelFormat(hdc, &pfd);

    assert(pixel_format != 0);
    assert(SetPixelFormat(hdc, pixel_format, &pfd));
}

/*--------------------------------------
 * Function: makeCurrent()
 * Parameters:
 *   g  Det grafikobjekt som n�stkommande anrop till ritfunktioner ska anv�nda.
 *
 * Description:
 *   S�tter det specificerade grafikobjektet till det grafikobjekt som anv�nds
 *   f�r ritfunktioner.
 *------------------------------------*/
static void makeCurrent(graphicsT_ *g) {
    if (g == current)
        return;

    current = g;

    if (g)
        wglMakeCurrent(g->hdc, g->hglrc);
}

/*--------------------------------------
 * Function: initGraphics()
 * Parameters:
 *   window  Det f�nster som grafikobjektet ska initieras f�r.
 *
 * Returns:
 *   En pekare till grafikobjektet.
 *
 * Description:
 *   Initierar grafik f�r det specificerade f�nstret.
 *------------------------------------*/
graphicsT *initGraphics(windowT *window) {
    HDC hdc = GetDC(_getHwnd(window));

    initPixelFormat(window, hdc);

    HGLRC hglrc = wglCreateContext(hdc);

    assert(hglrc != NULL);
    assert(wglMakeCurrent(hdc, hglrc));

    graphicsT_ *g = malloc(sizeof(*(graphicsT_ *)NULL));

    g->window = window;
    g->hdc    = hdc;
    g->hglrc  = hglrc;

    assert(g != NULL);

    /* H�r nedanf�r initierar vi OpenGL. */

    /* Utan GL_BLEND fungerar inte kantutj�mningen f�r linjer. */
    glEnable   (GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* Runda, fina prickar! :-) */
    glEnable   (GL_POINT_SMOOTH);
    glHint     (GL_POINT_SMOOTH_HINT, GL_NICEST);

    /* Mjuka, fina linjer! */
    glEnable   (GL_LINE_SMOOTH);
    glHint     (GL_LINE_SMOOTH_HINT, GL_NICEST);

    return ((graphicsT *)g);
}

/*--------------------------------------
 * Function: freeGraphics()
 * Parameters:
 *   g  Det grafikobjekt som ska frias.
 *
 * Description:
 *   Frig�r det specificerade grafikobjektet.
 *------------------------------------*/
void freeGraphics(graphicsT *g) {
    if (g == current) {
        wglMakeCurrent(((graphicsT_ *)g)->hdc, NULL);
        current = NULL;
    }

    wglDeleteContext(((graphicsT_ *)g)->hglrc);

    free(g);
}

/*--------------------------------------
 * Function: clearCanvas()
 * Parameters:
 *   g      Det grafikobjekt som anropet g�ller.
 *   red    R�d f�rgkomponent.
 *   green  Gr�n f�rgkomponent.
 *   blue   Bl� f�rgkomponent.
 *
 * Description:
 *   Rensar ritytan til den specificerade f�rgen.
 *------------------------------------*/
void clearCanvas(graphicsT *g, float red, float green, float blue) {
    makeCurrent((graphicsT_ *)g);

    glClearColor(red, green, blue, 1.0f);
    glClear     (GL_COLOR_BUFFER_BIT);
}

/*--------------------------------------
 * Function: setColor()
 * Parameters:
 *   g      Det grafikobjekt som anropet g�ller.
 *   red    R�d f�rgkomponent.
 *   green  Gr�n f�rgkomponent.
 *   blue   Bl� f�rgkomponent.
 *
 * Description:
 *   �ndrar f�rg f�r n�stkommande anrop till ritfunktioner.
 *------------------------------------*/
void setColor(graphicsT *g, float red, float green, float blue) {
    makeCurrent((graphicsT_ *)g);

    glColor3f(red, green, blue);
}

/*--------------------------------------
 * Function: updateGraphics()
 * Parameters:
 *   g      Det grafikobjekt som anropet g�ller.
 *
 * Description:
 *   Uppdaterar grafiken och ritar upp den i det associerade f�nstret.
 *------------------------------------*/
void updateGraphics(graphicsT *g) {
    makeCurrent((graphicsT_ *)g);

    SwapBuffers(((graphicsT_ *)g)->hdc);
}
