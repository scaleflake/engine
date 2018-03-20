#ifndef OPENGL_H
#define OPENGL_H
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Physics/Body.h"

struct SpriteShader {
    void Initialize();
    void Activate() const;
    uint shaderProgram;
    uint projectionUniform;
    uint viewUniform;
    uint modelUniform;
    uint colorUniform;
};

struct DebugShader {
    void Initialize();
    void Activate() const;
    uint shaderProgram;
    uint projectionUniform;
    uint viewUniform;
    uint modelUniform;
};

namespace Renderer {
    void Initialize();
    void SetView(const vec2& view);
    uint GetTexture(const string& name);
    void RenderSprite(const uint& texture, const vec2& scale, const vec2& pos, const float& rotation, const vec4& color);
    void RenderSprite(const uint& texture, const vec2& scale, const vec2& pos, const float& rotation);
    void RenderPoint(const vec2& position);
    void RenderPoint(const float& x, const float& y);
    void RenderRay(const vec2& origin, const vec2& direction);
    void RenderLine(const vec2& origin, const vec2& destiny);
    void RenderLine(const float& ax, const float& ay, const float& bx, const float& by);
    void RenderCircle(const float& radius, const vec2& position);
    void Deinitialize();
};

class Sprite {
public:
    Sprite(uint texture, vec2 scale, Body* body);
    Sprite(uint texture, vec2 scale, Body* body, vec4 color);

    uint texture;
    vec2 scale;
    vec4 color;

    Body* body;
};

class Painter {
public:
    Painter() {}
    Sprite* AddSprite(uint texture, vec2 scale, Body* body);
    Sprite* AddSprite(uint texture, vec2 scale, Body* body, vec4 color);
    void Render() const;
    void Clear();
private:
    vector<Sprite*> sprites;
};
#endif // OPENGL_H
