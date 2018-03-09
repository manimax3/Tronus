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

    mRenderables.reserve(BUFFER_GROWTH);

    mResManager->LoadResource(SHADER_ID);
    mShader = mResManager->GetRes<GLSLShader>(SHADER_ID);

    mProjectionMatrix = Mat4::Orthographic(0, 1280, 720, 0, 1, -1);

    // Generate the buffers
    Call(glGenBuffers(1, &mVbo));
    Call(glGenBuffers(1, &mIbo));

    Call(glGenVertexArrays(1, &mVao));
    Call(glBindVertexArray(mVao));

    Call(glBindBuffer(GL_ARRAY_BUFFER, mVbo));
    Call(glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * BUFFER_GROWTH * 4, NULL,
                      GL_DYNAMIC_DRAW));

    Call(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIbo));
    Call(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * BUFFER_GROWTH * 6,
                      NULL, GL_STATIC_DRAW));

    Call(uint *indices = static_cast<uint *>(
             glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY)));

    for (uint i = 0; i < BUFFER_GROWTH * 6; i += 6) {
        const uint vertex = i / 6;
        indices[i + 0]    = vertex + 0;
        indices[i + 1]    = vertex + 1;
        indices[i + 2]    = vertex + 2;
        indices[i + 3]    = vertex + 0;
        indices[i + 4]    = vertex + 2;
        indices[i + 5]    = vertex + 3;
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

    if (count == 0){
        mRenderCount = 0;
        return;
    }

    Call(glBindBuffer(GL_ARRAY_BUFFER, mVbo));

        EASY_BLOCK("Transforming renderables");
        Call(Vertex *v = static_cast<Vertex *>(glMapBufferRange(
                 GL_ARRAY_BUFFER, 0, count * sizeof(Vertex) * 4,
                 GL_MAP_WRITE_BIT)));

        for (const auto &r : mRenderables) {

            if (!r.visible) {
                count--;
                continue;
            }

            v->pos   = r.bottom_left;
            v->color = r.color;

            v++;

            v->pos   = r.top_left;
            v->color = r.color;

            v++;

            v->pos   = r.top_right;
            v->color = r.color;

            v++;

            v->pos   = r.bottom_right;
            v->color = r.color;

            v++;
    }

    Call(glUnmapBuffer(GL_ARRAY_BUFFER));
    mRenderCount = count;
}

void tr::Renderer2D::Render()
{
    EASY_BLOCK("Renderer2D Render");

    Call(glBindVertexArray(mVao));
    mShader->Bind();
    mShader->Set("vp", mProjectionMatrix);
    Call(glDrawElements(GL_TRIANGLES, mRenderCount * 6, GL_UNSIGNED_INT, (void *)0));
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

    static std::mt19937       rng;

    if (ie.Key == KEY_F5) {
        for (int i = 0; i < 1023; i++) {
            std::uniform_real_distribution<> width(0, 1280.f);
            std::uniform_real_distribution<> height(0, 720.f);
            std::uniform_real_distribution<> color(0.f, 1.f);
            auto *                           r = GetNewRenderable();
            r->top_left     = { static_cast<float>(width(rng)),
                            static_cast<float>(height(rng)) };
            r->top_right    = { static_cast<float>(width(rng)),
                             static_cast<float>(height(rng)) };
            r->bottom_left  = { static_cast<float>(width(rng)),
                               static_cast<float>(height(rng)) };
            r->bottom_right = { static_cast<float>(width(rng)),
                                static_cast<float>(height(rng)) };
            r->color        = { static_cast<float>(color(rng)),
                         static_cast<float>(color(rng)),
                         static_cast<float>(color(rng)), 1.0f };
            r->visible      = true;
        }
    }
}

std::vector<int> tr::Renderer2D::SubscripeTo() const
{
    return { ENGINE_CHANNEL };
}
