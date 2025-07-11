#pragma once

#define EXPORT

#if defined( __cplusplus )

#undef EXPORT
#define EXPORT extern "C"

#if !defined( restrict )

#define restrict __restrict

#endif

#endif
