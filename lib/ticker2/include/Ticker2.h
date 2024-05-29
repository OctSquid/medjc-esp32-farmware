#ifndef TICKER2_H
#define TICKER2_H

extern "C"
{
#include "esp_timer.h"
}
#include <functional>

class Ticker2
{
public:
    Ticker2();
    ~Ticker2();

    typedef void (*callback_with_arg_t)(void *);
    typedef std::function<void(void)> callback_function_t;

    void attach(float seconds, callback_function_t callback)
    {
        _callback_function = std::move(callback);
        _attach_us(1000000ULL * seconds, true, _static_callback, this);
    }

    void attach_ms(uint64_t milliseconds, callback_function_t callback)
    {
        _callback_function = std::move(callback);
        _attach_us(1000ULL * milliseconds, true, _static_callback, this);
    }

    void attach_us(uint64_t micros, callback_function_t callback)
    {
        _callback_function = std::move(callback);
        _attach_us(micros, true, _static_callback, this);
    }

    template <typename TArg>
    void attach(float seconds, void (*callback)(TArg), TArg arg)
    {
        static_assert(sizeof(TArg) <= sizeof(void *), "attach() callback argument size must be <= sizeof(void*)");
        // C-cast serves two purposes:
        // static_cast for smaller integer types,
        // reinterpret_cast + const_cast for pointer types
        _attach_us(1000000ULL * seconds, true, reinterpret_cast<callback_with_arg_t>(callback), reinterpret_cast<void *>(arg));
    }

    template <typename TArg>
    void attach_ms(uint64_t milliseconds, void (*callback)(TArg), TArg arg)
    {
        static_assert(sizeof(TArg) <= sizeof(void *), "attach() callback argument size must be <= sizeof(void*)");
        _attach_us(1000ULL * milliseconds, true, reinterpret_cast<callback_with_arg_t>(callback), reinterpret_cast<void *>(arg));
    }

    template <typename TArg>
    void attach_us(uint64_t micros, void (*callback)(TArg), TArg arg)
    {
        static_assert(sizeof(TArg) <= sizeof(void *), "attach() callback argument size must be <= sizeof(void*)");
        _attach_us(micros, true, reinterpret_cast<callback_with_arg_t>(callback), reinterpret_cast<void *>(arg));
    }

    void once(float seconds, callback_function_t callback)
    {
        _callback_function = std::move(callback);
        _attach_us(1000000ULL * seconds, false, _static_callback, this);
    }

    void once_ms(uint64_t milliseconds, callback_function_t callback)
    {
        _callback_function = std::move(callback);
        _attach_us(1000ULL * milliseconds, false, _static_callback, this);
    }

    void once_us(uint64_t micros, callback_function_t callback)
    {
        _callback_function = std::move(callback);
        _attach_us(micros, false, _static_callback, this);
    }

    template <typename TArg>
    void once(float seconds, void (*callback)(TArg), TArg arg)
    {
        static_assert(sizeof(TArg) <= sizeof(void *), "attach() callback argument size must be <= sizeof(void*)");
        _attach_us(1000000ULL * seconds, false, reinterpret_cast<callback_with_arg_t>(callback), reinterpret_cast<void *>(arg));
    }

    template <typename TArg>
    void once_ms(uint64_t milliseconds, void (*callback)(TArg), TArg arg)
    {
        static_assert(sizeof(TArg) <= sizeof(void *), "attach() callback argument size must be <= sizeof(void*)");
        _attach_us(1000ULL * milliseconds, false, reinterpret_cast<callback_with_arg_t>(callback), reinterpret_cast<void *>(arg));
    }

    template <typename TArg>
    void once_us(uint64_t micros, void (*callback)(TArg), TArg arg)
    {
        static_assert(sizeof(TArg) <= sizeof(void *), "attach() callback argument size must be <= sizeof(void*)");
        _attach_us(micros, false, reinterpret_cast<callback_with_arg_t>(callback), reinterpret_cast<void *>(arg));
    }

    void detach();
    bool active() const;

protected:
    static void _static_callback(void *arg);

    callback_function_t _callback_function = nullptr;

    esp_timer_handle_t _timer;

private:
    void _attach_us(uint64_t micros, bool repeat, callback_with_arg_t callback, void *arg);
};

#endif // TICKER_H
