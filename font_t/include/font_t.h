#pragma once

#include <cglm/cglm.h>
#include <glad/gl.h>
#include <stb/stb_truetype.h>
#include <stdbool.h>

#include "asset_t.h"

#define FONT_ATLAS_W 512
#define FONT_ATLAS_H 512

#define ASCII_START ( 32 )
#define ASCII_END ( 126 )
#define CHARACTERS_COUNT ( ASCII_END - ( ASCII_START - 1 ) )

#define DEFAULT_FONT { .texture = 0, .height = 0.0 }

typedef struct {
    GLuint vbo;
    GLuint ebo;

    GLuint shader;
    GLuint texture;
    stbtt_bakedchar glyphs[ CHARACTERS_COUNT ];
    stbtt_fontinfo info;
    float height;
    vec3 color;
} font_t;

font_t font_t$create( void );
bool font_t$destroy( font_t* restrict _font );

bool font_t$load$fromAsset( font_t* restrict _font, asset_t* restrict _asset );
bool font_t$load$fromPath( font_t* restrict _font, const char* restrict _path );
bool font_t$unload( font_t* restrict _font );

#if 0
GLuint g_fontTex;
glGenTextures(1, &g_fontTex);
glBindTexture(GL_TEXTURE_2D, g_fontTex);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, FONT_BITMAP_W, FONT_BITMAP_H, 0, GL_RED, GL_UNSIGNED_BYTE, l_bitmap);

glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

void draw_text(const char *_text, float _x, float _y) {
    glBindTexture(GL_TEXTURE_2D, g_fontTex);
    glBegin(GL_QUADS); // or use VAOs/VBOs in modern OpenGL

    while (*_text) {
        if (*_text >= 32 && *_text < 128) {
            stbtt_aligned_quad l_q;
            stbtt_GetBakedQuad(g_cdata, FONT_BITMAP_W, FONT_BITMAP_H, *_text - 32, &_x, &_y, &l_q, 1);

            glTexCoord2f(l_q.s0, l_q.t0); glVertex2f(l_q.x0, l_q.y0);
            glTexCoord2f(l_q.s1, l_q.t0); glVertex2f(l_q.x1, l_q.y0);
            glTexCoord2f(l_q.s1, l_q.t1); glVertex2f(l_q.x1, l_q.y1);
            glTexCoord2f(l_q.s0, l_q.t1); glVertex2f(l_q.x0, l_q.y1);
        }
        _text++;
    }

    glEnd();
}
#endif
#if 0
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#define FONT_ATLAS_W 512
#define FONT_ATLAS_H 512

unsigned char *g_fontFile = NULL;
unsigned char g_fontBitmap[FONT_ATLAS_W * FONT_ATLAS_H];
stbtt_bakedchar g_fontChars[96]; // ASCII 32..126
GLuint g_fontTex;

void load_font(const char *_path, float _pixelHeight) {
    FILE *l_file = fopen(_path, "rb");
    fseek(l_file, 0, SEEK_END);
    int l_size = ftell(l_file);
    fseek(l_file, 0, SEEK_SET);
    g_fontFile = malloc(l_size);
    fread(g_fontFile, 1, l_size, l_file);
    fclose(l_file);

    stbtt_BakeFontBitmap(g_fontFile, 0, _pixelHeight, g_fontBitmap, FONT_ATLAS_W, FONT_ATLAS_H, 32, 96, g_fontChars);

    glGenTextures(1, &g_fontTex);
    glBindTexture(GL_TEXTURE_2D, g_fontTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, FONT_ATLAS_W, FONT_ATLAS_H, 0, GL_RED, GL_UNSIGNED_BYTE, g_fontBitmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

// text vert
#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;

uniform mat4 u_proj;

out vec2 vUV;

void main() {
    vUV = aUV;
    gl_Position = u_proj * vec4(aPos, 0.0, 1.0);
}

// text frag
#version 330 core
in vec2 vUV;
out vec4 fragColor;

uniform sampler2D u_font;
uniform vec3 u_color;

void main() {
    float alpha = texture(u_font, vUV).r;
    fragColor = vec4(u_color, alpha);
}

GLuint g_textVAO, g_textVBO;

void init_text_rendering() {
    glGenVertexArrays(1, &g_textVAO);
    glGenBuffers(1, &g_textVBO);

    glBindVertexArray(g_textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, g_textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0); // pos
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);

    glEnableVertexAttribArray(1); // uv
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void draw_text(GLuint _shader, const char *_text, float _x, float _y, float _scale, vec3 _color, mat4 _proj) {
    glUseProgram(_shader);
    glUniform3fv(glGetUniformLocation(_shader, "u_color"), 1, &_color[0]);
    glUniformMatrix4fv(glGetUniformLocation(_shader, "u_proj"), 1, GL_FALSE, &_proj[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(g_textVAO);
    glBindTexture(GL_TEXTURE_2D, g_fontTex);

    while (*_text) {
        if (*_text < 32 || *_text >= 128) {
            _text++;
            continue;
        }

        stbtt_aligned_quad l_q;
        stbtt_GetBakedQuad(g_fontChars, FONT_ATLAS_W, FONT_ATLAS_H, *_text - 32, &_x, &_y, &l_q, 1);

        float l_vertices[6][4] = {
            { l_q.x0, l_q.y0, l_q.s0, l_q.t0 },
            { l_q.x1, l_q.y0, l_q.s1, l_q.t0 },
            { l_q.x1, l_q.y1, l_q.s1, l_q.t1 },

            { l_q.x0, l_q.y0, l_q.s0, l_q.t0 },
            { l_q.x1, l_q.y1, l_q.s1, l_q.t1 },
            { l_q.x0, l_q.y1, l_q.s0, l_q.t1 },
        };

        glBindBuffer(GL_ARRAY_BUFFER, g_textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(l_vertices), l_vertices);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        _text++;
    }

    glBindVertexArray(0);
    glUseProgram(0);
}

mat4 l_proj = ortho(0.0f, screen_width, screen_height, 0.0f, -1.0f, 1.0f);
vec3 l_color = {1.0f, 1.0f, 1.0f};
draw_text(g_shader, "Hello World", 50.0f, 100.0f, 1.0f, l_color, l_proj);
#endif
