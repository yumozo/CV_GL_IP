#pragma once
#include "../GUI/tests_menu.h"
#include "../renderer/OpenGL_framebuffer.h"

class GUI {
   public:
    TestsMenuUI m_TestMenu;

    std::shared_ptr<Framebuffer> m_Framebuffer;

    GUI();
    ~GUI();
    void RenderUI();
    void DeleteUI();
};