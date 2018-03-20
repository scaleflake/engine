#include "Precompiled.h"

/* shaders */
SpriteShader* spriteShader;
DebugShader* debugShader;
uint LoadShaders(string vertex, string fragment){
    // Шобы чекнуть
    GLint success = false;
    char infoLog[512];
    // Вершинный шейдер
    uint vertexShader = glCreateShader(GL_VERTEX_SHADER); // Создаем
    string vertexShaderCode; // Читаем из файла
    ifstream vertexShaderStream(vertex, std::ios::in);
    if(vertexShaderStream.is_open()) {
        string Line = "";
        while(getline(vertexShaderStream, Line))
            vertexShaderCode += "\n" + Line;
        vertexShaderStream.close();
    }
    char const * vertexShaderSource = vertexShaderCode.c_str(); // Компиляция
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success); // Чекаем
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // Фрагментный шейдер
    uint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // Создаем
    string fragmentShaderCode; // Читаем из файла
    ifstream fragmentShaderStream(fragment, std::ios::in);
    if(fragmentShaderStream.is_open()) {
        string Line = "";
        while(getline(fragmentShaderStream, Line))
            fragmentShaderCode += "\n" + Line;
        fragmentShaderStream.close();
    }
    char const * fragmentShaderSource = fragmentShaderCode.c_str(); // Компиляция
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);  // Чекаем
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // Линкуем
    uint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success); // Чекаем
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    // Пашол вон
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}

/* textures */
map<string, uint> textures;
uint LoadTexture(char* path) {
    uint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // Наложение
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// повтор
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Фильтрация
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // линейная
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load image, create texture and generate mipmaps
//    int width = 0, height = 0;
//    byte* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGBA);
//    assert(width and height);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
//    glGenerateMipmap(GL_TEXTURE_2D);
//    SOIL_free_image_data(image);

    int width, height, nrChannels;
    //stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    byte* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    return texture;
}

/* models */
uint sprite_vbo;
uint sprite_vao;
uint line_vbo;
uint line_vao;
uint point_vbo;
uint point_vao;
uint circle_vbo;
uint circle_vao;
uint LoadModel() {
    return 0;
}

//float CalculateCircleVertices() {
//    float circleVertices[720];
//    float tetha = 0.0f;
//    float inc = CONST_PI / 30.0f;
//
//    circleVertices[0] = std::cos(tetha); // x
//    circleVertices[1] = std::sin(tetha); // y
//    circleVertices[2] = 0.0f;            // z
//    circleVertices[3] = 0.8f;            // r
//    circleVertices[4] = 0.1f;            // g
//    circleVertices[5] = 1.0f;            // b
//    for (uint i = 0; i < 59; ++i) {
//        circleVertices[i * 12 +  6] = std::cos(tetha); // xw
//        circleVertices[i * 12 +  7] = std::sin(tetha); // y
//        circleVertices[i * 12 +  8] = 0.0f;            // z
//        circleVertices[i * 12 +  9] = 0.8f;            // r
//        circleVertices[i * 12 + 10] = 0.1f;            // g
//        circleVertices[i * 12 + 11] = 1.0f;            // b
//        circleVertices[i * 12 + 12] = circleVertices[i * 12 +  0];
//        circleVertices[i * 12 + 13] = circleVertices[i * 12 +  1];
//        circleVertices[i * 12 + 14] = 0.0f;
//        circleVertices[i * 12 + 15] = 0.8f;
//        circleVertices[i * 12 + 16] = 0.1f;
//        circleVertices[i * 12 + 17] = 1.0f;
//        tetha += inc;
//    }
//    circleVertices[714] = circleVertices[0];
//    circleVertices[715] = circleVertices[1];
//    circleVertices[716] = 0.0f;
//    circleVertices[717] = 0.8f;
//    circleVertices[718] = 0.1f;
//    circleVertices[719] = 1.0f;
//
//    return circleVertices;
//}

void Renderer::Initialize() {
    textures = {
        { "circle"    , LoadTexture("assets/Circle.png")     },
        { "background", LoadTexture("assets/Background.png") }
    };

    /* Настройка OpenGL */
    // Прозрачность
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Сглаживание точек
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    // Сглаживание линий
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
//    // Сглаживание полигонов
//    glEnable(GL_POLYGON_SMOOTH);
//    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    glClearColor(30.0f/256.0f, 30.0f/256.0f, 30.0f/256.0f, 0.1f);
    glPointSize(4.0f);
    glLineWidth(0.5f);
    mat4 projection;
    projection = glm::ortho(-16.0f, 16.0f,  -9.0f,  9.0f, -20.0f, 20.0f);
    //projection = glm::ortho(-24.0f, 24.0f, -13.5f, 13.5f, -20.0f, 20.0f);

    mat4 view;
    view = glm::translate(view, vec3(0.0f, 0.0f, -10.0f));

//    mat4 projection;
//    projection = glm::perspective( 45.0f, (float)ENGINE_WIDTH/(float)ENGINE_HEIGHT, 0.1f, 100.0f);
//
//    mat4 view;
//    view = glm::rotate(view, -0.4f, vec3(1.0f, 0.0f, 0.0f));
//    view = glm::translate(view, vec3(0.0f, 9.0f, -20.0f));

    spriteShader = new SpriteShader();
    spriteShader->Initialize();
    spriteShader->Activate();
    glUniformMatrix4fv(spriteShader->projectionUniform, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(spriteShader->viewUniform, 1, GL_FALSE, value_ptr(view));

    debugShader = new DebugShader();
    debugShader->Initialize();
    debugShader->Activate();
    glUniformMatrix4fv(debugShader->projectionUniform, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(debugShader->viewUniform, 1, GL_FALSE, value_ptr(view));

    float vertices[] = {
        -0.5f, -0.5f,  0.0f,      0.0f, 0.0f,
         0.5f, -0.5f,  0.0f,      1.0f, 0.0f,
         0.5f,  0.5f,  0.0f,      1.0f, 1.0f,
         0.5f,  0.5f,  0.0f,      1.0f, 1.0f,
        -0.5f,  0.5f,  0.0f,      0.0f, 1.0f,
        -0.5f, -0.5f,  0.0f,      0.0f, 0.0f,
    };

    glGenBuffers(1, &sprite_vbo);
    glGenVertexArrays(1, &sprite_vao);

    glBindBuffer(GL_ARRAY_BUFFER, sprite_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(sprite_vao);
    // Говорим где позиции
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Говорим где цвет
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    float line_vertices[] = {
         0.0f,  0.0f,  0.0f,      0.00f, 1.00f, 0.00f,
         1.0f,  1.0f,  0.0f,      0.00f, 1.00f, 0.00f
    };

    glGenBuffers(1, &line_vbo);
    glGenVertexArrays(1, &line_vao);

    glBindBuffer(GL_ARRAY_BUFFER, line_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(line_vertices), line_vertices, GL_STATIC_DRAW);

    glBindVertexArray(line_vao);
    // Говорим где позиции
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Говорим где цвет
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    float point_vertices[] = {
         0.0f,  0.0f,  0.0f,      1.00f, 0.00f, 0.00f
    };

    glGenBuffers(1, &point_vbo);
    glGenVertexArrays(1, &point_vao);

    glBindBuffer(GL_ARRAY_BUFFER, point_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(point_vertices), point_vertices, GL_STATIC_DRAW);

    glBindVertexArray(point_vao);
    // Говорим где позиции
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Говорим где цвет
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

//    float* circle_vertices = CalculateCircleVertices();
//
//    glGenBuffers(1, &circle_vbo);
//    glGenVertexArrays(1, &circle_vao);
//
//    glBindBuffer(GL_ARRAY_BUFFER, circle_vbo);
//    glBufferData(GL_ARRAY_BUFFER, 720 * 4, circle_vertices, GL_STATIC_DRAW);
//
//    glBindVertexArray(circle_vao);
//    // Говорим где позиции
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
//    glEnableVertexAttribArray(0);
//    // Говорим где цвет
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
//    glEnableVertexAttribArray(1);
//    glBindVertexArray(0);
}
void Renderer::SetView(const vec2& view) {
    mat4 viewMatrix;
    viewMatrix = glm::translate(viewMatrix, vec3(-view, -1.0f));
    spriteShader->Activate();
    glUniformMatrix4fv(spriteShader->viewUniform,  1, GL_FALSE, value_ptr(viewMatrix));
    debugShader->Activate();
    glUniformMatrix4fv(debugShader->viewUniform, 1, GL_FALSE, value_ptr(viewMatrix));
}
uint Renderer::GetTexture(const string& name) {
    return textures[name];
}
void Renderer::RenderSprite(const uint& texture, const vec2& scale, const vec2& pos, const float& rotation, const vec4& color) {
    spriteShader->Activate();
    glBindVertexArray(sprite_vao);
    glBindTexture(GL_TEXTURE_2D, texture);
    mat4 model;
    model = glm::translate(model, vec3(pos, 0.0f));
    model = glm::scale(model, vec3(scale, 1.0f));
    model = glm::rotate(model, rotation, vec3(0.0f, 0.0f, 1.0f));
    glUniform4f(spriteShader->colorUniform, color.x, color.y, color.z, color.w);
    glUniformMatrix4fv(spriteShader->modelUniform, 1, GL_FALSE, value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
void Renderer::RenderSprite(const uint& texture, const vec2& scale, const vec2& pos, const float& rotation) {
    spriteShader->Activate();
    glBindVertexArray(sprite_vao);
    glBindTexture(GL_TEXTURE_2D, texture);
    mat4 model;
    model = glm::translate(model, vec3(pos, 0.0f));
    model = glm::scale(model, vec3(scale, 1.0f));
    model = glm::rotate(model, rotation, vec3(0.0f, 0.0f, 1.0f));
    glUniformMatrix4fv(spriteShader->modelUniform, 1, GL_FALSE, value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
void Renderer::RenderPoint(const vec2& position) {
    debugShader->Activate();
    glBindVertexArray(point_vao);
    mat4 model;
    model = glm::translate(model, vec3(position, 0.0f));
    glUniformMatrix4fv(debugShader->modelUniform, 1, GL_FALSE, value_ptr(model));
    glDrawArrays(GL_POINTS, 0, 1);
}
void Renderer::RenderPoint(const float& x, const float& y) {
    Renderer::RenderPoint(vec2(x, y));
}
void Renderer::RenderRay(const vec2& origin, const vec2& direction) {
    debugShader->Activate();
    glBindVertexArray(line_vao);
    mat4 model;
    model = glm::translate(model, vec3(origin, 0.0f));
    model = glm::scale(model, vec3(direction, 1.0f));
    glUniformMatrix4fv(debugShader->modelUniform, 1, GL_FALSE, value_ptr(model));
    glDrawArrays(GL_LINES, 0, 2);
}
void Renderer::RenderLine(const vec2& origin, const vec2& destiny) {
    debugShader->Activate();
    glBindVertexArray(line_vao);
    mat4 model;
    model = glm::translate(model, vec3(origin, 0.0f));
    model = glm::scale(model, vec3(destiny - origin, 1.0f));
    glUniformMatrix4fv(debugShader->modelUniform, 1, GL_FALSE, value_ptr(model));
    glDrawArrays(GL_LINES, 0, 2);
}
void Renderer::RenderLine(const float& ax, const float& ay, const float& bx, const float& by) {
    RenderLine(vec2(ax, ay), vec2(bx, by));
}
void Renderer::RenderCircle(const float& radius, const vec2& position) {
    debugShader->Activate();
    glBindVertexArray(circle_vao);
    mat4 model;
    model = glm::translate(model, vec3(position, 0.0f));
    model = glm::scale(model, vec3(radius, radius, 1.0f));
    glUniformMatrix4fv(debugShader->modelUniform, 1, GL_FALSE, value_ptr(model));
    glDrawArrays(GL_LINES, 0, 120);
}
void Renderer::Deinitialize() {
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &sprite_vao);
    glDeleteBuffers(1, &sprite_vbo);
    glDeleteVertexArrays(1, &line_vao);
    glDeleteBuffers(1, &line_vbo);
    glDeleteVertexArrays(1, &point_vao);
    glDeleteBuffers(1, &point_vbo);
    delete spriteShader;
    delete debugShader;
}

void SpriteShader::Initialize() {
    shaderProgram = LoadShaders("assets/sprite.vertex", "assets/sprite.fragment");
    projectionUniform = glGetUniformLocation(shaderProgram, "projection");
    viewUniform = glGetUniformLocation(shaderProgram, "view");
    modelUniform = glGetUniformLocation(shaderProgram, "model");
    colorUniform = glGetUniformLocation(shaderProgram, "color");
}
void SpriteShader::Activate() const {
    glUseProgram(shaderProgram);
}

void DebugShader::Initialize() {
    shaderProgram = LoadShaders("assets/debug.vertex", "assets/debug.fragment");
    projectionUniform = glGetUniformLocation(shaderProgram, "projection");
    viewUniform = glGetUniformLocation(shaderProgram, "view");
    modelUniform = glGetUniformLocation(shaderProgram, "model");
}
void DebugShader::Activate() const {
    glUseProgram(shaderProgram);
}

Sprite::Sprite(uint texture, vec2 scale, Body* body) {
    this->texture = texture;
    this->scale = scale;
    this->color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    this->body = body;
}
Sprite::Sprite(uint texture, vec2 scale, Body* body, vec4 color) {
    this->texture = texture;
    this->scale = scale;
    this->color = color;
    this->body = body;
}

Sprite* Painter::AddSprite(uint texture, vec2 scale, Body* body) {
    assert(body);
    Sprite* sprite = new Sprite(texture, scale, body);
    sprites.push_back(sprite);
    return sprite;
}
Sprite* Painter::AddSprite(uint texture, vec2 scale, Body* body, vec4 color) {
    assert(body);
    Sprite* sprite = new Sprite(texture, scale, body, color);
    sprites.push_back(sprite);
    return sprite;
}
void Painter::Render() const {
    spriteShader->Activate();
    glBindVertexArray(sprite_vao);
    for (auto sprite : sprites) {
        glBindTexture(GL_TEXTURE_2D, sprite->texture);
        mat4 model;
        model = glm::translate(model, vec3(sprite->body->position, 0.0f));
        model = glm::scale(model, vec3(sprite->scale, 1.0f));
        model = glm::rotate(model, sprite->body->orientation, vec3(0.0f, 0.0f, 1.0f));
        glUniform4f(spriteShader->colorUniform, sprite->color.x, sprite->color.y, sprite->color.z, sprite->color.w);
        glUniformMatrix4fv(spriteShader->modelUniform, 1, GL_FALSE, value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}
void Painter::Clear() {
    for (auto sprite : sprites) {
        delete sprite;
    }
    sprites.clear();
}
