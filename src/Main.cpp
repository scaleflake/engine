#include "Precompiled.h"

vec2 GetWorldCursorPos() {
    double xpos, ypos;
    GetCursorPos(&xpos, &ypos);
    return vec2(xpos / ENGINE_WIDTH  * 32.0f - 16.0f, ypos / ENGINE_HEIGHT * -18.0f + 9.0f);
}


Collider collider;
Painter painter;

class Bullet {
public:
    Bullet() {}
    void Initialize() {
        body = collider.AddBody(new Circle(0.1f), 0.0f, 4.0f);
        sprite = painter.AddSprite(1, vec2(0.2f, 0.2f), body, vec4(0.7, 0.4, 0.4f, 0.7f));
    }

    void Update() {

    }

    Body* body;
    Sprite* sprite;
};

class Player {
public:
    Player() {}
    void Initialize() {
        body = collider.AddBody(new Circle(0.5f), 0.0f, 4.0f);
        sprite = painter.AddSprite(1, vec2(1.0f, 1.0f), body, vec4(0.4, 0.6, 0.7f, 0.7f));
    }

    uint cooldown = 5;
    void Update() {
        //collider.RayCasting(this->body->position, glm::normalize(GetWorldCursorPos() - this->body->position), 10.0f);
        collider.CircleCasting(GetWorldCursorPos(), 1.0f);
        if (cooldown > 0) {
            cooldown--;
        }
        if (cooldown <= 0) {
            if (GetMouseButton(GLFW_MOUSE_BUTTON_LEFT)) {
                Bullet bullet;
                bullet.Initialize();
                bullet.body->position = this->body->position;
                bullet.body->velocity = glm::normalize(GetWorldCursorPos() - this->body->position) * 30.0f;
            }
            if (GetMouseButton(GLFW_MOUSE_BUTTON_RIGHT)) {
                int c = rand() % 4 + 3;
                vec2* vertices = new vec2[c];
                for (int i = 0; i < c; ++i) {
                    vertices[i] = vec2(Random(-2.0f, 2.0f), Random(-2.0f, 2.0f));
                }
                Polygon poly(vertices, c);
                Body* body = collider.AddBody(&poly, GetWorldCursorPos());
                body->SetOrientation(Random(-CONST_PI, CONST_PI));
                delete[] vertices;
            }
            cooldown = 5;
        }

        this->body->velocity = vec2(0.0f, 0.0f);
        this->body->angularVelocity = 0.0f;
        if (GetKey(GLFW_KEY_W)) {
            this->body->velocity.y =  1.0f;
        }
        if (GetKey(GLFW_KEY_A)) {
            this->body->velocity.x = -1.0f;
        }
        if (GetKey(GLFW_KEY_S)) {
            this->body->velocity.y = -1.0f;
        }
        if (GetKey(GLFW_KEY_D)) {
            this->body->velocity.x =  1.0f;
        }
        if (body->velocity != vec2(0.0f, 0.0f)) {
            this->body->velocity = glm::normalize(body->velocity) * 5.0f;
        }
        //Renderer::SetView(body->position);
    }

    Body* body;
    Sprite* sprite;
};

class Aim {
public:
    Aim() {}
    void Initialize() {
        body = collider.AddBody(new Circle(0.2f), 0.0f, 0.0f);
        sprite = painter.AddSprite(1, vec2(0.4f, 0.4f), body, vec4(1.0, 0.0, 0.0f, 0.5f));
    }

    void Update() {
        this->body->position = GetWorldCursorPos();
    }
    Body* body;
    Sprite* sprite;
};

class Pillars {
public:
    Pillars() {}
    void Initialize() {
        Body* b;
        Circle c(0.0f);

        vec2 coords[] = {
            vec2(  0.0f, 0.0f),
            vec2(  4.0f, 0.0f),
            vec2( -4.0f, 0.0f),
            vec2(  8.0f, 0.0f),
            vec2( -8.0f, 0.0f),
            vec2( 12.0f, 0.0f),
            vec2(-12.0f, 0.0f),
            vec2( 16.0f, 0.0f),
            vec2(-16.0f, 0.0f),
        };

        vec4 color(0.6, 0.6, 0.6f, 0.7f);

        for (uint i = 0; i < 9; i++) {
            Circle c(Random(1.0f, 2.0f));
            b = collider.AddBody(&c, coords[i]);
            painter.AddSprite(1, vec2(c.radius, c.radius) * 2.0f, b, color);
            b->SetStatic();
        }
    }

    void Update() {

    }
};

class Walls {
public:
    Walls() {}
    void Initialize() {
        Body* b;
        Polygon p(20.0f, 2.5f);

        b = collider.AddBody(&p, 0.0f,  11.0f);
        b->SetStatic();
        b->SetOrientation(0.0f);
        b = collider.AddBody(&p, 0.0f, -11.0f);
        b->SetStatic();
        b->SetOrientation(0.0f);

        p = Polygon(2.5f, 13.0f);
        b = collider.AddBody(&p, -18.0f, 0.0f);
        b->SetStatic();
        b->SetOrientation(0.0f);
        b = collider.AddBody(&p,  18.0f, 0.0f);
        b->SetStatic();
        b->SetOrientation(0.0f);
    }
};

void Level1() {
    Walls walls;
    walls.Initialize();

    Pillars pillars;
    pillars.Initialize();
}

void Level2() {
    Walls walls;
    walls.Initialize();
}

int main(int argc, char** argv) {
    InitializeWindow();
    Renderer::Initialize();

    Level1();

    Player player;
    player.Initialize();

    //Aim aim;
    //aim.Initialize();

    uint counter = 0;
    while (!WindowShouldClose()) {
        glClear(GL_COLOR_BUFFER_BIT);
        glfwPollEvents();

        Renderer::RenderSprite(2, vec2(31.0f, 17.0f), vec2(0.0f, 0.0f), 0.0f, vec4(0.4f, 0.8f, 1.0f, 0.2f));

        player.Update();
        //aim.Update();

        collider.Step();
        painter.Render();
        collider.Render();
        counter++;

        SwapBuffers();
    }

    return 0;
}
