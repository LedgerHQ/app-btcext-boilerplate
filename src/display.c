#include "display.h"

#include "../bitcoin_app_base/src/ui/display.h"
#include "../bitcoin_app_base/src/ui/menu.h"
#include "io.h"
#include "nbgl_use_case.h"

static void review_choice(bool approved) {
    set_ux_flow_response(approved);  // sets the return value of io_ui_process

    if (approved) {
        // nothing to do in this case; after signing, the responsibility to show the main menu
        // goes back to the base app's handler
    } else {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_REJECTED, ui_menu_main);
    }
}

#define MAX_N_PAIRS 4;

bool display_transaction(dispatcher_context_t *dc,
                         int64_t value_spent,
                         uint64_t magic_input_value,
                         uint64_t fee) {
    nbgl_layoutTagValue_t pairs[4];
    nbgl_layoutTagValueList_t pairList;

    // format value_spent
    char value_str[32], magic_value_str[32], fee_str[32];
    uint64_t value_spent_abs = value_spent < 0 ? -value_spent : value_spent;
    format_sats_amount(COIN_COINID_SHORT, value_spent_abs, value_str);
    format_sats_amount(COIN_COINID_SHORT, magic_input_value, magic_value_str);
    format_sats_amount(COIN_COINID_SHORT, fee, fee_str);

    int n_pairs = 0;
    pairs[n_pairs++] = (nbgl_layoutTagValue_t){
        .item = "Transaction type",
        .value = "FOO",
    };

    if (value_spent >= 0) {
        pairs[n_pairs++] = (nbgl_layoutTagValue_t){
            .item = "Value spent",
            .value = value_str,
        };
    } else {
        pairs[n_pairs++] = (nbgl_layoutTagValue_t){
            .item = "Value received",
            .value = value_str,
        };
    }

    pairs[n_pairs++] = (nbgl_layoutTagValue_t){
        .item = "Magic value",
        .value = magic_value_str,
    };

    pairs[n_pairs++] = (nbgl_layoutTagValue_t){
        .item = "Fee",
        .value = fee_str,
    };

    assert(n_pairs <= MAX_N_PAIRS);

    // Setup list
    pairList.nbMaxLinesForValue = 0;
    pairList.nbPairs = n_pairs;
    pairList.pairs = pairs;

    nbgl_useCaseReview(TYPE_TRANSACTION,
                       &pairList,
                       &C_App_64px,
                       "Review transaction\nto a FOO output",
                       NULL,
                       "Sign transaction\nto create a FOO output?",
                       review_choice);

    // blocking call until the user approves or rejects the transaction
    bool result = io_ui_process(dc);
    if (!result) {
        SEND_SW(dc, SW_DENY);
        return false;
    }

    return true;
}