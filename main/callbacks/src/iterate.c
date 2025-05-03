#include <glad/gl.h>

#include "callbacks.h"
#include "log.h"
#include "stdfunc.h"
#include "texture_t.h"

callbackResult_t iterate$unlimited(
    applicationState_t* restrict _applicationState ) {
    callbackResult_t l_returnValue = ( callbackResult_t )failure;

    static texture_t t;
    static bool once = false;
    static GLuint g_shaderProgram = 0;
    static GLuint g_vao = 0;
    static GLuint g_vbo = 0;

    if ( !once ) {
        once = true;

        // Load texture
        t = texture_t$create();
        if ( !texture_t$load$fromPath( &t, "t.ktx2" ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Failed to load texture\n" );
            goto EXIT;
        }

        // Setup OpenGL sampling state
        glBindTexture( t.type, t.texture );
        glTexParameteri( t.type, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        glTexParameteri( t.type, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        glTexParameteri( t.type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glTexParameteri( t.type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

        // Swizzle single-channel RED to RGB (to avoid red-tinted grayscale)
        glTexParameteri( t.type, GL_TEXTURE_SWIZZLE_R, GL_RED );
        glTexParameteri( t.type, GL_TEXTURE_SWIZZLE_G, GL_RED );
        glTexParameteri( t.type, GL_TEXTURE_SWIZZLE_B, GL_RED );

        // Set white clear color
        glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );

        // Shader sources
        const char* l_vertSrc =
            "#version 330 core\n"
            "layout(location = 0) in vec2 aPos;\n"
            "layout(location = 1) in vec2 aUV;\n"
            "out vec2 vUV;\n"
            "void main() {\n"
            "    vUV = aUV;\n"
            "    gl_Position = vec4(aPos, 0.0, 1.0);\n"
            "}\n";

        const char* l_fragSrc =
            "#version 330 core\n"
            "in vec2 vUV;\n"
            "out vec4 fragColor;\n"
            "uniform sampler2D uTex;\n"
            "void main() {\n"
            "    fragColor = texture(uTex, vUV);\n"
            "}\n";

        // Compile vertex shader
        GLuint l_vert = glCreateShader( GL_VERTEX_SHADER );
        glShaderSource( l_vert, 1, &l_vertSrc, NULL );
        glCompileShader( l_vert );

        // Compile fragment shader
        GLuint l_frag = glCreateShader( GL_FRAGMENT_SHADER );
        glShaderSource( l_frag, 1, &l_fragSrc, NULL );
        glCompileShader( l_frag );

        // Link shader program
        g_shaderProgram = glCreateProgram();
        glAttachShader( g_shaderProgram, l_vert );
        glAttachShader( g_shaderProgram, l_frag );
        glLinkProgram( g_shaderProgram );
        glDeleteShader( l_vert );
        glDeleteShader( l_frag );

        // Setup fullscreen quad
        const float l_quadVerts[] = {
            -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,  -1.0f, 1.0f, 0.0f,
            1.0f,  1.0f,  1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f,  1.0f,  1.0f, 1.0f, -1.0f, 1.0f,  0.0f, 1.0f,
        };

        glGenVertexArrays( 1, &g_vao );
        glGenBuffers( 1, &g_vbo );
        glBindVertexArray( g_vao );
        glBindBuffer( GL_ARRAY_BUFFER, g_vbo );
        glBufferData( GL_ARRAY_BUFFER, sizeof( l_quadVerts ), l_quadVerts,
                      GL_STATIC_DRAW );

        glEnableVertexAttribArray( 0 );
        glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof( float ),
                               ( void* )( 0 ) );

        glEnableVertexAttribArray( 1 );
        glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof( float ),
                               ( void* )( 2 * sizeof( float ) ) );

        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        glBindVertexArray( 0 );
    }

    // Render
    {
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        glUseProgram( g_shaderProgram );

        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( t.type, t.texture );
        glUniform1i( glGetUniformLocation( g_shaderProgram, "uTex" ), 0 );

        glBindVertexArray( g_vao );
        glDrawArrays( GL_TRIANGLES, 0, 6 );
        glBindVertexArray( 0 );

        glfwSwapBuffers( _applicationState->window );
    }

    _applicationState->totalFramesRendered++;
    l_returnValue = ( callbackResult_t )remain;

EXIT:
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
