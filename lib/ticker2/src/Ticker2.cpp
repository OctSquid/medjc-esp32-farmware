#include <Ticker2.h>

Ticker2::Ticker2() : _timer(nullptr) {}

Ticker2::~Ticker2()
{
    detach();
}

void Ticker2::_attach_us(uint64_t micros, bool repeat, callback_with_arg_t callback, void *arg)
{
    esp_timer_create_args_t _timerConfig;
    _timerConfig.arg = reinterpret_cast<void *>(arg);
    _timerConfig.callback = callback;
    _timerConfig.dispatch_method = ESP_TIMER_TASK;
    _timerConfig.name = "Ticker";
    if (_timer)
    {
        esp_timer_stop(_timer);
        esp_timer_delete(_timer);
    }
    esp_timer_create(&_timerConfig, &_timer);
    if (repeat)
    {
        esp_timer_start_periodic(_timer, micros);
    }
    else
    {
        esp_timer_start_once(_timer, micros);
    }
}

void Ticker2::detach()
{
    if (_timer)
    {
        esp_timer_stop(_timer);
        esp_timer_delete(_timer);
        _timer = nullptr;
        _callback_function = nullptr;
    }
}

bool Ticker2::active() const
{
    if (!_timer)
    {
        return false;
    }
    return esp_timer_is_active(_timer);
}

void Ticker2::_static_callback(void *arg)
{
    Ticker2 *_this = reinterpret_cast<Ticker2 *>(arg);
    if (_this && _this->_callback_function)
    {
        _this->_callback_function();
    }
}
