#include "render_UI.h"

#include "../../include/imgui/imgui.h"
#include "../../include/imgui/imgui_impl_glfw.h"

namespace GUI {

void RenderUI() {
    ImGui::Begin( "Settings" );
    ImGui::Button( "Hello" );
    static float value = 0.f;
    ImGui::DragFloat( "Value", &value );
    ImGui::End();
}

}  // namespace GUI