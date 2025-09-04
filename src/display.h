#pragma once

#include <stdbool.h>

#include "../bitcoin_app_base/src/boilerplate/dispatcher.h"

bool display_transaction(dispatcher_context_t *dc,
                         int64_t value_spent,
                         uint64_t magic_input_value,
                         uint64_t fee);
