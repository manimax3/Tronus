#include "ImguiRenderer.h"
#include "../core/Engine.h"
#include "../event/CommonEvents.h"
#include "../profile/Profiler.h"
#include "../util/Keys.h"
#include "GLCheck.h"
#include "GraphicsHandler.h"
#include "Texture.h"
#include "glad/glad.h"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui.h"
#include "nlohmann/json.hpp"

static bool   g_MouseJustPressed[3] = { false, false, false };
static double g_time                = 0.0;

void tr::ImguiRenderer::Init(GraphicsHandler *gfx, ResourceManager *rm)
{
    using json = nlohmann::json;

    mGfxHandler = gfx;
    mResManager = rm;

    const auto w_size = gfx->GetWindowSize();

    mProjectionMatrix = math::ortho(0.f, w_size.x, w_size.y, 0.f, 1.f, -1.f);

    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    auto &im = ImGui::GetIO();

    im.DisplaySize.x = w_size.x;
    im.DisplaySize.y = w_size.y;

    im.DisplayFramebufferScale = ImVec2(1, 1);

    /* mResManager->LoadResource("DebugRenderer.json"); */

    Call(glGenBuffers(1, &mVbo));
    Call(glGenBuffers(1, &mIbo));
    Call(glGenVertexArrays(1, &mVao));

    Call(glBindVertexArray(mVao));

    Call(glBindBuffer(GL_ARRAY_BUFFER, mVbo));
    Call(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIbo));

    Call(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert),
                               (GLvoid *)offsetof(ImDrawVert, pos)));
    Call(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert),
                               (GLvoid *)offsetof(ImDrawVert, uv)));
    Call(glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE,
                               sizeof(ImDrawVert),
                               (GLvoid *)offsetof(ImDrawVert, col)));

    Call(glEnableVertexAttribArray(0));
    Call(glEnableVertexAttribArray(1));
    Call(glEnableVertexAttribArray(2));

    Call(glBindVertexArray(0));

    uint32 *pixels;
    int     width, height;
    im.Fonts->GetTexDataAsRGBA32(reinterpret_cast<byte **>(&pixels), &width,
                                 &height);

    json texture_handle;
    texture_handle["id"]               = TEXTURE_ID;
    texture_handle["type"]             = "Texture";
    texture_handle["compression"]      = false;
    texture_handle["generate_mipmaps"] = false;
    texture_handle["mag_filter"]       = 1;
    texture_handle["min_filter"]       = 1;
    texture_handle["wrap_s"]           = 1;
    texture_handle["wrap_t"]           = 1;
    texture_handle["dependencies"]     = json::array();
    json o                             = { { "id", IMAGE_ID },
               { "type", "Image" },
               { "memory", reinterpret_cast<uint64>(pixels) },
               { "xSize", width },
               { "ySize", height } };
    texture_handle["dependencies"][0]  = o;

    mResManager->LoadResource(texture_handle.dump(), true);
    mResManager->LoadResource(SHADER_ID);

    mShader = mResManager->GetRes<GLSLShader>(SHADER_ID);

    Texture *tex    = mResManager->GetRes<Texture>(TEXTURE_ID);
    im.Fonts->TexID = reinterpret_cast<void *>(tex);

    im.KeyMap[ImGuiKey_Tab]        = KEY_TAB;
    im.KeyMap[ImGuiKey_LeftArrow]  = KEY_LEFT;
    im.KeyMap[ImGuiKey_RightArrow] = KEY_RIGHT;
    im.KeyMap[ImGuiKey_UpArrow]    = KEY_UP;
    im.KeyMap[ImGuiKey_DownArrow]  = KEY_DOWN;
    im.KeyMap[ImGuiKey_PageUp]     = KEY_PAGE_UP;
    im.KeyMap[ImGuiKey_PageDown]   = KEY_PAGE_DOWN;
    im.KeyMap[ImGuiKey_Home]       = KEY_HOME;
    im.KeyMap[ImGuiKey_End]        = KEY_END;
    im.KeyMap[ImGuiKey_Insert]     = KEY_INSERT;
    im.KeyMap[ImGuiKey_Delete]     = KEY_DELETE;
    im.KeyMap[ImGuiKey_Backspace]  = KEY_BACKSPACE;
    im.KeyMap[ImGuiKey_Space]      = KEY_SPACE;
    im.KeyMap[ImGuiKey_Enter]      = KEY_ENTER;
    im.KeyMap[ImGuiKey_Escape]     = KEY_ESCAPE;
    im.KeyMap[ImGuiKey_A]          = KEY_A;
    im.KeyMap[ImGuiKey_C]          = KEY_C;
    im.KeyMap[ImGuiKey_V]          = KEY_V;
    im.KeyMap[ImGuiKey_X]          = KEY_X;
    im.KeyMap[ImGuiKey_Y]          = KEY_Y;
    im.KeyMap[ImGuiKey_Z]          = KEY_Z;

    im.SetClipboardTextFn = [](void *ud, const char *txt) {
        auto *gfx = reinterpret_cast<GraphicsHandler *>(ud);
        if (gfx)
            gfx->SetClipboard(txt);
    };
    im.GetClipboardTextFn = [](void *ud) {
        auto *gfx = reinterpret_cast<GraphicsHandler *>(ud);
        if (gfx)
            gfx->GetClipboard();
        return (const char *)nullptr;
    };

    im.ClipboardUserData = reinterpret_cast<void *>(mGfxHandler);

    mGfxHandler->GetEngine().sEventSystem->AddListener(this);
}

void tr::ImguiRenderer::Render()
{
    EASY_BLOCK("ImguiRenderer Render");
    ImGui::Render();
    draw_data(ImGui::GetDrawData());
}

void tr::ImguiRenderer::StartDebugFrame()
{
    EASY_BLOCK("ImguiRenderer StartDebugFrame");
    auto &     im     = ImGui::GetIO();
    const auto w_size = mGfxHandler->GetWindowSize();

    im.DisplaySize.x = w_size.x;
    im.DisplaySize.y = w_size.y;

    const double current_time = mGfxHandler->GetTime();
    im.DeltaTime
        = g_time > 0.0 ? (float)(current_time - g_time) : (float)(1.0f / 60.0f);
    g_time = current_time;

    for (int i = 0; i < 3; i++) {
        im.MouseDown[i] = g_MouseJustPressed[i];
    }

    ImGui::NewFrame();

    mGfxHandler->GetEngine().sEventSystem->DispatchEvent(RenderDebugEvent(),
                                                         RENDER_CHANNEL);
}

void tr::ImguiRenderer::draw_data(ImDrawData *draw_data)
{
    auto &im = ImGui::GetIO();

    int fb_width  = (int)(im.DisplaySize.x * im.DisplayFramebufferScale.x);
    int fb_height = (int)(im.DisplaySize.y * im.DisplayFramebufferScale.y);
    if (fb_width == 0 || fb_height == 0)
        return;

    draw_data->ScaleClipRects(im.DisplayFramebufferScale);

    Call(glEnable(GL_BLEND));
    Call(glBlendEquation(GL_FUNC_ADD));
    Call(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    Call(glDisable(GL_CULL_FACE));
    Call(glDisable(GL_DEPTH_TEST));
    Call(glEnable(GL_SCISSOR_TEST));
    Call(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));

    mShader->Bind();
    mShader->Set("proj", mProjectionMatrix);
    mShader->Set("tex", 0);

    Call(glBindSampler(0, 0));

    Call(glBindVertexArray(mVao));
    Call(glBindBuffer(GL_ARRAY_BUFFER, mVbo));
    Call(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIbo));

    for (int n = 0; n < draw_data->CmdListsCount; n++) {
        const ImDrawList *cmd_list          = draw_data->CmdLists[n];
        const ImDrawIdx * idx_buffer_offset = 0;

        Call(glBufferData(
            GL_ARRAY_BUFFER,
            (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert),
            (const GLvoid *)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW));
        Call(glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx),
            (const GLvoid *)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW));

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
            const ImDrawCmd *pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback) {
                pcmd->UserCallback(cmd_list, pcmd);
            } else {
                reinterpret_cast<Texture *>(pcmd->TextureId)->Bind();
                Call(glScissor((int)pcmd->ClipRect.x,
                               (int)(fb_height - pcmd->ClipRect.w),
                               (int)(pcmd->ClipRect.z - pcmd->ClipRect.x),
                               (int)(pcmd->ClipRect.w - pcmd->ClipRect.y)));
                Call(glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount,
                                    sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT
                                                           : GL_UNSIGNED_INT,
                                    idx_buffer_offset));
            }
            idx_buffer_offset += pcmd->ElemCount;
        }
    }
    Call(glDisable(GL_SCISSOR_TEST));
    Call(glDisable(GL_BLEND));
    Call(glBindVertexArray(0));
}

std::vector<int> tr::ImguiRenderer::SubscripeTo() const
{
    return { ENGINE_CHANNEL };
}

void tr::ImguiRenderer::OnEvent(const Event &e, int channel)
{
    if (e.Identifier == event::INPUT) {

        const InputEvent &ie = static_cast<const InputEvent &>(e);
        auto &            im = ImGui::GetIO();

        if (ie.type == InputEvent::MouseButton) {

            if (ie.action == InputEvent::REPEAT)
                return;

            if (ie.Key >= 0 && ie.Key < 3)
                g_MouseJustPressed[ie.Key] = ie.action == InputEvent::PRESS;

        } else if (ie.type == InputEvent::Scroll) {

            /* im.MouseWheelH += ie.xoffset; */
            im.MouseWheel += (float)ie.yoffset;

        } else if (ie.type == InputEvent::Keyboard) {

            if (ie.action == InputEvent::PRESS)
                im.KeysDown[ie.Key] = true;
            else if (ie.action == InputEvent::RELEASE)
                im.KeysDown[ie.Key] = false;

            im.KeyAlt   = ie.Alt;
            im.KeyCtrl  = ie.Control;
            im.KeyShift = ie.Shift;
            im.KeySuper = ie.Super;

        } else if (ie.type == InputEvent::Char) {

            if (ie.codepoint > 0 && ie.codepoint < 0x10000)
                im.AddInputCharacter((unsigned short)ie.codepoint);
        } else if (ie.type == InputEvent::Mouse) {
            im.MousePos = ImVec2(ie.XPos, ie.YPos);
        }
    } else if (e.Identifier == event::WINDOW) {
        const WindowEvent &we = static_cast<const WindowEvent &>(e);

        if (we.type == WindowEvent::RESIZED) {
            mProjectionMatrix
                = math::ortho<float>(0.f, we.xSize, we.ySize, 0.f, 1.f, -1.f);
        }
    }
}

void tr::ImguiRenderer::Shutdown() {}
