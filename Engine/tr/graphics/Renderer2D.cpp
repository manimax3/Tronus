#include "Renderer2D.h"
#include "../core/Engine.h"
#include "../event/CommonEvents.h"
#include "../filesystem/ResourceManager.h"
#include "../util/Keys.h"
#include "GLCheck.h"
#include "GraphicsHandler.h"

#include <random>

#include "glad/glad.h"

void tr::Renderer2D::Init(GraphicsHandler *gfx, ResourceManager *rm)
{
    rm->GetEngine().sEventSystem->AddListener(this);
    mGfxHandler = gfx;
    mResManager = rm;

    mRenderables.reserve(RENDERABLE_SIZE);
    mUpdateBuffer.reserve(V_BUF_SIZE / sizeof(Vertex));

    mResManager->LoadResource(SHADER_ID);
    mShader = mResManager->GetRes<GLSLShader>(SHADER_ID);

    mProjectionMatrix = Mat4::Orthographic(0, 1280, 720, 0, 1, -1);

    // Generate the buffers
    Call(glGenBuffers(1, &mVbo));
    Call(glGenBuffers(1, &mIbo));

    Call(glGenVertexArrays(1, &mVao));
    Call(glBindVertexArray(mVao));

    Call(glBindBuffer(GL_ARRAY_BUFFER, mVbo));
    Call(glBufferData(GL_ARRAY_BUFFER, V_BUF_SIZE, NULL, GL_DYNAMIC_DRAW));

    Call(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIbo));
    Call(glBufferData(GL_ELEMENT_ARRAY_BUFFER, I_BUF_SIZE * sizeof(uint), NULL,
                      GL_STATIC_DRAW));

    Call(uint *indices = reinterpret_cast<uint *>(
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
                               (void *)0));
    Call(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                               (void *)sizeof(Vec2)));

    Call(glEnableVertexAttribArray(0));
    Call(glEnableVertexAttribArray(1));

    Call(glBindVertexArray(0));
}

void tr::Renderer2D::Tick()
{
    EASY_BLOCK("Renderer2D Tick");

    uint count = mRenderables.size();

    if (count == 0) {
        mRenderCount = 0;
        return;
    }

    Vertex *v           = mUpdateBuffer.data();
    bool    found_dirty = false;

    for (auto &r : mRenderables) {

        if (!r.visible) {
            count--;
            continue;
        }

        if (r.dirty) {
            r.dirty     = false;
            found_dirty = true;
        }

        v[0].pos   = r.bottom_left;
        v[0].color = r.color;

        v[1].pos   = r.top_left;
        v[1].color = r.color;

        v[2].pos   = r.top_right;
        v[2].color = r.color;

        v[3].pos   = r.bottom_right;
        v[3].color = r.color;

        v += 4;
    }

    mRenderCount = count;

    if (!found_dirty)
        return;

    Call(glBindVertexArray(mVao));
    Call(glBufferSubData(GL_ARRAY_BUFFER, 0, mRenderCount * sizeof(Vertex) * 4,
                         mUpdateBuffer.data()));
    Call(glBindVertexArray(0));
}

void tr::Renderer2D::Render()
{
    EASY_BLOCK("Renderer2D Render");

    Call(glBindVertexArray(mVao));
    mShader->Bind();
    mShader->Set("vp", mProjectionMatrix);
    Call(glDrawElements(GL_TRIANGLES, mRenderCount * 6, GL_UNSIGNED_INT,
                        (void *)0));
    Call(glBindVertexArray(0));
}

void tr::Renderer2D::Shutdown() { mResManager->DeleteResource(SHADER_ID); }

tr::Renderer2D::Renderable *tr::Renderer2D::GetNewRenderable()
{
    mRenderables.emplace_back();
    mRenderables.back().id = mIdCounter++;
    return &mRenderables.back();
}

void tr::Renderer2D::DeleteRenderable(Renderable *r)
{
    if (!r)
        return;

    mRenderables.erase(
        std::remove_if(std::begin(mRenderables), std::end(mRenderables),
                       [&](const auto &_r) { return r->id == _r.id; }));

    r = nullptr;
}

void tr::Renderer2D::OnEvent(const Event &e, int channel)
{
    if (e.Identifier != event::INPUT)
        return;

    const InputEvent &ie = static_cast<const InputEvent &>(e);

    if (ie.type != InputEvent::Keyboard)
        return;
    if (ie.action != InputEvent::PRESS)
        return;

    static std::mt19937 rng;

    if (ie.Key == KEY_F5) {
        for (float _x = 0; _x < 1280; _x += 4) {
            for (float _y = 0; _y < 720; _y += 4) {
                std::uniform_real_distribution<> color(0.f, 1.f);
                const float                      x    = _x * 1;
                const float                      y    = _y * 1;
                const float                      size = 4;

                auto *r         = GetNewRenderable();
                r->top_left     = { x, y };
                r->top_right    = { x + size, y };
                r->bottom_left  = { x, y + size };
                r->bottom_right = { x + size, y + size };

                r->color   = { static_cast<float>(color(rng)),
                             static_cast<float>(color(rng)),
                             static_cast<float>(color(rng)), 1.0f };
                r->visible = true;
                r->dirty   = true;
            }
        }
    }
}

std::vector<int> tr::Renderer2D::SubscripeTo() const
{
    return { ENGINE_CHANNEL };
}