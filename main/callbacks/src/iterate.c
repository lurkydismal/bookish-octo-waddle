#include <glad/gl.h>

#include "callbacks.h"
#include "image_t.h"
#include "log.h"
#include "stdfunc.h"

callbackResult_t iterate$unlimited( applicationState_t* _applicationState ) {
    callbackResult_t l_returnValue = ( callbackResult_t )failure;

    static bool x = false;
    static image_t t;
    static GLuint shaderProgram = 0;
    static GLuint vao = 0;
    static GLuint textureID = 0;

    if ( !x ) {
        x = true;

        t = image_t$create();

        if ( !image_t$load$fromPath( &t, "t.png" ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "image load from path\n" );
        }

        // Create texture
        glGenTextures( 1, &textureID );
        glBindTexture( GL_TEXTURE_2D, textureID );
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, t.width, t.height, 0, GL_RGBA,
                      GL_UNSIGNED_BYTE, t.data );
        glGenerateMipmap( GL_TEXTURE_2D );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

        // Free CPU-side image data
        image_t$unload( &t );

        // Vertex and fragment shader source
        const char* vs =
            "#version 330 core\n"
            "layout(location = 0) in vec2 pos;\n"
            "layout(location = 1) in vec2 uv;\n"
            "out vec2 texCoord;\n"
            "void main() {\n"
            "  texCoord = uv;\n"
            "  gl_Position = vec4(pos, 0.0, 1.0);\n"
            "}";

        const char* fs =
            "#version 330 core\n"
            "in vec2 texCoord;\n"
            "out vec4 color;\n"
            "uniform sampler2D tex;\n"
            "void main() {\n"
            "color = texture(tex, texCoord) * vec4(1.0, 0.0, 0.0, 1.0);\n"
            "}";
#if 0
            "  color = texture(tex, texCoord);\n"
#endif

        GLuint vsh = glCreateShader( GL_VERTEX_SHADER );
        glShaderSource( vsh, 1, &vs, NULL );
        glCompileShader( vsh );

        GLuint fsh = glCreateShader( GL_FRAGMENT_SHADER );
        glShaderSource( fsh, 1, &fs, NULL );
        glCompileShader( fsh );

        shaderProgram = glCreateProgram();
        glAttachShader( shaderProgram, vsh );
        glAttachShader( shaderProgram, fsh );
        glLinkProgram( shaderProgram );

        // Check vertex shader
        GLint success;
        glGetShaderiv( vsh, GL_COMPILE_STATUS, &success );
        if ( !success ) {
            char infoLog[ 512 ];
            glGetShaderInfoLog( vsh, 512, NULL, infoLog );
            log$transaction$query$format( ( logLevel_t )debug,
                                          "Vertex Shader Compile Error: %s\n",
                                          infoLog );
        }

        // Check fragment shader
        glGetShaderiv( fsh, GL_COMPILE_STATUS, &success );
        if ( !success ) {
            char infoLog[ 512 ];
            glGetShaderInfoLog( fsh, 512, NULL, infoLog );
            log$transaction$query$format( ( logLevel_t )debug,
                                          "Fragment Shader Compile Error: %s\n",
                                          infoLog );
        }

        // Check program link
        glGetProgramiv( shaderProgram, GL_LINK_STATUS, &success );
        if ( !success ) {
            char infoLog[ 512 ];
            glGetProgramInfoLog( shaderProgram, 512, NULL, infoLog );
            log$transaction$query$format( ( logLevel_t )debug,
                                          "Shader Program Link Error: %s\n",
                                          infoLog );
        }

        glDeleteShader( vsh );
        glDeleteShader( fsh );

        // Fullscreen quad (clip space coords)
        float vertices[] = { // pos     // uv
                             -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f,
                             1.0f,  0.0f,  1.0f, 1.0f, 1.0f, 1.0f,
                             -1.0f, 1.0f,  0.0f, 1.0f };

        unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

        GLuint vbo, ebo;
        glGenVertexArrays( 1, &vao );
        glGenBuffers( 1, &vbo );
        glGenBuffers( 1, &ebo );

        glBindVertexArray( vao );

        glBindBuffer( GL_ARRAY_BUFFER, vbo );
        glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices,
                      GL_STATIC_DRAW );

        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( indices ), indices,
                      GL_STATIC_DRAW );

        glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof( float ),
                               ( void* )0 );
        glEnableVertexAttribArray( 0 );
        glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof( float ),
                               ( void* )( 2 * sizeof( float ) ) );
        glEnableVertexAttribArray( 1 );

        glBindVertexArray( 0 );
    }

    {
        int width, height;
        glfwGetFramebufferSize( _applicationState->window, &width, &height );
        glViewport( 0, 0, width, height );

        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        glBindFramebuffer( GL_FRAMEBUFFER, 0 );
        glDisable( GL_DEPTH_TEST );
        glDisable( GL_CULL_FACE );

        glUseProgram( shaderProgram );
        glUniform1i( glGetUniformLocation( shaderProgram, "tex" ),
                     0 ); // use texture unit 0
        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, textureID );
        glBindVertexArray( vao );
        glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );

        glfwSwapBuffers( _applicationState->window );
    }

    _applicationState->totalFramesRendered++;

    l_returnValue = ( callbackResult_t )remain;

    return l_returnValue;
}

callbackResult_t iterate$limited( applicationState_t* _applicationState ) {
    callbackResult_t l_returnValue = ( callbackResult_t )failure;

    const bool l_logTransactionResult = log$transaction$commit();

    if ( UNLIKELY( !l_logTransactionResult ) ) {
        l_returnValue = ( callbackResult_t )failure;

    } else {
        l_returnValue = ( callbackResult_t )remain;
    }

    return ( l_returnValue );
}
