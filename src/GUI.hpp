#pragma once

#include <raylib.h>
#include <raygui.h>

enum SelectedState {
	PAN = 0,
	ADD,
	UH,
	NO
};

class GUI {
	private:
		float buttonSize = 50;
		int selectedState = 1;
		bool paused = false;

		Rectangle toolbarRect;

	public:
		GUI();

		void handleResize();
		int getSelected();
		bool getPaused();
		void togglePaused();
		bool isMouseOverGUI();
		bool isOverlapping(Vector2 point);
		void render();
};
