#include "Renderer2D.h"
#include "../core/Engine.h"
#include "../event/CommonEvents.h"
#include "../filesystem/ResourceManager.h"
#include "../profile/Profiler.h"
#include "../util/Keys.h"
#include "GLCheck.h"
#include "GraphicsHandler.h"
#include "Image.h"
#include "Texture.h"
#include "glm/gtc/matrix_transform.hpp"

#include <algorithm>

#include "glad/glad.h"

#include "imgui.h"

tr::Renderer2D::Renderer2D()
    : mIDGenerator(RENDERABLE_SIZE)
{
}

void tr::Renderer2D::Init(GraphicsHandler *gfx, ResourceManager *rm)
{
    mGfxHandler = gfx;
    mResManager = rm;

    gfx->WindowChanged.connect(
        [this](const WindowEvent &e) { this->OnEvent(e); });

    mRenderables.reserve(RENDERABLE_SIZE);

    mShader = ResCast<GLSLShader>(mResManager->LoadResource(SHADER_ID));

    const Vec2 ws     = gfx->GetWindowSize();
    mProjectionMatrix = math::ortho<float>(0, ws.x, ws.y, 0, 1, -1);

    // Generate the buffers
    Call(glGenBuffers(1, &mVbo));
    Call(glGenBuffers(1, &mIbo));

    Call(glGenVertexArrays(1, &mVao));
    Call(glBindVertexArray(mVao));

    Call(glBindBuffer(GL_ARRAY_BUFFER, mVbo));
    Call(glBufferData(GL_ARRAY_BUFFER, V_BUF_SIZE, nullptr, GL_DYNAMIC_DRAW));

    Call(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIbo));
    Call(glBufferData(GL_ELEMENT_ARRAY_BUFFER, I_BUF_SIZE * sizeof(uint),
                      nullptr, GL_STATIC_DRAW));

    Call(auto *indices = reinterpret_cast<uint *>(
             glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY)));

    for (uint i = 0; i < I_BUF_SIZE; i += 6) {
        const uint vertex = (i / 6) * 4;
        indices[i + 0]    = vertex + 0;
        indices[i + 1]    = vertex + 1;
        indices[i + 2]    = vertex + 2;
        indices[i + 3]    = vertex + 2;
        indices[i + 4]    = vertex + 3;
        indices[i + 5]    = vertex + 0;
    }

    Call(glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER));

    Call(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                               (void *)nullptr));
    Call(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                               (void *)sizeof(Vec2)));
    Call(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                               (void *)(sizeof(Vec2) + sizeof(Vec4))));

    Call(glEnableVertexAttribArray(0));
    Call(glEnableVertexAttribArray(1));
    Call(glEnableVertexAttribArray(2));

    Call(glBindVertexArray(0));
}

void tr::Renderer2D::Tick() { EASY_BLOCK("Renderer2D Tick"); }

void tr::Renderer2D::RenderRenderables()
{
    for (auto &r : mRenderables) {
        Submit(r);
    }
}

void tr::Renderer2D::Render()
{
    EASY_BLOCK("Renderer2D Render");
    StartFrame();
    RenderRenderables();
    /* mGfxHandler->GetEngine().sEventSystem->DispatchEvent(Render2DEvent(*this),
     */
    /*                                                      RENDER_CHANNEL); */
    EndFrame();
}

void tr::Renderer2D::StartFrame()
{
    EASY_FUNCTION();
    Call(glBindVertexArray(mVao));
    Call(glBindBuffer(GL_ARRAY_BUFFER, mVbo));

    mShader->Bind();

    mShader->Set("vp", mProjectionMatrix);
    mShader->Set("tex0", 0);
    mShader->Set("tex_avail", !!mCurrenTexture);

    if (mCurrenTexture)
        mCurrenTexture->Bind();

    mRenderCount = 0;

    Call(mBufferAccess = reinterpret_cast<Vertex *>(
             glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY)));
}

void tr::Renderer2D::Submit(const Renderable &r)
{
    EASY_FUNCTION();

    if (!r.visible) {
        return;
    }

    if (mCurrenTexture != r.texture) {
        EndFrame();
        PushTexture(r.texture);
        StartFrame();
    }

    mBufferAccess[0].pos   = r.bottom_left;
    mBufferAccess[0].color = r.color;
    mBufferAccess[0].uv    = Vec2(r.uv.x, r.uv.w);

    mBufferAccess[1].pos   = r.top_left;
    mBufferAccess[1].color = r.color;
    mBufferAccess[1].uv    = Vec2(r.uv.x, r.uv.y);

    mBufferAccess[2].pos   = r.top_right;
    mBufferAccess[2].color = r.color;
    mBufferAccess[2].uv    = Vec2(r.uv.z, r.uv.y);

    mBufferAccess[3].pos   = r.bottom_right;
    mBufferAccess[3].color = r.color;
    mBufferAccess[3].uv    = Vec2(r.uv.z, r.uv.w);

    mBufferAccess += 4;

    mRenderCount++;
}

void tr::Renderer2D::EndFrame()
{
    EASY_FUNCTION();

    Call(glUnmapBuffer(GL_ARRAY_BUFFER));

    Call(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIbo));
    Call(glDrawElements(GL_TRIANGLES, mRenderCount * 6, GL_UNSIGNED_INT,
                        (void *)nullptr));

    Call(glBindVertexArray(0));
}

void tr::Renderer2D::PushTexture(Texture *const tex) { mCurrenTexture = tex; }

void tr::Renderer2D::Shutdown() { mResManager->DeleteResource(SHADER_ID); }

tr::uint tr::Renderer2D::SubmitRenderable(const Renderable &r)
{
    uint id;
    mIDGenerator.CreateID(id);
    mRenderables.push_back(r);
    mRenderables.back().id = id;
    return id;
}

void tr::Renderer2D::DeleteRenderable(uint renderable)
{

    if (!mIDGenerator.IsID(renderable)) {
        return;
    }

    const auto &r = std::remove_if(
        std::begin(mRenderables), std::end(mRenderables),
        [&](const Renderable &r) { return r.id == renderable; });

    if (r != std::end(mRenderables)) {
        mRenderables.erase(r);
    }

    mIDGenerator.DestroyID(renderable);
}

tr::Renderer2D::Renderable *tr::Renderer2D::ModifyRenderable(uint r)
{

    if (!mIDGenerator.IsID(r)) {
        return nullptr;
    }

    const auto &it
        = std::find_if(std::begin(mRenderables), std::end(mRenderables),
                       [&](const Renderable &_r) { return r == _r.id; });

    if (it != std::end(mRenderables)) {
        return &(*it);
    }

    return nullptr;
}

void tr::Renderer2D::OnEvent(const WindowEvent &e)
{
    if (e.type != WindowEvent::RESIZED)
        return;

    mProjectionMatrix = math::ortho<float>(0, e.xSize, e.ySize, 0, 1, -1);
}
