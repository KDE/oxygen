
#ifndef OXYGEN_CONFIG_EXPORT_H
#define OXYGEN_CONFIG_EXPORT_H

#ifdef OXYGENSTYLECONFIG5_STATIC_DEFINE
#  define OXYGEN_CONFIG_EXPORT
#  define OXYGENSTYLECONFIG5_NO_EXPORT
#else
#  ifndef OXYGEN_CONFIG_EXPORT
#    ifdef oxygenstyleconfig5_EXPORTS
        /* We are building this library */
#      define OXYGEN_CONFIG_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define OXYGEN_CONFIG_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef OXYGENSTYLECONFIG5_NO_EXPORT
#    define OXYGENSTYLECONFIG5_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef OXYGENSTYLECONFIG5_DEPRECATED
#  define OXYGENSTYLECONFIG5_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef OXYGENSTYLECONFIG5_DEPRECATED_EXPORT
#  define OXYGENSTYLECONFIG5_DEPRECATED_EXPORT OXYGEN_CONFIG_EXPORT OXYGENSTYLECONFIG5_DEPRECATED
#endif

#ifndef OXYGENSTYLECONFIG5_DEPRECATED_NO_EXPORT
#  define OXYGENSTYLECONFIG5_DEPRECATED_NO_EXPORT OXYGENSTYLECONFIG5_NO_EXPORT OXYGENSTYLECONFIG5_DEPRECATED
#endif

#define DEFINE_NO_DEPRECATED 0
#if DEFINE_NO_DEPRECATED
# define OXYGENSTYLECONFIG5_NO_DEPRECATED
#endif

#endif
