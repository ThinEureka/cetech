//==============================================================================
// Includes
//==============================================================================

#include "include/SDL2/SDL.h"
#include "include/mpack/mpack.h"

#include <cetech/core/container/map.inl>

#include <cetech/core/hash.h>
#include <cetech/core/application.h>
#include <cetech/core/config.h>
#include <cetech/core/memory/memory.h>
#include <cetech/core/module.h>
#include <cetech/core/os/path.h>
#include <cetech/core/os/vio.h>
#include <cetech/core/api.h>

#include <cetech/modules/resource/resource.h>
#include <cetech/modules/develop_system/develop.h>
#include <cetech/modules/filesystem/filesystem.h>


#include "resource.h"

//==============================================================================
// Struct and types
//==============================================================================

typedef struct {
    void *data;
    uint8_t ref_count;
} resource_item_t;

ARRAY_PROTOTYPE(resource_item_t)

ARRAY_PROTOTYPE_N(const char*, cstring)

MAP_PROTOTYPE(resource_item_t)

ARRAY_PROTOTYPE_N(MAP_T(resource_item_t), resource_data)

ARRAY_PROTOTYPE(resource_callbacks_t)


//==============================================================================
// Gloals
//==============================================================================

#define LOG_WHERE "resource_manager"

//==============================================================================
// Gloals
//==============================================================================

static const resource_item_t null_item = {.data=NULL, .ref_count=0};

#define is_item_null(item) (item.data == null_item.data)

#define _G ResourceManagerGlobals
struct G {
    MAP_T(uint32_t) type_map;
    ARRAY_T(resource_data) resource_data;
    ARRAY_T(resource_callbacks_t) resource_callbacks;
    int autoload_enabled;

    struct {
        cvar_t build_dir;
    } config;

} _G = {0};


IMPORT_API(memory_api_v0);
IMPORT_API(cnsole_srv_api_v0);
IMPORT_API(filesystem_api_v0);
IMPORT_API(config_api_v0);
IMPORT_API(app_api_v0);
IMPORT_API(path_v0);
IMPORT_API(vio_api_v0);
IMPORT_API(log_api_v0);
IMPORT_API(hash_api_v0);


int resource_compiler_get_build_dir(char *build_dir,
                                    size_t max_len,
                                    const char *platform) {
    const char *build_dir_str = config_api_v0.get_string(_G.config.build_dir);
    return path_v0.path_join(build_dir, max_len, build_dir_str, platform);
}

//==============================================================================
// Private
//==============================================================================

void resource_reload_all();

static int _cmd_reload_all(mpack_node_t args,
                           mpack_writer_t *writer) {
    resource_reload_all();
    return 0;
}

static MAP_T(resource_item_t) *_get_resource_map(uint64_t type) {
    const uint32_t idx = MAP_GET(uint32_t, &_G.type_map, type, UINT32_MAX);

    if (idx == UINT32_MAX) {
        return NULL;
    }

    return &ARRAY_AT(&_G.resource_data, idx);
}

void *package_resource_loader(struct vio *input,
                              struct allocator *allocator) {
    const int64_t size = vio_api_v0.size(input);
    char *data = CETECH_ALLOCATE(allocator, char, size);
    vio_api_v0.read(input, data, 1, size);

    return data;
}

void package_resource_unloader(void *new_data,
                               struct allocator *allocator) {
    CETECH_DEALLOCATE(allocator, new_data);
}

void package_resource_online(uint64_t name,
                             void *data) {
}

void package_resource_offline(uint64_t name,
                              void *data) {
}

void *package_resource_reloader(uint64_t name,
                                void *old_data,
                                void *new_data,
                                struct allocator *allocator) {
    CETECH_DEALLOCATE(allocator, old_data);
    return new_data;
}

static const resource_callbacks_t package_resource_callback = {
        .loader = package_resource_loader,
        .unloader =package_resource_unloader,
        .online =package_resource_online,
        .offline =package_resource_offline,
        .reloader = package_resource_reloader
};

extern int package_init(struct api_v0 *api);

extern void package_shutdown();

void resource_register_type(uint64_t type,
                            resource_callbacks_t callbacks);

static void _init_api(struct api_v0 *api) {

    static struct resource_api_v0 resource_api = {0};

    resource_api.set_autoload = resource_set_autoload;
    resource_api.register_type = resource_register_type;
    resource_api.load = resource_load;
    resource_api.add_loaded = resource_add_loaded;
    resource_api.load_now = resource_load_now;
    resource_api.unload = resource_unload;
    resource_api.reload = resource_reload;
    resource_api.reload_all = resource_reload_all;
    resource_api.can_get = resource_can_get;
    resource_api.can_get_all = resource_can_get_all;
    resource_api.get = resource_get;
    resource_api.type_name_string = resource_type_name_string;
    resource_api.compiler_get_build_dir = resource_compiler_get_build_dir;

#ifdef CETECH_CAN_COMPILE
    resource_api.compiler_get_core_dir = resource_compiler_get_core_dir;
    resource_api.compiler_register = resource_compiler_register;
    resource_api.compiler_compile_all = resource_compiler_compile_all;
    resource_api.compiler_get_filename = resource_compiler_get_filename;
    resource_api.compiler_get_tmp_dir = resource_compiler_get_tmp_dir;
    resource_api.compiler_external_join = resource_compiler_external_join;
    resource_api.compiler_create_build_dir = resource_compiler_create_build_dir;
    resource_api.compiler_get_source_dir = resource_compiler_get_source_dir;
#endif

    api->register_api("resource_api_v0", &resource_api);

    static struct package_api_v0 package_api = {0};

    package_api.load = package_load;
    package_api.unload = package_unload;
    package_api.is_loaded = package_is_loaded;
    package_api.flush = package_flush;

    api->register_api("package_api_v0", &package_api);
}


static void _init(struct api_v0 *api) {
    GET_API(api, cnsole_srv_api_v0);
    GET_API(api, memory_api_v0);
    GET_API(api, filesystem_api_v0);
    GET_API(api, config_api_v0);
    GET_API(api, app_api_v0);
    GET_API(api, path_v0);
    GET_API(api, vio_api_v0);
    GET_API(api, log_api_v0);
    GET_API(api, hash_api_v0);

    ARRAY_INIT(resource_data, &_G.resource_data,
               memory_api_v0.main_allocator());
    ARRAY_INIT(resource_callbacks_t, &_G.resource_callbacks,
               memory_api_v0.main_allocator());
    MAP_INIT(uint32_t, &_G.type_map, memory_api_v0.main_allocator());

//    _G.config.build_dir = config_api_v0.find("build");


    char build_dir_full[4096] = {0};
    path_v0.path_join(build_dir_full,
                      CETECH_ARRAY_LEN(build_dir_full),
                      config_api_v0.get_string(_G.config.build_dir),
                      app_api_v0.platform());

    filesystem_api_v0.filesystem_map_root_dir(
            hash_api_v0.id64_from_str("build"),
            build_dir_full);

    resource_register_type(hash_api_v0.id64_from_str("package"),
                           package_resource_callback);

    cnsole_srv_api_v0.consolesrv_register_command("resource.reload_all",
                                                  _cmd_reload_all);

    package_init(api);
    //return package_init();

}

static void _init_cvar(struct config_api_v0 config) {
    _G = (struct G) {0};

    config_api_v0 = config;

    _G.config.build_dir = config.new_str("build", "Resource build dir",
                                         "data/build");
}

static void _shutdown() {
    for (int i = 0; i < ARRAY_SIZE(&_G.resource_data); ++i) {
        MAP_DESTROY(resource_item_t, &ARRAY_AT(&_G.resource_data, i));
    }

    ARRAY_DESTROY(resource_data, &_G.resource_data);
    ARRAY_DESTROY(resource_callbacks_t, &_G.resource_callbacks);
    MAP_DESTROY(uint32_t, &_G.type_map);

    package_shutdown();
}



//==============================================================================
// Public interface
//==============================================================================

int resource_type_name_string(char *str,
                              size_t max_len,
                              uint64_t type,
                              uint64_t name) {
    return snprintf(str, max_len, "%" SDL_PRIX64 "%" SDL_PRIX64, type,
                    name);
}


void resource_set_autoload(int enable) {
    _G.autoload_enabled = enable;
}

void resource_register_type(uint64_t type,
                            resource_callbacks_t callbacks) {

    const uint32_t idx = ARRAY_SIZE(&_G.resource_data);

    ARRAY_PUSH_BACK(resource_data, &_G.resource_data,
                    (MAP_T(resource_item_t)) {0});
    ARRAY_PUSH_BACK(resource_callbacks_t, &_G.resource_callbacks, callbacks);

    MAP_INIT(resource_item_t, &ARRAY_AT(&_G.resource_data, idx),
             memory_api_v0.main_allocator());

    MAP_SET(uint32_t, &_G.type_map, type, idx);
}

void resource_add_loaded(uint64_t type,
                         uint64_t *names,
                         void **resource_data,
                         size_t count) {
    const uint32_t idx = MAP_GET(uint32_t, &_G.type_map, type, UINT32_MAX);

    if (idx == UINT32_MAX) {
        return;
    }

    MAP_T(resource_item_t) *resource_map = &ARRAY_AT(&_G.resource_data, idx);

    resource_item_t item = {.ref_count=1};
    for (size_t i = 0; i < count; i++) {
        item.data = resource_data[i];
        MAP_SET(resource_item_t, resource_map, names[i], item);

        if (resource_data[i] == 0) {
            continue;
        }
        ARRAY_AT(&_G.resource_callbacks, idx).online(names[i],
                                                     resource_data[i]);
    }
}

void resource_load(void **loaded_data,
                   uint64_t type,
                   uint64_t *names,
                   size_t count,
                   int force);

void resource_load_now(uint64_t type,
                       uint64_t *names,
                       size_t count) {
    void *loaded_data[count];

    resource_load(loaded_data, type, names, count, 0);
    resource_add_loaded(type, names, loaded_data, count);
}

int resource_can_get(uint64_t type,
                     uint64_t names) {
    MAP_T(resource_item_t) *resource_map = _get_resource_map(type);

    if (resource_map == NULL) {
        return 1;
    }

    return MAP_HAS(resource_item_t, resource_map, names);
}

int resource_can_get_all(uint64_t type,
                         uint64_t *names,
                         size_t count) {
    MAP_T(resource_item_t) *resource_map = _get_resource_map(type);

    if (resource_map == NULL) {
        return 1;
    }

    for (size_t i = 0; i < count; ++i) {
        if (!MAP_HAS(resource_item_t, resource_map, names[i])) {
            return 0;
        }
    }

    return 1;
}

void resource_load(void **loaded_data,
                   uint64_t type,
                   uint64_t *names,
                   size_t count,
                   int force) {
    const uint32_t idx = MAP_GET(uint32_t, &_G.type_map, type, UINT32_MAX);

    if (idx == UINT32_MAX) {
        log_api_v0.log_error(LOG_WHERE,
                             "Loader for resource is not is not registred");
        memset(loaded_data, sizeof(void *), count);
        return;
    }

    const uint64_t root_name = hash_api_v0.id64_from_str("build");

    MAP_T(resource_item_t) *resource_map = &ARRAY_AT(&_G.resource_data, idx);

    resource_callbacks_t type_clb = ARRAY_AT(&_G.resource_callbacks, idx);


    for (int i = 0; i < count; ++i) {
        resource_item_t item = MAP_GET(resource_item_t, resource_map,
                                       names[i], null_item);

        if (!force && (item.ref_count > 0)) {
            ++item.ref_count;
            MAP_SET(resource_item_t, resource_map, names[i], item);
            loaded_data[i] = 0;
            continue;
        }

        char build_name[33] = {0};
        resource_type_name_string(build_name, CETECH_ARRAY_LEN(build_name),
                                  type,
                                  names[i]);

#ifdef CETECH_CAN_COMPILE
        char filename[4096] = {0};
        resource_compiler_get_filename(filename, CETECH_ARRAY_LEN(filename),
                                       type,
                                       names[i]);
#else
        char *filename = build_name;
#endif
        log_api_v0.log_debug("resource", "Loading resource %s from %s/%s",
                             filename,
                             filesystem_api_v0.filesystem_get_root_dir(
                                     root_name),
                             build_name);

        struct vio *resource_file = filesystem_api_v0.filesystem_open(root_name,
                                                                      build_name,
                                                                      VIO_OPEN_READ);

        if (resource_file != NULL) {
            loaded_data[i] = type_clb.loader(resource_file,
                                             memory_api_v0.main_allocator());
            filesystem_api_v0.filesystem_close(resource_file);
        } else {
            loaded_data[i] = 0;
        }
    }
}

void resource_unload(uint64_t type,
                     uint64_t *names,
                     size_t count) {
    const uint32_t idx = MAP_GET(uint32_t, &_G.type_map, type, UINT32_MAX);

    if (idx == UINT32_MAX) {
        return;
    }

    MAP_T(resource_item_t) *resource_map = _get_resource_map(type);

    resource_callbacks_t type_clb = ARRAY_AT(&_G.resource_callbacks, idx);

    for (int i = 0; i < count; ++i) {
        resource_item_t item = MAP_GET(resource_item_t, resource_map,
                                       names[i], null_item);

        if (item.ref_count == 0) {
            continue;
        }

        if (--item.ref_count == 0) {
            char build_name[33] = {0};
            resource_type_name_string(build_name, CETECH_ARRAY_LEN(build_name),
                                      type, names[i]);

#ifdef CETECH_CAN_COMPILE
            char filename[4096] = {0};
            resource_compiler_get_filename(filename, CETECH_ARRAY_LEN(filename),
                                           type,
                                           names[i]);
#else
            char *filename = build_name;
#endif

            log_api_v0.log_debug("resource", "Unload resource %s ", filename);

            type_clb.offline(names[i], item.data);
            type_clb.unloader(item.data, memory_api_v0.main_allocator());

            MAP_REMOVE(resource_item_t, resource_map, names[i]);
        }

        MAP_SET(resource_item_t, resource_map, names[i], item);
    }
}

void *resource_get(uint64_t type,
                   uint64_t names) {
    MAP_T(resource_item_t) *resource_map = _get_resource_map(type);

    resource_item_t item = MAP_GET(resource_item_t, resource_map, names,
                                   null_item);
    if (is_item_null(item)) {
        char build_name[33] = {0};
        resource_type_name_string(build_name, CETECH_ARRAY_LEN(build_name),
                                  type,
                                  names);

        if (_G.autoload_enabled) {
#ifdef CETECH_CAN_COMPILE
            char filename[4096] = {0};
            resource_compiler_get_filename(filename, CETECH_ARRAY_LEN(filename),
                                           type,
                                           names);
#else
            char *filename = build_name;
#endif
            log_api_v0.log_warning(LOG_WHERE, "Autoloading resource %s",
                                   filename);
            resource_load_now(type, &names, 1);
            item = MAP_GET(resource_item_t, resource_map, names, null_item);
        } else {
            // TODO: fallback resource #205
            CETECH_ASSERT(LOG_WHERE, false);
        }
    }

    return item.data;
}

void resource_reload(uint64_t type,
                     uint64_t *names,
                     size_t count) {
    module_reload_all();

    void *loaded_data[count];
    MAP_T(resource_item_t) *resource_map = _get_resource_map(type);
    const uint32_t idx = MAP_GET(uint32_t, &_G.type_map, type, 0);

    resource_callbacks_t type_clb = ARRAY_AT(&_G.resource_callbacks, idx);

    resource_load(loaded_data, type, names, count, 1);
    for (int i = 0; i < count; ++i) {
#ifdef CETECH_CAN_COMPILE
        char filename[4096] = {0};
        resource_compiler_get_filename(filename, CETECH_ARRAY_LEN(filename),
                                       type,
                                       names[i]);
#else
        char build_name[33] = {0};
        resource_type_name_string(build_name, CETECH_ARRAY_LEN(build_name),
                                  type, names[i]);

        char *filename = build_name;
#endif
        log_api_v0.log_debug("resource", "Reload resource %s ", filename);

        void *old_data = resource_get(type, names[i]);

        void *new_data = type_clb.reloader(names[i], old_data, loaded_data[i],
                                           memory_api_v0.main_allocator());

        resource_item_t item = MAP_GET(resource_item_t, resource_map,
                                       names[i], null_item);
        item.data = new_data;
        //--item.ref_count; // Load call increase item.ref_count, because is loaded
        MAP_SET(resource_item_t, resource_map, names[i], item);
    }
}

void resource_reload_all() {
    const MAP_ENTRY_T(uint32_t) *type_it = MAP_BEGIN(uint32_t, &_G.type_map);
    const MAP_ENTRY_T(uint32_t) *type_end = MAP_END(uint32_t, &_G.type_map);

    ARRAY_T(uint64_t) name_array = {0};
    ARRAY_INIT(uint64_t, &name_array, memory_api_v0.main_allocator());

    while (type_it != type_end) {
        uint64_t type_id = type_it->key;

        MAP_T(resource_item_t) *resource_map = _get_resource_map(type_id);

        const MAP_ENTRY_T(resource_item_t) *name_it = MAP_BEGIN(resource_item_t,
                                                                resource_map);
        const MAP_ENTRY_T(resource_item_t) *name_end = MAP_END(resource_item_t,
                                                               resource_map);

        ARRAY_RESIZE(uint64_t, &name_array, 0);
        while (name_it != name_end) {
            uint64_t name_id = name_it->key;

            ARRAY_PUSH_BACK(uint64_t, &name_array, name_id);

            ++name_it;
        }

        resource_reload(type_id, &ARRAY_AT(&name_array, 0),
                        ARRAY_SIZE(&name_array));

        ++type_it;
    }

    ARRAY_DESTROY(uint64_t, &name_array);
}

void *resourcesystem_get_module_api(int api) {
    switch (api) {
        case PLUGIN_EXPORT_API_ID: {
            static struct module_api_v0 module = {0};

            module.init = _init;
            module.init_api = _init_api;
            module.shutdown = _shutdown;
            module.init_cvar = _init_cvar;

            return &module;
        }


        default:
            return NULL;
    }

}