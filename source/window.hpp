#pragma once
#include "core.hpp"
#include "math.hpp"
#include "renderer.hpp"

class GrayEnigne;

namespace GR
{
	class Window
	{
		friend class GrayEngine;

		GLFWwindow* glfwWindow;

	protected:
		Window(const char* title, int width, int height);

		~Window();

	public:
		GRAPI void SetTitle(const char* title);

		GRAPI void SetWindowSize(int width, int height);

		GRAPI void MinimizeWindow();

		GRAPI TIVec2 GetWindowSize() const;

		GRAPI TVec2 GetCursorPos() const;

		GRAPI void SetCursorPos(double xpos, double ypos);

		GRAPI void ShowCursor(bool bShow);

		GRAPI void DisableCursor(bool bDisabled);

		GRAPI void SetAttribute(int attrib, int value);
	};
}