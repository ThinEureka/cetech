#include <cetech/api_system.h>
#include <cetech/log.h>
#include <cetech/machine/machine.h>

#include <include/SDL2/SDL_cpuinfo.h>
#include <cetech/os/cpu.h>

int cpu_count() {
    return SDL_GetCPUCount();
}

static ct_cpu_a0 cpu_api = {
        .count = cpu_count
};

extern "C" void cpu_load_module(ct_api_a0 *api) {
    api->register_api("ct_cpu_a0", &cpu_api);
}

extern "C" void cpu_unload_module(ct_api_a0 *api) {
}
