#include <glad/gl.h>

#include "callbacks.h"
#include "font_t.h"
#include "log.h"
#include "stdfunc.h"

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
            l_font.color[ 1 ] = 1;
            l_font.color[ 2 ] = 1;
            l_font.height = 40;

            if ( UNLIKELY( !font_t$load$fromPath( &l_font, "t.ttf" ) ) ) {
                log$transaction$query( ( logLevel_t )error, "font1\n" );
            }
        }
    }

    // Render
    {
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        const char* text = "Hello, world!";
        const size_t windowWidth = _applicationState->settings.window.width;
        const size_t windowHeight = _applicationState->settings.window.height;

        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();
        glOrtho( 0, windowWidth, windowHeight, 0, -1, 1 ); // top-left origin
        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();

        glEnable( GL_TEXTURE_2D );
        glBindTexture( GL_TEXTURE_2D, l_font.texture );
        glEnable( GL_BLEND );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        glColor3f( l_font.color[ 0 ], l_font.color[ 1 ], l_font.color[ 2 ] );

        float x = 0.0f;
        float y = windowHeight / 2.0f; // vertical center baseline

        // Compute text width to center horizontally
        float textWidth = 0.0f;
        for ( const char* p = text; *p; ++p ) {
            if ( *p >= ASCII_START && *p <= ASCII_END ) {
                int glyphIndex = *p - ASCII_START;
                textWidth += l_font.glyphs[ glyphIndex ].xadvance;
            }
        }

        x = ( windowWidth - textWidth ) / 2.0f;

        glBegin( GL_QUADS );
        {
            int index = 'A' - ASCII_START;

            stbtt_aligned_quad q;
            float x = 20.0f, y = 30.0f;

            stbtt_GetPackedQuad( l_font.glyphs, l_font.atlasWidth,
                                 l_font.atlasHeight, index, &x, &y, &q, 1 );

            glTexCoord2f( q.s0, q.t0 );
            glVertex2f( q.x0, q.y0 );
            glTexCoord2f( q.s1, q.t0 );
            glVertex2f( q.x1, q.y0 );
            glTexCoord2f( q.s1, q.t1 );
            glVertex2f( q.x1, q.y1 );
            glTexCoord2f( q.s0, q.t1 );
            glVertex2f( q.x0, q.y1 );
        }

        for ( const char* p = text; *p; ++p ) {
            if ( *p < ASCII_START || *p > ASCII_END )
                continue;

            int glyphIndex = *p - ASCII_START;

            if ( p > text ) {
                int prevGlyphIndex = *( p - 1 ) - ASCII_START;
                // Apply kerning using stbtt_GetCodepointKernAdvance
                int kernAdvance = stbtt_GetCodepointKernAdvance(
                    &l_font.info, *( p - 1 ), *p );
                x += kernAdvance; // Apply kerning adjustment
            }

            stbtt_aligned_quad q;
            stbtt_GetPackedQuad( l_font.glyphs, l_font.atlasWidth,
                                 l_font.atlasHeight, glyphIndex, &x, &y, &q,
                                 1 );

            glTexCoord2f( q.s0, q.t0 );
            glVertex2f( q.x0, q.y0 );
            glTexCoord2f( q.s1, q.t0 );
            glVertex2f( q.x1, q.y0 );
            glTexCoord2f( q.s1, q.t1 );
            glVertex2f( q.x1, q.y1 );
            glTexCoord2f( q.s0, q.t1 );
            glVertex2f( q.x0, q.y1 );
        }
        glEnd();

        glDisable( GL_BLEND );

        glfwSwapBuffers( _applicationState->window );
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
