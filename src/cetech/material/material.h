#ifndef CETECH_MATERIAL_H
#define CETECH_MATERIAL_H

#include <corelib/cdb.h>



//==============================================================================
// Includes
//==============================================================================

#include <stdint.h>

//==============================================================================
// Typedefs
//==============================================================================

struct ct_cdb_obj_t;

#define MATERIAL_TYPE \
    CT_ID64_0("material", 0xeac0b497876adedfULL)

#define MATERIAL_SHADER_PROP \
    CT_ID64_0("shader", 0xcce8d5b5f5ae333fULL)

#define MATERIAL_STATE_PROP \
    CT_ID64_0("state", 0x82830aedd03d8beeULL)

#define MATERIAL_VARIABLES_PROP \
    CT_ID64_0("variables", 0x4fb1ab3fd540bd03ULL)

#define MATERIAL_VAR_HANDLER_PROP \
    CT_ID64_0("handler", 0xe08f763ff8f80d07ULL)

#define MATERIAL_VAR_NAME_PROP \
    CT_ID64_0("name", 0xd4c943cba60c270bULL)

#define MATERIAL_VAR_TYPE_PROP \
    CT_ID64_0("type", 0xa21bd0e01ac8f01fULL)

#define MATERIAL_VAR_VALUE_PROP \
    CT_ID64_0("value", 0x920b430f38928dc9ULL)

#define MATERIAL_ASSET_NAME \
    CT_ID64_0("asset_name", 0xf82d0a5475e3d5eaULL)

#define MATERIAL_LAYERS \
    CT_ID64_0("layers", 0x289219f856ee5cd6ULL)

#define MATERIAL_LAYER_NAME \
    CT_ID64_0("layer_name", 0x29aed2afaafefcc9ULL)

//==============================================================================
// Api
//==============================================================================

struct ct_material_a0 {
    uint64_t (*create)(uint64_t name);

    void (*submit)(uint64_t material,
                   uint64_t layer,
                   uint8_t viewid);

    void (*set_texture_handler)(uint64_t material,
                                uint64_t layer,
                                const char *slot,
                                struct ct_render_texture_handle texture);
};

CT_MODULE(ct_material_a0);

#endif //CETECH_MATERIAL_H