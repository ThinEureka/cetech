#include <stdio.h>

#include "engine/plugin/plugin_api.h"
//
//static struct log_api_v0 *log = 0;
//static struct memory_api_v0 *mem = 0;
//static struct lua_api_v0 *lua = 0;
//static Alloc_t alloc = 0;
//
//
//static int _foo(lua_State *l) {
//    log->warning("fooo", "dasdsadsadas");
//    return 0;
//}
//
//static int _foo2(lua_State *l) {
//    log->error("fooo", "dasdsadsadas");
//    return 0;
//}
//
static void _init_api(get_api_fce_t get_engine_api) {
//    log = get_engine_api(LOG_API_ID, 0);
//    mem = get_engine_api(MEMORY_API_ID, 0);
//    lua = get_engine_api(LUA_API_ID, 0);
}

static void _init(get_api_fce_t get_engine_api) {
    _init_api(get_engine_api);
//
//    alloc = mem->create_plugin_allocator("example");
//
//    lua->add_module_function("example", "foo", _foo);
//    lua->add_module_function("example", "foo2", _foo2);
//
//    lua->execute_string("engine.example.foo()");
}

static void _shutdown() {
    //mem->destroy_plugin_allocator(alloc);
}

static void *_reload_begin(get_api_fce_t get_engine_api) {
    //log->info("plugin_example", "Reload begin");

    return NULL;
}

static void _reload_end(get_api_fce_t get_engine_api, void *data) {
    _init_api(get_engine_api);
    _init(get_engine_api);
}

void *get_plugin_api(int api, int version) {
    if (api == PLUGIN_API_ID && version == 0) {
        static struct plugin_api_v0 plugin = {0};

        plugin.init = _init;
        plugin.shutdown = _shutdown;
        plugin.reload_begin = _reload_begin;
        plugin.reload_end = _reload_end;

        return &plugin;
    }
    return 0;
}
