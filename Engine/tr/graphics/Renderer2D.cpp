#include "Renderer2D.h"
#include "../core/Engine.h"
#include "../event/CommonEvents.h"
#include "../filesystem/ResourceManager.h"
#include "../util/Keys.h"
#include "GLCheck.h"
#include "GraphicsHandler.h"
#include "Image.h"
#include "Texture.h"

#include "glad/glad.h"

#include "imgui.h"

void tr::Renderer2D::Init(GraphicsHandler *gfx, ResourceManager *rm)
{
    rm->GetEngine().sEventSystem->AddListener(this);
    mGfxHandler = gfx;
    mResManager = rm;

    mRenderables.reserve(RENDERABLE_SIZE);

    mResManager->LoadResource(SHADER_ID);
    mShader = mResManager->GetRes<GLSLShader>(SHADER_ID);

    mProjectionMatrix = Mat4::Orthographic(0, 1280, 720, 0, 1, -1);
    mTransformation.push(Mat4::Identity());

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
    PushTransform(Mat4::Identity(), true);
    StartFrame();
    for (auto &r : mRenderables) {
        Submit(r);
    }
    PopTransform();
    EndFrame();
}

void tr::Renderer2D::Render()
{
    EASY_BLOCK("Renderer2D Render");
    RenderRenderables();
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
    mBufferAccess[0].uv    = { r.uv.x, r.uv.w };

    mBufferAccess[1].pos   = r.top_left;
    mBufferAccess[1].color = r.color;
    mBufferAccess[1].uv    = { r.uv.x, r.uv.y };

    mBufferAccess[2].pos   = r.top_right;
    mBufferAccess[2].color = r.color;
    mBufferAccess[2].uv    = { r.uv.z, r.uv.y };

    mBufferAccess[3].pos   = r.bottom_right;
    mBufferAccess[3].color = r.color;
    mBufferAccess[3].uv    = { r.uv.z, r.uv.w };

    mBufferAccess += 4;

    mRenderCount++;
}

void tr::Renderer2D::EndFrame()
{
    EASY_FUNCTION();

    Call(glUnmapBuffer(GL_ARRAY_BUFFER));

    Call(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIbo));
    Call(glDrawElements(GL_TRIANGLES, mRenderCount * 6, GL_UNSIGNED_INT,
                        (void *)0));

    Call(glBindVertexArray(0));
}

void tr::Renderer2D::PushTransform(const Mat4 &transform, bool over)
{
    if (over)
        mTransformation.push(transform);
    else
        mTransformation.push(mTransformation.top() * transform);
}

void tr::Renderer2D::PopTransform() { mTransformation.pop(); }

void tr::Renderer2D::PushTexture(Texture *const tex) { mCurrenTexture = tex; }

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

void tr::Renderer2D::OnEvent(const Event &e, int channel) {}

std::vector<int> tr::Renderer2D::SubscripeTo() const
{
    return { ENGINE_CHANNEL };
}
