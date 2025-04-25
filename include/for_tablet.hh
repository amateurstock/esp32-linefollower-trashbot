#ifndef FOR_TABLET_HH
#define FOR_TABLET_HH
#include "esp_err.h"

typedef int esp_err_t;
typedef void* TaskHandle_t;
typedef uint32_t TickType_t;
void vTaskDelay( const TickType_t xTicksToDelay );

#define ESP_ERROR_CHECK(x) do {                                         \
        esp_err_t err_rc_ = (x);                                        \
        if (unlikely(err_rc_ != ESP_OK)) {                              \
            _esp_error_check_failed(err_rc_, __FILE__, __LINE__,        \
                                    __ASSERT_FUNC, #x);                 \
        }                                                               \
    } while(0)

#endif
