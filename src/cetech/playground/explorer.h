#ifndef CETECH_LEVEL_INSPECTOR_H
#define CETECH_LEVEL_INSPECTOR_H

#ifdef __cplusplus
extern "C" {
#endif

//==============================================================================
// Includes
//==============================================================================

#include <stddef.h>


struct ct_entity;
struct ct_world;

//==============================================================================
// Typedefs
//==============================================================================
typedef void (*ct_li_on_entity)(struct ct_world world,
                                struct ct_entity entity,
                                const char *filename,
                                ct_cdb_obj_t* obj);

//==============================================================================
// Api
//==============================================================================

struct ct_explorer_a0 {
    void (*set_level)(ct_world world,
                      ct_entity level,
                      uint64_t name,
                      uint64_t root,
                      const char *path);

    void (*register_on_entity_click)(ct_li_on_entity on_entity);

    void (*unregister_on_entity_click)(ct_li_on_entity on_entity);
};

#ifdef __cplusplus
}
#endif

#endif //CETECH_LEVEL_INSPECTOR_H
