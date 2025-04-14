#include <cglm/cglm.h>
#include <glad/gl.h>

#include "callbacks.h"
#include "font_t.h"
#include "log.h"
#include "stdfunc.h"

#if 0
const char* vertexShaderSource =
    "#version 130\n"
    "in vec2 aPos;             // Position of the character quad\n"
    "in vec2 aTexCoords;       // Texture coordinates for the character\n"
    "out vec2 TexCoords;       // Output texture coordinates to fragment "
    "shader\n"
    "uniform mat4 projection;   // Projection matrix\n"
    "void main() {\n"
    "    gl_Position = projection * vec4(aPos, 0.0, 1.0); // Apply the "
    "projection matrix to the position\n"
    "    TexCoords = aTexCoords; // Pass texture coordinates\n"
    "}\n";

const char* fragmentShaderSource =
    "#version 130\n"
    "in vec2 TexCoords;        // Texture coordinates from vertex shader\n"
    "out vec4 FragColor;       // Output fragment color\n"
    "uniform sampler2D text;   // Font texture\n"
    "void main() {\n"
    "    vec4 color = texture(text, TexCoords);\n"
    "    if (color.a < 0.1) {\n"
    "        discard; // If the character has no color (transparent), don't "
    "draw it\n"
    "    }\n"
    "    FragColor = color; // Output the color of the character\n"
    "}\n";
#endif

const char* vertexShaderSource =
    "#version 120\n"
    "attribute vec2 aPos;           // Position of the character quad\n"
    "attribute vec2 aTexCoords;     // Texture coordinates for the character\n"
    "varying vec2 TexCoords;        // Output texture coordinates to fragment "
    "shader\n"
    "uniform mat4 projection;       // Projection matrix\n"
    "void main() {\n"
    "    gl_Position = projection * vec4(aPos, 0.0, 1.0); // Apply the "
    "projection matrix to the position\n"
    "    TexCoords = aTexCoords;     // Pass texture coordinates\n"
    "}\n";

const char* fragmentShaderSource =
    "#version 120\n"
    "varying vec2 TexCoords;        // Texture coordinates from vertex shader\n"
    "uniform sampler2D text;        // Font texture\n"
    "void main() {\n"
    "    vec4 color = texture2D(text, TexCoords);\n"
    "    if (color.a < 0.1) {\n"
    "        discard; // If the character has no color (transparent), don't "
    "draw it\n"
    "    }\n"
    "    gl_FragColor = color; // Output the color of the character\n"
    "}\n";

GLuint compileShader( GLenum type, const char* source ) {
    GLuint shader = glCreateShader( type );
    glShaderSource( shader, 1, &source, NULL );
    glCompileShader( shader );

    // Check for compilation errors
    GLint success;
    glGetShaderiv( shader, GL_COMPILE_STATUS, &success );
    if ( !success ) {
        GLint logLength;
        glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logLength );
        char errorLog[ logLength ];
        glGetShaderInfoLog( shader, logLength, &logLength, errorLog );
        log$transaction$query$format( ( logLevel_t )error,
                                      "ERROR::SHADER::COMPILATION_FAILED\n%s\n",
                                      errorLog );
    }
    return shader;
}

GLuint createShaderProgram() {
    GLuint vertexShader = compileShader( GL_VERTEX_SHADER, vertexShaderSource );
    GLuint fragmentShader =
        compileShader( GL_FRAGMENT_SHADER, fragmentShaderSource );

    GLuint shaderProgram = glCreateProgram();
    glAttachShader( shaderProgram, vertexShader );
    glAttachShader( shaderProgram, fragmentShader );
    glLinkProgram( shaderProgram );

    GLint success;
    glGetProgramiv( shaderProgram, GL_LINK_STATUS, &success );
    if ( !success ) {
        GLint logLength;
        glGetProgramiv( shaderProgram, GL_INFO_LOG_LENGTH, &logLength );
        char errorLog[ logLength ];
        glGetProgramInfoLog( shaderProgram, logLength, &logLength, errorLog );
        log$transaction$query$format(
            ( logLevel_t )error, "ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n",
            errorLog );
    }

    glDeleteShader( vertexShader );
    glDeleteShader( fragmentShader );

    return shaderProgram;
}

void renderText( font_t* font,
                 const char* text,
                 float x,
                 float y,
                 float scale ) {
    glUseProgram( font->shader );

    // Set the projection matrix
    mat4 projection;
    glm_ortho( 0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f, projection );
    GLuint projectionLoc = glGetUniformLocation( font->shader, "projection" );
    glUniformMatrix4fv( projectionLoc, 1, GL_TRUE, ( float* )&projection );

    // Set text color
    GLuint colorLoc = glGetUniformLocation( font->shader, "textColor" );
    glUniform3f( colorLoc, font->color[ 0 ], font->color[ 1 ],
                 font->color[ 2 ] );

    // Bind texture
    glBindTexture( GL_TEXTURE_2D, font->texture );

    // Loop through each character in the text
    for ( size_t i = 0; i < __builtin_strlen( text ); i++ ) {
        char c = text[ i ];
        if ( c < 32 || c > 126 )
            continue; // Skip unsupported characters

        // Calculate character quad dimensions and positions
        int xadvance = ( int )( font->glyphs[ ( unsigned char )c ].xadvance );
        float xpos = x + i * xadvance * scale;
        float ypos = y;

        float w = ( font->glyphs[ ( unsigned char )c ].x1 -
                    font->glyphs[ ( unsigned char )c ].x0 ) *
                  scale;
        float h = ( font->glyphs[ ( unsigned char )c ].y1 -
                    font->glyphs[ ( unsigned char )c ].y0 ) *
                  scale;

        // Update the texture and render the quad for this character
        glBindTexture( GL_TEXTURE_2D, font->texture );

        // Create a quad (vertex positions and texture coordinates) for
        // rendering the character
        GLfloat vertices[ 6 ][ 4 ] = {
            { xpos, ypos + h, 0.0f, 0.0f }, { xpos + w, ypos, 1.0f, 1.0f },
            { xpos, ypos, 0.0f, 1.0f },

            { xpos, ypos + h, 0.0f, 0.0f }, { xpos + w, ypos + h, 1.0f, 0.0f },
            { xpos + w, ypos, 1.0f, 1.0f } };

        glGenBuffers( 1, &font->vbo );
        glGenBuffers( 1, &font->ebo );

        if ( font->vbo == 0 || font->ebo == 0 ) {
            log$transaction$query$format(
                ( logLevel_t )error,
                "VBO or EBO not initialized correctly.\n" );
        }

        // Render the character quad here
        glBindBuffer( GL_ARRAY_BUFFER, font->vbo );

        glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof( GLfloat ),
                               ( void* )0 );
        glEnableVertexAttribArray( 0 );

        glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof( GLfloat ),
                               ( void* )( 2 * sizeof( GLfloat ) ) );
        glEnableVertexAttribArray( 1 );

        glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( vertices ), vertices );

        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, font->ebo );
        glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );

        // Advance to the next character
        x += xadvance * scale;
    }
}

callbackResult_t iterate$unlimited(
    applicationState_t* restrict _applicationState ) {
    callbackResult_t l_returnValue = ( callbackResult_t )failure;

    static bool x = false;
    static font_t l_font;

    if ( !x ) {
        x = true;

        {
            l_font = font_t$create();

            l_font.color[ 0 ] = 1;
            l_font.color[ 1 ] = 0;
            l_font.color[ 2 ] = 1;
            l_font.height = 20;

            if ( UNLIKELY( !font_t$load$fromPath( &l_font, "t.ttf" ) ) ) {
                log$transaction$query( ( logLevel_t )error, "font1\n" );
            }

            l_font.shader = createShaderProgram();
        }
    }

    {
        int width, height;
        glfwGetFramebufferSize( _applicationState->window, &width, &height );
        glViewport( 0, 0, width, height );

        // Render
        {
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

            renderText( &l_font, "Hello, OpenGL!", 50.0f, 100.0f, 1.0f );

            glfwSwapBuffers( _applicationState->window );
        }
    }

    _applicationState->totalFramesRendered++;

    l_returnValue = ( callbackResult_t )remain;

    return ( l_returnValue );
}

callbackResult_t iterate$limited(
    applicationState_t* restrict _applicationState ) {
    callbackResult_t l_returnValue = ( callbackResult_t )failure;

    const bool l_logTransactionResult = log$transaction$commit();

    if ( UNLIKELY( !l_logTransactionResult ) ) {
        l_returnValue = ( callbackResult_t )failure;

    } else {
        l_returnValue = ( callbackResult_t )remain;
    }

    return ( l_returnValue );
}
