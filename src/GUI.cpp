#include "GUI.hpp"

#include "helper.hpp"

GUI::GUI() {
	GuiSetIconScale(2);

#ifdef __EMSCRIPTEN__
	if (isMobile())	{
		buttonSize = 100;
		GuiSetIconScale(4);
	}
#endif
	handleResize();
}

void GUI::handleResize() {
	toolbarRect = { GetScreenWidth() - (buttonSize + 8) * 4 - 24, 8, buttonSize * 3, buttonSize};
}

int GUI::getSelected() {
	return selectedState;
}

bool GUI::getPaused() {
	return paused;
}

void GUI::togglePaused() {
	paused = !paused;
}

bool GUI::isMouseOverGUI() {
	return CheckCollisionPointRec(GetMousePosition(), toolbarRect);
}

bool GUI::isOverlapping(Vector2 point) {
	return CheckCollisionPointRec(point, toolbarRect);
}

void GUI::render() {
	GuiToggleGroup((Rectangle) {
		toolbarRect.x, toolbarRect.y, buttonSize, buttonSize
	}, "#68#;#212#;TWO", &selectedState);

	GuiToggle((Rectangle) {
		toolbarRect.x + (buttonSize + 8) * 3, toolbarRect.y, buttonSize, buttonSize
	}, "#132#", &paused);
}
