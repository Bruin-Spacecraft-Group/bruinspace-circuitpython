# BabbleSim testing

This document describes how to build and run CircuitPython tests against the
BabbleSim (bsim) nRF5340 board.

## Board target

We use the Zephyr BabbleSim board for the nRF5340 application core:

- Zephyr board: `nrf5340bsim/nrf5340/cpuapp`
- CircuitPython board alias: `native_nrf5340bsim`

The tests expect two bsim instances to run in the same simulation, which allows
future BLE/802.15.4 multi-node tests.

## Prerequisites

BabbleSim needs to be available to Zephyr. Either:

- Use the repo-provided `tools/bsim` checkout (if present)
- Or set environment variables:

```
export BSIM_COMPONENTS_PATH=/path/to/bsim/components
export BSIM_OUT_PATH=/path/to/bsim
```

## Build

```
CCACHE_TEMPDIR=/tmp/ccache-tmp make -j<n> BOARD=native_nrf5340bsim
```

If you do not use ccache, you can omit `CCACHE_TEMPDIR`.

## Run the bsim test

```
pytest tests/test_bsim_basics.py -v
```

## BLE scan + advertising tests

The BLE tests run multiple bsim instances and build Zephyr samples on-demand:

- `tests/test_bsim_ble_scan.py` scans for the Zephyr beacon sample
- `tests/test_bsim_ble_advertising.py` advertises from CircuitPython while the
  Zephyr observer sample scans

The fixtures build the Zephyr samples if missing:

- Beacon: `zephyr/samples/bluetooth/beacon` (board `nrf52_bsim`)
- Observer: `zephyr/samples/bluetooth/observer` (board `nrf52_bsim`)

Run the tests with:

```
pytest tests/test_bsim_ble_scan.py -v
pytest tests/test_bsim_ble_advertising.py -v
```

## Pytest markers

For bsim-specific test tuning:

- `@pytest.mark.duration(seconds)` controls simulation runtime/timeout.

Example:

```py
pytestmark = pytest.mark.duration(30.0)
```

## Notes

- The bsim test spawns two instances that share a sim id. It only checks UART
  output for now, but is the base for BLE/Thread multi-node tests.
- The BLE tests rely on the sysbuild HCI IPC net-core image for the nRF5340
  simulator (enabled via `sysbuild.conf`).
- The board uses a custom devicetree overlay to provide the SRAM region and
  CircuitPython flash partition expected by the port.
