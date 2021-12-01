#include "InputHandler.h"
#include "Renderer.h"
#include "OrbitalNavigator.h"
#include "Renderer.h"
#include "imgui.h"

namespace
{
	Navigator::Button translateButton(GL::platform::Button button)
	{
		switch (button)
		{
			case GL::platform::Button::LEFT:
				return Navigator::Button::LEFT;
			case GL::platform::Button::RIGHT:
				return Navigator::Button::RIGHT;
			case GL::platform::Button::MIDDLE:
				return Navigator::Button::MIDDLE;
		}
		return static_cast<Navigator::Button>(-1);
	}
}

InputHandler::InputHandler(Renderer& _renderer, Navigator& _navigator)
    : renderer(_renderer), navigator(_navigator)
{
}

void InputHandler::keyDown(GL::platform::Key key)
{
  if (key == GL::platform::Key::SPACE)
    renderer.tooglePause();
  if (key >= GL::platform::Key::C_1 && key <= GL::platform::Key::C_7)
  {
    renderer.setScene(static_cast<int>(key) - static_cast<int>(GL::platform::Key::C_1));
    return;
  }
}

void InputHandler::keyUp(GL::platform::Key key)
{
}

void InputHandler::buttonDown(GL::platform::Button button, int x, int y)
{
	ImGuiIO& io = ImGui::GetIO();

	if (!ImGui::IsMouseHoveringAnyWindow())
	{
		if (button == GL::platform::Button::LEFT)
			navigator.buttonDown(translateButton(button), x, y);
		if (button == GL::platform::Button::RIGHT)
			navigator.buttonDown(translateButton(button), x, y);
    if (button == GL::platform::Button::MIDDLE)
      navigator.buttonDown(translateButton(button), x, y);
	}
	else
	{
		if (button == GL::platform::Button::LEFT)
			io.MouseDown[0] = true;
		if (button == GL::platform::Button::RIGHT)
			io.MouseDown[1] = true;
		if (button == GL::platform::Button::MIDDLE)
			io.MouseDown[2] = true;
	}
}

void InputHandler::buttonUp(GL::platform::Button button, int x, int y)
{
	ImGuiIO& io = ImGui::GetIO();
	if (button == GL::platform::Button::LEFT)
		io.MouseDown[0] = false;
	if (button == GL::platform::Button::RIGHT)
		io.MouseDown[1] = false;
	if (button == GL::platform::Button::MIDDLE)
		io.MouseDown[2] = false;

	navigator.buttonUp(translateButton(button), x, y);
}

void InputHandler::mouseMove(int x, int y)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2(static_cast<float>(x), static_cast<float>(y));
	//if (Renderer::gui_mouse_hoveres_render_image)
	navigator.mouseMove(x, y);
}

void InputHandler::mouseWheel(int delta)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MouseWheel = static_cast<float>(delta);
	//if (Renderer::gui_mouse_hoveres_render_image)
	navigator.mouseWheel(delta);
}
