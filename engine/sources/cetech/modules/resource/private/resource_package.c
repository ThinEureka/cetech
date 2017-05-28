//==============================================================================
// Includes
//==============================================================================

#include <stddef.h>

#include <cetech/core/memory/allocator.h>
#include <cetech/core/yaml.h>
#include <cetech/core/container/array.inl>

#include <cetech/core/hash.h>
#include <cetech/core/os/thread.h>
#include <cetech/core/memory/memory.h>
#include <cetech/core/module.h>
#include <cetech/core/os/vio.h>
#include <cetech/core/api.h>

#include <cetech/modules/task/task.h>
#include <cetech/modules/resource/resource.h>

#include "resource_package.h"

//==============================================================================
// Public interface
//==============================================================================

struct package_task_data {
    uint64_t name;
};

#define _G PackageGlobals
struct G {
    uint64_t package_typel;
} _G = {0};


IMPORT_API(memory_api_v0);
IMPORT_API(resource_api_v0);
IMPORT_API(task_api_v0);
IMPORT_API(thread_api_v0);
IMPORT_API(vio_api_v0);
IMPORT_API(hash_api_v0);

//==============================================================================
// Resource compiler
//==============================================================================

struct package_compile_data {
    ARRAY_T(uint64_t) types;
    ARRAY_T(uint64_t) name;
    ARRAY_T(uint32_t) name_count;
    ARRAY_T(uint32_t) offset;
};

void forach_clb(yaml_node_t key,
                yaml_node_t value,
                void *data) {
    struct package_compile_data *compile_data = data;

    char type_str[128] = {0};
    char name_str[128] = {0};

    yaml_as_string(key, type_str, CETECH_ARRAY_LEN(type_str));

    ARRAY_PUSH_BACK(uint64_t, &compile_data->types,
                    hash_api_v0.id64_from_str(type_str));
    ARRAY_PUSH_BACK(uint32_t, &compile_data->offset,
                    ARRAY_SIZE(&compile_data->name));

    const size_t name_count = yaml_node_size(value);
    ARRAY_PUSH_BACK(uint32_t, &compile_data->name_count, name_count);

    for (int i = 0; i < name_count; ++i) {
        yaml_node_t name_node = yaml_get_seq_node(value, i);
        yaml_as_string(name_node, name_str, CETECH_ARRAY_LEN(name_str));

        ARRAY_PUSH_BACK(uint64_t, &compile_data->name,
                        hash_api_v0.id64_from_str(name_str));

        yaml_node_free(name_node);
    }
}

int _package_compiler(const char *filename,
                      struct vio *source_vio,
                      struct vio *build_vio,
                      struct compilator_api *compilator_api) {

    char source_data[vio_api_v0.size(source_vio) + 1];
    memset(source_data, 0, vio_api_v0.size(source_vio) + 1);
    vio_api_v0.read(source_vio, source_data, sizeof(char),
                    vio_api_v0.size(source_vio));

    yaml_document_t h;
    yaml_node_t root = yaml_load_str(source_data, &h);

    struct package_compile_data compile_data = {0};
    ARRAY_INIT(uint64_t, &compile_data.types,
               memory_api_v0.main_allocator());
    ARRAY_INIT(uint64_t, &compile_data.name,
               memory_api_v0.main_allocator());
    ARRAY_INIT(uint32_t, &compile_data.offset, memory_api_v0.main_allocator());
    ARRAY_INIT(uint32_t, &compile_data.name_count,
               memory_api_v0.main_allocator());

    yaml_node_foreach_dict(root, forach_clb, &compile_data);

    struct package_resource resource = {0};
    resource.type_count = ARRAY_SIZE(&compile_data.types);
    resource.type_offset = sizeof(resource);
    resource.name_count_offset = resource.type_offset + (sizeof(uint64_t) *
                                                         ARRAY_SIZE(
                                                                 &compile_data.types));
    resource.name_offset = resource.name_count_offset +
                           (sizeof(uint32_t) *
                            ARRAY_SIZE(&compile_data.name_count));
    resource.offset_offset = resource.name_offset + (sizeof(uint64_t) *
                                                     ARRAY_SIZE(
                                                             &compile_data.name));

    vio_api_v0.write(build_vio, &resource, sizeof(resource), 1);
    vio_api_v0.write(build_vio, ARRAY_BEGIN(&compile_data.types),
                     sizeof(uint64_t), ARRAY_SIZE(&compile_data.types));
    vio_api_v0.write(build_vio, ARRAY_BEGIN(&compile_data.name_count),
                     sizeof(uint32_t),
                     ARRAY_SIZE(&compile_data.name_count));
    vio_api_v0.write(build_vio, ARRAY_BEGIN(&compile_data.name),
                     sizeof(uint64_t), ARRAY_SIZE(&compile_data.name));
    vio_api_v0.write(build_vio, ARRAY_BEGIN(&compile_data.offset),
                     sizeof(uint32_t),
                     ARRAY_SIZE(&compile_data.offset));

    ARRAY_DESTROY(uint64_t, &compile_data.types);
    ARRAY_DESTROY(uint64_t, &compile_data.name);
    ARRAY_DESTROY(uint32_t, &compile_data.offset);
    ARRAY_DESTROY(uint32_t, &compile_data.name_count);

    return 1;
}

int package_init(struct api_v0 *api) {
    GET_API(api, memory_api_v0);
    GET_API(api, resource_api_v0);
    GET_API(api, task_api_v0);
    GET_API(api, thread_api_v0);
    GET_API(api, vio_api_v0);
    GET_API(api, hash_api_v0);

    _G = (struct G) {0};

    _G.package_typel = hash_api_v0.id64_from_str("package");

#ifdef CETECH_CAN_COMPILE
    resource_api_v0.compiler_register(_G.package_typel, _package_compiler);
#endif

    return 1;
}

void package_shutdown() {

}


void package_task(void *data) {
    struct package_task_data *task_data = data;
    struct package_resource *package = resource_api_v0.get(_G.package_typel,
                                                           task_data->name);

    const uint32_t task_count = package->type_count;
    for (int j = 0; j < task_count; ++j) {
        resource_api_v0.load_now(package_type(package)[j],
                                 &package_name(package)[package_offset(
                                         package)[j]],
                                 package_name_count(package)[j]);
    }
}

void package_load(uint64_t name) {

    struct package_task_data *task_data =
    CETECH_ALLOCATE(memory_api_v0.main_allocator(), struct package_task_data,
                    1);

    task_data->name = name;

    struct task_item item = {
            .name = "package_task",
            .work = package_task,
            .data = task_data,
            .affinity = TASK_AFFINITY_NONE
    };

    task_api_v0.add(&item, 1);
}

void package_unload(uint64_t name) {
    struct package_resource *package = resource_api_v0.get(_G.package_typel,
                                                           name);

    const uint32_t task_count = package->type_count;
    for (int j = 0; j < task_count; ++j) {
        resource_api_v0.unload(package_type(package)[j],
                               &package_name(package)[package_offset(
                                       package)[j]],
                               package_name_count(package)[j]);
    }
}

int package_is_loaded(uint64_t name) {
    const uint64_t package_type = hash_api_v0.id64_from_str("package");
    struct package_resource *package = resource_api_v0.get(package_type, name);

    const uint32_t task_count = package->type_count;
    for (int i = 0; i < task_count; ++i) {
        if (!resource_api_v0.can_get_all(package_type(package)[i],
                                         &package_name(package)[package_offset(
                                                 package)[i]],
                                         package_name_count(package)[i])) {
            return 0;
        }
    }

    return 1;
}

void package_flush(uint64_t name) {
    while (!package_is_loaded(name)) {
        if (!task_api_v0.do_work()) {
            thread_api_v0.yield();
        }
    }
}