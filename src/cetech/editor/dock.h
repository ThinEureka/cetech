#ifndef CETECH_DOCK_H
#define CETECH_DOCK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>


#define CT_DOCK_API \
    CE_ID64_0("ct_dock_a0", 0x90cc2a5b4273931aULL)

#define DOCK_INTERFACE_NAME \
    "ct_dock_i0"

#define DOCK_INTERFACE \
    CE_ID64_0("ct_dock_i0", 0x3a3181c12a9e109fULL)

#define PROP_DOCK_VISIBLE\
    CE_ID64_0("visible", 0x24b77991b5d39298ULL)

#define PROP_DOCK_flags\
    CE_ID64_0("flags", 0x6def9a496b03ba91ULL)

#define PROP_DOCK_CONTEXT\
    CE_ID64_0("context", 0x1349eedc2838510ULL)

#define PROP_DOCK_SELECTED_OBJ\
    CE_ID64_0("selected_object", 0x7cc97924b2b963b1ULL)

typedef struct ct_dock_i0 {
    uint64_t (*cdb_type)();
    uint64_t (*dock_flags)();

    void (*open)(uint64_t dock);
    void (*close)(uint64_t dock);

    const char *(*display_title)(uint64_t dock);
    const char *(*name)(uint64_t dock);

    void (*draw_ui)(uint64_t dock);
    void (*draw_menu)(uint64_t dock);
    void (*draw_main_menu)(uint64_t dock);
} ct_dock_i0;


struct ct_dock_a0 {
    uint64_t (*create_dock)(uint64_t type,
                            bool visible);
    bool (*context_btn)(uint64_t dock);

    void (*draw_all)();
    void (*draw_menu)();
};

CE_MODULE(ct_dock_a0);

#ifdef __cplusplus
};
#endif

#endif //CETECH_DOCK_H
