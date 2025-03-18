#include "asset_t.h"

#include <fcntl.h>
#include <liburing.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "log.h"
#include "stdfunc.h"

// Max number of concurrent I/O operations
#define QUEUE_DEPTH 8

static bool g_isInitialized = false;
static struct io_uring g_ring;

static FORCE_INLINE void init_asset_loader( void ) {
    io_uring_queue_init( QUEUE_DEPTH, &g_ring, 0 );
}

static FORCE_INLINE void cleanup_asset_loader() {
    io_uring_queue_exit( &g_ring );
}

static FORCE_INLINE int check_asset_loaded( asset_t* _asset ) {
    struct io_uring_cqe* cqe;

    int ret = io_uring_peek_cqe( &g_ring, &cqe );

    if ( ret == 0 && cqe ) {
        asset_t* loaded_asset = io_uring_cqe_get_data( cqe );
        io_uring_cqe_seen( &g_ring, cqe );
        return 1;
    }

    return 0;
}

asset_t asset_t$create( void ) {}

void asset_t$destroy( asset_t* _asset ) {}

void asset_t$load( asset_t* _asset, const char* _path ) {
    int fd = open( _path, O_RDONLY );

    if ( fd < 0 ) {
        printf( "Failed to open asset: %s\n", _path );

        return;
    }

    // Get file size
    off_t file_size = lseek( fd, 0, SEEK_END );
    lseek( fd, 0, SEEK_SET );

    _asset->data = ( uint8_t* )malloc( file_size );
    _asset->size = file_size;

    struct io_uring_sqe* sqe = io_uring_get_sqe( &g_ring );
    io_uring_prep_read( sqe, fd, _asset->data, file_size, 0 );
    io_uring_sqe_set_data( sqe, _asset );

    io_uring_submit( &g_ring );
    close( fd );
}

void asset_t$unload( asset_t* _asset ) {}
