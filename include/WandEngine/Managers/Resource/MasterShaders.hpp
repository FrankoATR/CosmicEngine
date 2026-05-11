#ifndef WAND_MASTERSHADERS_HPP
#define WAND_MASTERSHADERS_HPP

#include "../../Utils/Configurations.hpp"

#if defined(GAME_2D_CONFIGURATION)
    const char *sprite_vs = 
    "#version 330 core\n"
    "layout (location = 0) in vec4 vertex;\n"
    "out vec2 TexCoords;\n"
    "uniform mat4 model;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "TexCoords = vertex.zw;\n"
    "gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);\n"
    "}\0";

    const char *sprite_fs = 
    "#version 330 core\n"
    "in vec2 TexCoords;\n"
    "out vec4 color;\n"
    "uniform sampler2D image;\n"
    "uniform vec3 spriteColor;\n"
    "void main()\n"
    "{\n"
    "color = vec4(spriteColor, 1.0) * texture(image, TexCoords);\n"
    "}\0";

    const char *spriteSheet_vs = 
    "#version 330 core\n"
    "layout (location = 0) in vec4 vertex;\n"
    "out vec2 TexCoords;\n"
    "uniform mat4 model;\n"
    "uniform mat4 projection;\n"
    "uniform vec2 uvMin;\n"
    "uniform vec2 uvMax;\n"
    "void main()\n"
    "{\n"
    "TexCoords = mix(uvMin, uvMax, vertex.zw);\n"
    "gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);\n"
    "}\0";

    const char *spriteSheet_fs = 
    "#version 330 core\n"
    "in vec2 TexCoords;\n"
    "out vec4 color;\n"
    "uniform sampler2D image;\n"
    "uniform vec3 spriteColor;\n"
    "void main()\n"
    "{\n"
    "color = vec4(spriteColor, 1.0) * texture(image, TexCoords);\n"
    "}\0";

    const char *shape_vs = 
    "#version 330 core\n"
    "layout(location = 0) in vec3 aPos;\n"
    "uniform mat4 model;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "gl_Position = projection * model * vec4(aPos, 1.0);\n"
    "}\0";

    const char *shape_fs = 
    "#version 330 core\n"
    "uniform vec4 LineColor;\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "FragColor = LineColor;\n"
    "}\0";

    const char *text_vs =
    "#version 330 core\n"
    "layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>\n"
    "out vec2 TexCoords;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"
    "TexCoords = vertex.zw;\n"
    "}\0";

    const char *text_fs =
    "#version 330 core\n"
    "in vec2 TexCoords;\n"
    "out vec4 color;\n"
    "uniform sampler2D text;\n"
    "uniform vec3 textColor;\n"
    "void main()\n"
    "{\n"
    "vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
    "color = vec4(textColor, 1.0) * sampled;\n"
    "}\0";


#elif defined(GAME_3D_CONFIGURATION)
const char *sprite_vs = 
"#version 330 core\n"
"layout (location = 0) in vec4 vertex;\n"
"out vec2 TexCoords;\n"
"uniform mat4 model;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"TexCoords = vertex.zw;\n"
"gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);\n"
"}\0";

const char *sprite_fs = 
"#version 330 core\n"
"in vec2 TexCoords;\n"
"out vec4 color;\n"
"uniform sampler2D image;\n"
"uniform vec3 spriteColor;\n"
"void main()\n"
"{\n"
"color = vec4(spriteColor, 1.0) * texture(image, TexCoords);\n"
"}\0";

const char *spriteSheet_vs = 
"#version 330 core\n"
"layout (location = 0) in vec4 vertex;\n"
"out vec2 TexCoords;\n"
"uniform mat4 model;\n"
"uniform mat4 projection;\n"
"uniform vec2 uvMin;\n"
"uniform vec2 uvMax;\n"
"void main()\n"
"{\n"
"TexCoords = mix(uvMin, uvMax, vertex.zw);\n"
"gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);\n"
"}\0";

const char *spriteSheet_fs = 
"#version 330 core\n"
"in vec2 TexCoords;\n"
"out vec4 color;\n"
"uniform sampler2D image;\n"
"uniform vec3 spriteColor;\n"
"void main()\n"
"{\n"
"color = vec4(spriteColor, 1.0) * texture(image, TexCoords);\n"
"}\0";

const char *shape_vs = 
"#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"
"uniform mat4 model;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"}\0";

const char *shape_fs = 
"#version 330 core\n"
"uniform vec4 LineColor;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"FragColor = LineColor;\n"
"}\0";

const char *text_vs =
"#version 330 core\n"
"layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>\n"
"out vec2 TexCoords;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"
"TexCoords = vertex.zw;\n"
"}\0";

const char *text_fs =
"#version 330 core\n"
"in vec2 TexCoords;\n"
"out vec4 color;\n"
"uniform sampler2D text;\n"
"uniform vec3 textColor;\n"
"void main()\n"
"{\n"
"vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
"color = vec4(textColor, 1.0) * sampled;\n"
"}\0";


#else
    #error "[ObjectManager] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
#endif





#endif // WAND_MASTERSHADERS_HPP