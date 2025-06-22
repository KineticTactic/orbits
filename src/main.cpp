#include "App.hpp"
#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include "DarkGuiStyle.h"

#define RAYGUI_IMPLEMENTATION 
#include <raygui.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

#define SCREEN_WIDTH (1600)
#define SCREEN_HEIGHT (900)

#define WINDOW_TITLE "Orbits!"

#ifdef __EMSCRIPTEN__
EM_BOOL on_resize(int eventType, const EmscriptenUiEvent *uiEvent, void *userData) {
    int width = (int)uiEvent->windowInnerWidth;
    int height = (int)uiEvent->windowInnerHeight;
    SetWindowSize(width, height); // Tells Raylib to update its internal screen size
    return EM_TRUE;
}
#endif

int main(void) {
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
	SetTargetFPS(60);

	GuiLoadStyleGenesis();

	#ifdef __EMSCRIPTEN__
	double width, height;
    emscripten_get_element_css_size("#canvas", &width, &height);
    SetWindowSize((int)width, (int)height);
	emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_FALSE, on_resize);
	#endif

	App app;

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(Color{23, 22, 31, 255});

		app.handleInput();
		app.update();
		app.render();

		EndDrawing();
	}

	CloseWindow();

	return 0;
}
