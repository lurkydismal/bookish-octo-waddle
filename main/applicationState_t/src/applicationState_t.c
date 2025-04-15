#include "applicationState_t.h"

applicationState_t applicationState_t$create( void ) {
    applicationState_t l_returnValue = DEFAULT_APPLICATION_STATE;

    return ( l_returnValue );
}

bool applicationState_t$destroy( applicationState_t* _applicationState ) {
    return ( true );
}
