#pragma once

#include <imgui.h>

class ImGuiDrawable {
public:
	virtual void draw() = 0;
	virtual ~ImGuiDrawable() = default;
};