#pragma once
#include <tr.h>

#include "../filesystem/ResourceLoader.h"
#include "../graphics/Texture.h"
#include "../math/Rect.h"
#include "Component2D.h"

namespace tr {

class Texture;

/**
 * Component to render a sprite screen.
 */
class SpriteComponent : public Component2D {
public:
    void OnWorldEnter(class World &world) override;

    /**
     * Change the visibility of the sprite
     */
    void ChangeVisibleTo(bool visible = true);

    /**
     * Returns if the sprite is currently visible on screen
     */
    bool IsVisible() const { return mVisible; };

    /**
     * Sets the uvs.
     * The uvs do not influence the sprites size
     */
    void SetUVs(Rect rect);

    /**
     * Sets the uvs.
     * @param pos The position of the rectangle in the texture.
     * @param size The size of the rectangle.
     *
     * The pos has nothing to do with the sprites position in the world and is
     * only used to calculate where the sprite is located on a texture.
     *
     * Does not influence the sprites size.
     */
    void SetUVs(Vec2 pos, Vec2 size);

    /**
     * Returns the UV cooradiantes on the texture where the sprite is going to
     * be sampled form.
     */
    Rect GetUV() const { return mUVs; };

    /**
     * Sets the sprites size.
     * Final size may vary based on the components scale transform.
     * Does not influence the uvs.
     */
    void SetSize(Vec2 size);

    /**
     * Returns the size of the Sprite.
     * Doesnt take an scaling related transformations into account.
     */
    Vec2 GetSize() const { return mSize; };

    /**
     * Sets the sprites color
     * @param color color values from [0..1]
     */
    void SetColor(Vec4 color);

    /**
     * Returns the sprites color
     */
    Vec4 GetColor() const { return mColor; };

    /**
     * Sets the used texture.
     * Tries to get the resource from the resourcemanager autmatically.
     *
     * @param name Name of the texture.
     */
    void SetTexture(std::string_view name);

    /**
     * Sets the used texture.
     *
     * @param res ResourcePtr to an already loaded texture resource.
     */
    void SetTexture(ResourcePtr<Texture> res);

    /**
     * Resets the texture causing only the color to be rendered.
     */
    void ResetTexture();

    /**
     * Forces the sprite to udpate its state.
     * The state is basically a copy on the gpu. Needs to be called if changing
     * transform data.
     */
    void ForceRenderStateUpdate();

private:
    /**
     * Used to update the sprites transform
     */
    void OnRelativeChange() override;

private:
    /**
     * Setup the gpu state using the interface of the used renderer
     */
    void SetupRenderState();

    /**
     * Internal method to udpate the render state.
     * Should be called in order to apply change in *this to the gpu.
     */
    void UpdateRenderState();

    /**
     * Flag if the sprite should be visible.
     * Note: Setting this to mVisible == false does not remove the sprites state
     * from the gpu.
     */
    bool mVisible = true;

    /**
     * Handle to the renderers internal state.
     */
    uint mRenderState = 0;

    /**
     * Color of the sprite.
     * Gets multiplied with the sampled color form a possible texture.
     */
    Vec4 mColor = Vec4(1.f);

    /**
     * The coordiantes used to sample the colors from a texture.
     * The coordinates are in absolute pixels and will be auto converted to the
     * internal representation by UpdateRenderState()
     */
    Rect mUVs;

    /**
     * Size of the sprite.
     * In absoulete pixel coordiantes. Does not take scaling into account.
     */
    Vec2 mSize = { 50.f, 50.f };

    /**
     * Used texture to sample texutre colors from.
     */
    ResourcePtr<Texture> mTexture;

    /**
     * Reference to the used 2D Renderer.
     * Gets set on world entering.
     */
    class Renderer2D *mRenderer = nullptr;
};

class FlipbookComponent final : public SpriteComponent {
public:
    void OnWorldEnter(World &world) override;

    /**
     * We use this to advance the animation
     */
    void OnComponentUpdate() override;

    /**
     * Adds a new frame to this animation
     * @param rect UVs on the texture.
     * @param time_point After how many ms this frame should be shown.
     * @return the position of this frame
     */
    int AddFrame(Rect rect, int duration, int position_hint = -1);

    /**
     * Removes a frame at given timepoint.
     */
    void RemoveFrame(int position);

    /**
     * Should the frame be advanced autmatically.
     * Can be disabled to pause the antimation at any give time.
     */
    bool AutoAnimate = true;

    /**
     * Show the next frame.
     */
    void NextFrame();

private:

    /**
     * The curren avtive frame.
     */
    int mCurrentFrame = 0;

    /**
     * Type of a frame consisting of a duration and a rect
     */
    using Frame = std::tuple<int, Rect>;

    /**
     * The frames
     */
    std::list<Frame> mFrames;

    /**
     * We store the amount of ms that need to pass until the next frame should
     * be displayed.
     */
    float mMsUntilNext = 0;
};
}
