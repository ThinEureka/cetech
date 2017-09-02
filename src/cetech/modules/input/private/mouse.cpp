//==============================================================================
// Includes
//==============================================================================

#include <cetech/modules/machine/machine.h>
#include <cetech/modules/input/input.h>
#include <cetech/kernel/application.h>
#include "celib/allocator.h"
#include "cetech/kernel/config.h"
#include "celib/eventstream.inl"

#include "cetech/kernel/api_system.h"

#include "mousebtnstr.h"
#include "cetech/kernel/log.h"
#include "cetech/kernel/errors.h"
#include "cetech/kernel/module.h"

CETECH_DECL_API(ct_machine_a0);
CETECH_DECL_API(ct_log_a0);
CETECH_DECL_API(ct_app_a0);


//==============================================================================
// Defines
//==============================================================================

#define LOG_WHERE "mouse"


//==============================================================================
// Globals
//==============================================================================


static struct G {
    uint8_t state[MOUSE_BTN_MAX];
    uint8_t last_state[MOUSE_BTN_MAX];
    float pos[2];
    float delta_pos[2];
    float wheel[2];
    float wheel_last[2];
} _G = {};

//==============================================================================
// Interface
//==============================================================================
namespace mouse {
    uint32_t button_index(const char *button_name) {
        for (uint32_t i = 0; i < MOUSE_BTN_MAX; ++i) {
            if (!_btn_to_str[i]) {
                continue;
            }

            if (strcmp(_btn_to_str[i], button_name)) {
                continue;
            }

            return i;
        }

        return 0;
    }

    const char *button_name(const uint32_t button_index) {
        CETECH_ASSERT(LOG_WHERE,
                      (button_index >= 0) && (button_index < MOUSE_BTN_MAX));

        return _btn_to_str[button_index];
    }

    int button_state(uint32_t idx,
                     const uint32_t button_index) {
        CEL_UNUSED(idx);
        CETECH_ASSERT(LOG_WHERE,
                      (button_index >= 0) && (button_index < MOUSE_BTN_MAX));

        return _G.state[button_index];
    }

    int button_pressed(uint32_t idx,
                       const uint32_t button_index) {
        CEL_UNUSED(idx);
        CETECH_ASSERT(LOG_WHERE,
                      (button_index >= 0) && (button_index < MOUSE_BTN_MAX));

        return _G.state[button_index] && !_G.last_state[button_index];
    }

    int button_released(uint32_t idx,
                        const uint32_t button_index) {
        CEL_UNUSED(idx);
        CETECH_ASSERT(LOG_WHERE,
                      (button_index >= 0) && (button_index < MOUSE_BTN_MAX));

        return !_G.state[button_index] && _G.last_state[button_index];
    }

    const char *axis_name(const uint32_t axis_index) {
        CETECH_ASSERT(LOG_WHERE,
                      (axis_index >= 0) && (axis_index < MOUSE_AXIS_MAX));

        return _axis_to_str[axis_index];
    }

    uint32_t axis_index(const char *axis_name) {
        for (uint32_t i = 0; i < MOUSE_AXIS_MAX; ++i) {
            if (!_axis_to_str[i]) {
                continue;
            }

            if (strcmp(_axis_to_str[i], axis_name) != 0) {
                continue;
            }

            return i;
        }

        return 0;
    }

    void axis(uint32_t idx,
              const uint32_t axis_index,
              float *value) {
        CEL_UNUSED(idx);
        CETECH_ASSERT(LOG_WHERE,
                      (axis_index >= 0) && (axis_index < MOUSE_AXIS_MAX));

        switch (axis_index) {
            case MOUSE_AXIS_ABSOULTE:
                value[0] = _G.pos[0];
                value[1] = _G.pos[1];
                return;

            case MOUSE_AXIS_RELATIVE:
                value[0] = _G.delta_pos[0];
                value[1] = _G.delta_pos[1];

                return;

            case MOUSE_AXIS_WHEEL:
                value[0] = _G.wheel[0];
                value[1] = _G.wheel[1];

                return;

            default:
                value[0] = 0.0f;
                value[1] = 0.0f;
                return;
        }
    }

//    void mouse_set_cursor_pos(vec2f_t pos) {
//        //TODO: implement
//    }

    void update(float dt) {
        CEL_UNUSED(dt);
        ct_event_header *event = ct_machine_a0.event_begin();

        memcpy(_G.last_state, _G.state, MOUSE_BTN_MAX);
        _G.delta_pos[0] = 0;
        _G.delta_pos[1] = 0;
        _G.wheel[0] = 0;
        _G.wheel[1] = 0;

        while (event != ct_machine_a0.event_end()) {
            ct_mouse_move_event *move_event;

            switch (event->type) {
                case EVENT_MOUSE_DOWN:
                    _G.state[((ct_mouse_event *) event)->button] = 1;
                    break;

                case EVENT_MOUSE_UP:
                    _G.state[((ct_mouse_event *) event)->button] = 0;
                    break;

                case EVENT_MOUSE_MOVE:
                    move_event = ((ct_mouse_move_event *) event);

                    _G.delta_pos[0] = move_event->pos[0] - _G.pos[0];
                    _G.delta_pos[1] = move_event->pos[1] - _G.pos[1];

                    _G.pos[0] = move_event->pos[0];
                    _G.pos[1] = move_event->pos[1];

                    break;

                case EVENT_MOUSE_WHEEL: {
                    ct_mouse_wheel_event* ev = ((ct_mouse_wheel_event *) event);
                    _G.wheel[0] = ev->pos[0];// - _G.wheel_last[0];
                    _G.wheel[1] = ev->pos[1];// - _G.wheel_last[1];

                    _G.wheel_last[0] = ev->pos[0];
                    _G.wheel_last[1] = ev->pos[1];
                }

                default:
                    break;
            }

            event = ct_machine_a0.event_next(event);
        }
    }
}

namespace mouse_module {
    static ct_mouse_a0 a0 = {
            .button_index = mouse::button_index,
            .button_name = mouse::button_name,
            .button_state = mouse::button_state,
            .button_pressed = mouse::button_pressed,
            .button_released = mouse::button_released,
            .axis_index = mouse::axis_index,
            .axis_name = mouse::axis_name,
            .axis = mouse::axis,
    };

    void _init_api(ct_api_a0 *api) {
        api->register_api("ct_mouse_a0", &a0);
    }

    void _init(ct_api_a0 *api) {
        _init_api(api);

        _G = {};

        ct_app_a0.register_on_update(mouse::update);

        ct_log_a0.debug(LOG_WHERE, "Init");
    }

    void _shutdown() {
        ct_log_a0.debug(LOG_WHERE, "Shutdown");

        _G = {};
    }

}

CETECH_MODULE_DEF(
        mouse,
        {
            CETECH_GET_API(api, ct_machine_a0);
            CETECH_GET_API(api, ct_log_a0);
            CETECH_GET_API(api, ct_app_a0);

        },
        {
            CEL_UNUSED(reload);
            mouse_module::_init(api);
        },
        {
            CEL_UNUSED(reload);
            CEL_UNUSED(api);

            mouse_module::_shutdown();

        }
)