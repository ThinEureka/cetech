#include <stdio.h>
#include <string.h>

#include <celib/memory/allocator.h>
#include <celib/cdb.h>
#include <celib/ydb.h>
#include <celib/math/math.h>

#include <celib/macros.h>
#include "celib/id.h"
#include "celib/memory/memory.h"
#include "celib/api.h"
#include "celib/module.h"

#include <cetech/ecs/ecs.h>
#include <cetech/renderer/renderer.h>
#include <cetech/renderer/gfx.h>
#include <cetech/debugui/debugui.h>
#include <cetech/camera/camera.h>
#include <cetech/transform/transform.h>
#include <cetech/controlers/keyboard.h>
#include <cetech/resource/resource_browser.h>
#include <cetech/explorer/explorer.h>
#include <cetech/editor/editor.h>
#include <cetech/resource/resource.h>
#include <cetech/render_graph/render_graph.h>
#include <cetech/default_rg/default_rg.h>
#include <cetech/debugui/icons_font_awesome.h>
#include <cetech/debugui/debugui.h>
#include <cetech/editor/dock.h>
#include <cetech/controlers/controlers.h>
#include <cetech/resource/resource_editor.h>
#include <cetech/editor/selcted_object.h>
#include <celib/containers/array.h>

#define MAX_EDITOR 8

#define _G entity_editor_globals

typedef struct entity_editor {
    ct_world_t0 world;
    ct_entity_t0 camera_ent;
    ct_entity_t0 entity;
    ct_viewport_t0 viewport;

    bool mouse_hovering;
    bool free;
} entity_editor;

static struct _G {
    entity_editor *editors;
} _G;


#define _EDITOR_IDX \
    CE_ID64_0("editor_idx", 0x1b333d78a2ccaab7ULL)


static void draw_menu(uint64_t context_obj) {
    static enum OPERATION operation;
    ct_debugui_a0->RadioButton2(ICON_FA_ARROWS_ALT,
                                (int *) &operation, TRANSLATE);

    ct_debugui_a0->SameLine(0, 0);
    ct_debugui_a0->RadioButton2(ICON_FA_UNDO,
                                (int *) &operation, ROTATE);

    ct_debugui_a0->SameLine(0, 0);
    ct_debugui_a0->RadioButton2(ICON_FA_ARROWS_H,
                                (int *) &operation, SCALE);
}

static void draw_editor(uint64_t context_obj,
                        uint64_t context) {
    const ce_cdb_obj_o0 *reader = ce_cdb_a0->read(ce_cdb_a0->db(), context_obj);

    uint64_t editor_idx = ce_cdb_a0->read_uint64(reader, _EDITOR_IDX, 0);
    entity_editor *editor = &_G.editors[editor_idx];

    if (!editor->world.h) {
        return;
    }

    ce_vec2_t size = ct_debugui_a0->GetContentRegionAvail();
//    size.y -= ct_debugui_a0->GetTextLineHeightWithSpacing();

    bool is_mouse_hovering = ct_debugui_a0->IsMouseHoveringWindow();
    editor->mouse_hovering = is_mouse_hovering;


    ct_rg_builder_t0 *builder = ct_renderer_a0->viewport_builder(editor->viewport);

    ct_renderer_a0->viewport_set_size(editor->viewport, size);

    bgfx_texture_handle_t th;
    th = builder->get_texture(builder, RG_OUTPUT_TEXTURE);

    ct_debugui_a0->Image(th,
                         &size,
                         &(ce_vec4_t) {1.0f, 1.0f, 1.0f, 1.0f},
                         &(ce_vec4_t) {0.0f, 0.0f, 0.0, 0.0f});

}

static struct entity_editor *_new_editor(uint64_t context_obj) {

    const uint32_t n = ce_array_size(_G.editors);
    for (uint32_t i = 0; i < n; ++i) {
        struct entity_editor *e = &_G.editors[i];

        if (!e->free) {
            continue;
        }

        return e;
    }

    uint32_t idx = n;
    ce_array_push(_G.editors, (entity_editor) {}, ce_memory_a0->system);

    ce_cdb_obj_o0 *w = ce_cdb_a0->write_begin(ce_cdb_a0->db(), context_obj);
    ce_cdb_a0->set_uint64(w, _EDITOR_IDX, idx);
    ce_cdb_a0->write_commit(w);

    entity_editor *editor = &_G.editors[idx];

    editor->world = ct_ecs_a0->create_world();

    editor->camera_ent = ct_ecs_a0->spawn(editor->world, 0x57899875c4457313);
    editor->viewport = ct_renderer_a0->create_viewport();

    return editor;
}

static void close(uint64_t context_obj) {
    const ce_cdb_obj_o0 *reader = ce_cdb_a0->read(ce_cdb_a0->db(), context_obj);
    uint64_t editor_idx = ce_cdb_a0->read_uint64(reader, _EDITOR_IDX, 0);
    entity_editor *editor = &_G.editors[editor_idx];

    ct_ecs_a0->destroy(editor->world, &editor->entity, 1);
    editor->free = true;
}

static void open(uint64_t context_obj,
                 uint64_t obj) {
    entity_editor *editor = _new_editor(context_obj);
    editor->entity = ct_ecs_a0->spawn(editor->world, obj);
}

static void update(uint64_t context_obj,
                   float dt) {
    ct_controlers_i0 *keyboard;
    keyboard = ct_controlers_a0->get(CONTROLER_KEYBOARD);

    const ce_cdb_obj_o0 *reader = ce_cdb_a0->read(ce_cdb_a0->db(), context_obj);
    uint64_t editor_idx = ce_cdb_a0->read_uint64(reader, _EDITOR_IDX, 0);
    entity_editor *editor = &_G.editors[editor_idx];

    if (!editor->world.h) {
        return;
    }

    if (editor->mouse_hovering) {
    }

    ct_ecs_a0->simulate(editor->world, dt);

    ct_transform_comp *t = ct_ecs_a0->get_one(editor->world, TRANSFORM_COMPONENT,
                                              editor->camera_ent);
    ct_camera_component *c = ct_ecs_a0->get_one(editor->world, CT_CAMERA_COMPONENT,
                                                editor->camera_ent);

    ct_renderer_a0->viewport_render(editor->viewport,
                                    editor->world,
                                    (ct_camera_data_t0) {
                                            .world = t->world,
                                            .camera = *c,
                                    });
}

uint64_t cdb_type() {
    return ENTITY_RESOURCE_ID;
}

const char *display_icon() {
    return ICON_FA_CUBE;
}

const char *display_name() {
    return "Entity editor";
}


static struct ct_resource_editor_i0 ct_resource_editor_api = {
        .cdb_type = cdb_type,
        .open = open,
        .close = close,
        .update = update,
        .draw_ui = draw_editor,
        .draw_menu = draw_menu,
        .display_name = display_name,
        .display_icon = display_icon,
};

void CE_MODULE_LOAD(entity_editor)(struct ce_api_a0 *api,
                                   int reload) {
    CE_UNUSED(reload);
    CE_INIT_API(api, ce_memory_a0);
    CE_INIT_API(api, ce_id_a0);
    CE_INIT_API(api, ct_debugui_a0);
    CE_INIT_API(api, ct_ecs_a0);
    CE_INIT_API(api, ct_camera_a0);
    CE_INIT_API(api, ce_cdb_a0);
    CE_INIT_API(api, ct_rg_a0);
    CE_INIT_API(api, ct_default_rg_a0);


    _G = (struct _G) {
    };

    api->register_api(RESOURCE_EDITOR_I, &ct_resource_editor_api, sizeof(ct_resource_editor_api));
}

void CE_MODULE_UNLOAD(entity_editor)(struct ce_api_a0 *api,
                                     int reload) {

    CE_UNUSED(reload);
    CE_UNUSED(api);

    _G = (struct _G) {};
}
