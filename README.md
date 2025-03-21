# Ledger Boilerplate for Bitcoin Smart Contract Applications

This is a boilerplate application which can be forked to start a new project for Ledger devices that can sign specialized types of bitcoin transactions, while building on top of the tooling of the Ledger Bitcoin application.

## The Foo protocol

This repository implements an imaginary protocol for clear signing transactions of a specific format that the bitcoin app does not support.

The Foo protocol expects transactions that are just like normal transactions (inputs and change outputs from a standard account compliant to BIP-44, BIP-49, BIP-84 or BIP-86), plus in addition:
- a special 'magic input' that is a P2TR UTXO with taproot public key at a fixed derivation path `m/86'/1'/99'`.
- an `OP_RETURN` output with the message `FOO`.

The app's UX validates that the transaction satisfies this protocol, and shows all the transaction details with a clear UX.

This example app also implements a custom APDU that receives a data buffer, and returns the binary XOR of all its bytes. 

## Hooks

Your apps can hook into the several places in order to extend upon the functionality of the base app.

The [main.c](./src/main.c) contains an example and code documentation for each of them.

### <code>validate_and_display_transaction</code>

This function must be implemented by your application in order to make sure that the transaction is valid.

The function has access to the entire content of the PSBT (via the functionality provided by the base app), and two bitvectors (one for the inputs, and one for the outputs) indicating which inputs/outputs are considered *internal*. Internal inputs are the ones that are spending a UTXO controlled by the wallet policy; similarly, internal outputs are valid change addresses. All the other inputs/outputs are external.

This function must validate all the external inputs, and it MUST reject if any unexpected input is present.

### <code>sign_custom_inputs</code>

This function can be implemented to allow your application to sign for external inputs, are all the inputs that do not belong to the wallet policy (and are therefore custom to the protocol of your app - as any unrecognized input would have been rejected by <code>validate_and_display_transaction</code>).

The function can use the functionality implemented in the base app in order to comput the sighash and yield the signatures returned to the client:
- SegWitV1 (taproot) inputs: `compute_sighash_segwitv1` and `sign_sighash_schnorr_and_yield`;
- SegWitV0 inputs: `compute_sighash_segwitv0` and `sign_sighash_ecdsa_and_yield`;
- Legacy inputs: you should probably not use custom legacy inputs.

Please consult the code of the base app for exact documentation about those functions. Definitions are in the headers [sign_psbt.h](https://github.com/LedgerHQ/app-bitcoin-new/blob/baseapp/src/handler/sign_psbt.h) and [txhashes.h](https://github.com/LedgerHQ/app-bitcoin-new/blob/baseapp/src/handler/sign_psbt/txhashes.h).

If there are no external inputs to sign for, then this function can be omitted.

### <code>custom_apdu_handler</code>

This function can be implemented in order to customize the processing of APDUs, and add new ones.

It is recommended that your app uses the same `CLA` value of `0xE1` used by the base app, and an `INS` equal to 128 or above.

## Compiling the app

Initialize the submodule with

```
$ git submodule update --init --recursive
```

Compile the app [as usual](https://github.com/LedgerHQ/app-boilerplate#quick-start-guide).
You should be able to launch it using speculos.

## Running the tests

Create a Python virtual environment and install the requirements:

```
$ python -m venv venv
$ source venv/bin/activate
$ pip install -r tests/requirements.txt
```

Launch the test suite; for example, if you compiled the app for Ledger Flex:

```
$ pytest --device=flex
```
