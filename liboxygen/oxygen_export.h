
#ifndef OXYGEN_EXPORT_H
#define OXYGEN_EXPORT_H

#ifdef OXYGENSTYLE5_STATIC_DEFINE
#  define OXYGEN_EXPORT
#  define OXYGENSTYLE5_NO_EXPORT
#else
#  ifndef OXYGEN_EXPORT
#    ifdef oxygenstyle5_EXPORTS
        /* We are building this library */
#      define OXYGEN_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define OXYGEN_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef OXYGENSTYLE5_NO_EXPORT
#    define OXYGENSTYLE5_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef OXYGENSTYLE5_DEPRECATED
#  define OXYGENSTYLE5_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef OXYGENSTYLE5_DEPRECATED_EXPORT
#  define OXYGENSTYLE5_DEPRECATED_EXPORT OXYGEN_EXPORT OXYGENSTYLE5_DEPRECATED
#endif

#ifndef OXYGENSTYLE5_DEPRECATED_NO_EXPORT
#  define OXYGENSTYLE5_DEPRECATED_NO_EXPORT OXYGENSTYLE5_NO_EXPORT OXYGENSTYLE5_DEPRECATED
#endif

#define DEFINE_NO_DEPRECATED 0
#if DEFINE_NO_DEPRECATED
# define OXYGENSTYLE5_NO_DEPRECATED
#endif

#endif
