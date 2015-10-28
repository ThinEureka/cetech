#include <cstdio>
#include <ctime>

#include "celib/defines.h"

#include "cetech/log_system/log_system.h"
#include "celib/memory/memory.h"
#include "celib/container/array.inl.h"

namespace cetech {
    namespace {
        using namespace log;


        struct LogData {
            Array < handler_t > handlers;
            Array < void* > handlers_data;

            LogData(Allocator & allocator) : handlers(allocator), handlers_data(allocator) {}

        };

        struct Globals {
            static const int MEMORY = sizeof(LogData);
            char buffer[MEMORY];

            LogData* data;

            Globals() : data(0) {}
        } _globals;

        void vlog(const LogLevel::Enum level, const char* where, const char* format, va_list va) {
            CE_CHECK_PTR( _globals.data );
            LogData& data = *_globals.data;

            char msg[4096];     //!< Final msg.
            vsnprintf(msg, 4096, format, va);

            time_t tm = std::time(NULL);
            const uint32_t handlers_count = array::size(data.handlers);
            for (uint32_t i = 0; i < handlers_count; ++i) {
                if (data.handlers[i] == nullptr) {
                    continue;
                }

                data.handlers[i](level, tm, where, msg, data.handlers_data[i]);
            }
        }
    }

    namespace log {
        void register_handler(handler_t handler, void* data) {
            CE_CHECK_PTR( _globals.data );
            LogData& log_data = *_globals.data;

            array::push_back(log_data.handlers, handler);
            array::push_back(log_data.handlers_data, data);
        }

        void unregister_handler(handler_t handler) {
            CE_CHECK_PTR( _globals.data );
            LogData& log_data = *_globals.data;


            for (uint32_t i = 0; i < array::size(log_data.handlers); ++i) {
                if (log_data.handlers[i] != handler) {
                    continue;
                }

                log_data.handlers[i] = nullptr;
            }
        }


        void info(const char* where, const char* format, va_list va) {
            vlog(LogLevel::LOG_INFO, where, format, va);
        }

        void info(const char* where, const char* format, ...) {
            va_list args;

            va_start(args, format);
            info(where, format, args);
            va_end(args);
        }

        void warning(const char* where, const char* format, va_list va) {
            vlog(LogLevel::LOG_WARNING, where, format, va);
        }

        void warning(const char* where, const char* format, ...) {
            va_list args;

            va_start(args, format);
            warning(where, format, args);
            va_end(args);
        }


        void error(const char* where, const char* format, va_list va) {
            vlog(LogLevel::LOG_ERROR, where, format, va);
        }

        void error(const char* where, const char* format, ...) {
            va_list args;

            va_start(args, format);
            error(where, format, args);
            va_end(args);
        }

        void debug(const char* where, const char* format, va_list va) {
        #ifdef DEBUG
            vlog(LogLevel::LOG_DEBUG, where, format, va);
        #else
            //CE_UNUSED_PARAM(where);
            //CE_UNUSED_PARAM(format);
        #endif
        }


        void debug(const char* where, const char* format, ...) {
        #ifdef DEBUG
            va_list args;
            va_start(args, format);
            debug(where, format, args);
            va_end(args);
        #else
            //CE_UNUSED_PARAM(where);
            //CE_UNUSED_PARAM(format);
        #endif
        }
    }

    namespace log_globals {
        void init() {
            char* p = _globals.buffer;

            _globals.data = new(p) LogData(memory_globals::default_allocator());
            
            log::info("filesystem_globals", "Initialized");
        }

        void shutdown() {
            log::info("filesystem_globals", "Shutdown");
            
            _globals.data->~LogData();
            _globals = Globals();

        }
    }
}