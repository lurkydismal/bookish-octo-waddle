#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION

#include "font_t.h"

#include <stb/stb_truetype.h>

font_t font_t$create( void ) {
    font_t l_returnValue = DEFAULT_FONT;

    return ( l_returnValue );
}

bool font_t$destroy( font_t* _font ) {
    bool l_returnValue = false;

    {
        l_returnValue = true;
    }

    return ( l_returnValue );
}

bool font_t$load$fromAsset( font_t* _font, asset_t* _asset ) {
    bool l_returnValue = false;

    {
        l_returnValue = true;
    }

    return ( l_returnValue );
}

bool font_t$load$fromPath( font_t* _font, const char* _path ) {
    bool l_returnValue = false;

    {
        l_returnValue = true;
    }

    return ( l_returnValue );
}

bool font_t$unload( font_t* _font ) {
    bool l_returnValue = false;

    {
        l_returnValue = true;
    }

    return ( l_returnValue );
}
