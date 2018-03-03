#ifndef CETECH_TEXTURE_H
#define CETECH_TEXTURE_H

#ifdef __cplusplus
extern "C" {
#endif

//==============================================================================
// Includes
//==============================================================================

#include <stdint.h>

//==============================================================================
// Typedefs
//==============================================================================

//==============================================================================
// Api
//==============================================================================

struct ct_texture {
    uint16_t idx;
};

//==============================================================================
// Api
//==============================================================================

//! Texture API V0
struct ct_texture_a0 {
    struct ct_texture (*get)(uint32_t name);
};

#ifdef __cplusplus
}
#endif

#endif //CETECH_TEXTURE_H