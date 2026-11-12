# SPDX-FileCopyrightText: 2025 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""Pytest fixtures for CircuitPython bsim testing."""

import logging
import os
import shutil
import subprocess
from pathlib import Path

import pytest

from .. import SerialSaver, StdSerial

logger = logging.getLogger(__name__)

ZEPHYR_CP = Path(__file__).resolve().parents[2]
BSIM_ROOT = ZEPHYR_CP / "tools/bsim"
BSIM_PHY_BINARY = BSIM_ROOT / "bin/bs_2G4_phy_v1"


@pytest.fixture
def native_sim_env() -> dict[str, str]:
    env = os.environ.copy()
    env["BSIM_OUT_PATH"] = str(BSIM_ROOT)
    env["BSIM_COMPONENTS_PATH"] = str(BSIM_ROOT / "components")
    lib_path = str(BSIM_ROOT / "lib")
    existing = env.get("LD_LIBRARY_PATH", "")
    env["LD_LIBRARY_PATH"] = f"{lib_path}:{existing}" if existing else lib_path
    return env


@pytest.fixture(params=["native_nrf5340bsim", "native_nrf54lm20bsim"])
def board(request):
    """Parametrized board fixture for bsim tests.

    Overrides the parent conftest's board fixture to run each test on both
    bsim boards.
    """
    return request.param


@pytest.fixture
def bsim_phy_binary():
    """Return path to BabbleSim PHY binary, skip if not present."""
    if not BSIM_PHY_BINARY.exists():
        pytest.skip(f"bs_2G4_phy_v1 not found: {BSIM_PHY_BINARY}")
    return BSIM_PHY_BINARY


class BsimPhyInstance:
    def __init__(self, proc: subprocess.Popen, serial: SerialSaver, timeout: float):
        self.proc = proc
        self.serial = serial
        self.timeout = timeout

    def finish_sim(self) -> None:
        self.serial.wait_for("Cleaning up", timeout=self.timeout + 5)

    def shutdown(self) -> None:
        if self.proc.poll() is None:
            self.proc.terminate()
            self.proc.wait(timeout=2)
        self.serial.close()


class ZephyrSampleProcess:
    def __init__(self, proc: subprocess.Popen, timeout: float):
        self._proc = proc
        self._timeout = timeout
        if proc.stdout is None:
            raise RuntimeError("Failed to capture Zephyr sample stdout")
        self.serial = SerialSaver(StdSerial(None, proc.stdout), name="zephyr sample")

    def shutdown(self) -> None:
        if self._proc.poll() is None:
            self._proc.terminate()
            self._proc.wait(timeout=self._timeout)
        self.serial.close()


@pytest.fixture
def bsim_phy(request, bsim_phy_binary, native_sim_env, sim_id):
    duration_marker = request.node.get_closest_marker("duration")
    duration = float(duration_marker.args[0]) if duration_marker else 20.0

    devices = 1
    if "circuitpython2" in request.fixturenames or "zephyr_sample" in request.fixturenames:
        devices = 2

    sample_marker = request.node.get_closest_marker("zephyr_sample")
    if sample_marker is not None:
        sample_device_id = int(sample_marker.kwargs.get("device_id", 1))
        devices = max(devices, sample_device_id + 1)

    # Do not pass -sim_length: if the PHY exits on simulated time, device 0 can
    # still be flushing UART output and test output can get truncated. Instead,
    # let pytest own process lifetime and terminate the PHY at fixture teardown.
    cmd = [
        "stdbuf",
        "-oL",
        str(bsim_phy_binary),
        "-v=9",  # Cleaning up level is on 9. Connecting is 7.
        f"-s={sim_id}",
        f"-D={devices}",
        "-argschannel",
        "-at=40",  # 40 dB attenuation (default 60) so RSSI ~ -40 dBm
    ]
    print("Running:", " ".join(cmd))
    proc = subprocess.Popen(
        cmd,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        env=native_sim_env,
        cwd=BSIM_ROOT / "bin",
    )
    if proc.stdout is None:
        raise RuntimeError("Failed to capture bsim phy stdout")

    # stdbuf -oL forces line-buffered stdout so SerialSaver can
    # stream-read PHY output in real time.  Wrapping in StdSerial
    # ensures the reader thread exits on EOF when the PHY process
    # terminates, rather than spinning on empty timeout reads.
    phy_output = SerialSaver(StdSerial(None, proc.stdout), name="bsim phy")
    try:
        phy_output.wait_for("Connecting", timeout=2)
    except TimeoutError:
        if proc.poll() is not None:
            print(phy_output.all_output)
            raise RuntimeError("bsim PHY exited immediately")
        # Assume bsim is running

    phy = BsimPhyInstance(proc, phy_output, timeout=duration)
    yield phy
    phy.shutdown()

    print("bsim phy output:")
    print(phy_output.all_output)


def _build_zephyr_sample(build_dir: Path, source_dir: Path, board: str) -> Path:
    if shutil.which("west") is None:
        raise RuntimeError("west not found")

    cmd = [
        "west",
        "build",
        "-b",
        board,
        "-d",
        str(build_dir),
        "-p=auto",
        str(source_dir),
    ]
    logger.info("Building Zephyr sample: %s", " ".join(cmd))
    subprocess.run(cmd, check=True, cwd=ZEPHYR_CP)

    return build_dir / "zephyr/zephyr.exe"


@pytest.fixture
def zephyr_sample(request, bsim_phy, native_sim_env, sim_id):
    marker = request.node.get_closest_marker("zephyr_sample")
    if marker is None or len(marker.args) != 1:
        raise RuntimeError(
            "zephyr_sample fixture requires @pytest.mark.zephyr_sample('<sample_path>')"
        )

    sample = marker.args[0]
    board = marker.kwargs.get("board", "nrf52_bsim")
    device_id = int(marker.kwargs.get("device_id", 1))
    timeout = float(marker.kwargs.get("timeout", 10.0))

    sample_rel = str(sample).removeprefix("zephyr/samples/")
    source_dir = ZEPHYR_CP / "zephyr/samples" / sample_rel
    if not source_dir.exists():
        source_dir = ZEPHYR_CP / sample  # port-local sample
    if not source_dir.exists():
        pytest.skip(f"Zephyr sample not found: {source_dir}")

    build_name = f"build-bsim-sample-{sample_rel.replace('/', '_')}-{board}"
    build_dir = ZEPHYR_CP / build_name
    binary = build_dir / "zephyr/zephyr.exe"

    needs_rebuild = False
    if not binary.exists():
        needs_rebuild = True
    else:
        # Rebuild if any source file is newer than the binary.
        binary_mtime = binary.stat().st_mtime
        for root, _dirs, files in os.walk(source_dir):
            for f in files:
                if Path(root, f).stat().st_mtime > binary_mtime:
                    needs_rebuild = True
                    break
            if needs_rebuild:
                break

    if needs_rebuild:
        try:
            binary = _build_zephyr_sample(build_dir, source_dir, board)
        except (subprocess.CalledProcessError, RuntimeError) as exc:
            pytest.skip(f"Failed to build Zephyr sample {sample_rel}: {exc}")

    if not binary.exists():
        pytest.skip(f"Zephyr sample binary not found: {binary}")

    cmd = [
        str(binary),
        f"-s={sim_id}",
        f"-d={device_id}",
        "-disconnect_on_exit=1",
    ]
    logger.info("Running: %s", " ".join(cmd))
    proc = subprocess.Popen(
        cmd,
        stdin=subprocess.DEVNULL,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        env=native_sim_env,
    )
    sample_proc = ZephyrSampleProcess(proc, timeout=timeout)
    yield sample_proc
    sample_proc.shutdown()

    print("Zephyr sample output:")
    print(sample_proc.serial.all_output)


FROZEN_ROOT = (ZEPHYR_CP / "../../frozen").resolve()


def get_library_files(name: str) -> dict[str, str]:
    """Return a dict mapping CIRCUITPY paths to file contents for all
    .py files in a frozen library, suitable for circuitpy_drive.

    Looks up ``name`` under the ``frozen/`` directory (e.g.
    ``get_library_files("adafruit_ble")`` walks
    ``frozen/Adafruit_CircuitPython_BLE/adafruit_ble/``).
    """
    files: dict[str, str] = {}

    # Try directory-style library: */<name>/**/*.py
    py_files = sorted(FROZEN_ROOT.glob(f"*/{name}/**/*.py"))
    if py_files:
        repo_dir = py_files[0].relative_to(FROZEN_ROOT).parts[0]
        lib_parent = FROZEN_ROOT / repo_dir
        for py_file in py_files:
            rel = py_file.relative_to(lib_parent)
            files[str(rel)] = py_file.read_text(encoding="utf-8")
        return files

    # Try single-file module: */<name>.py
    py_files = list(FROZEN_ROOT.glob(f"*/{name}.py"))
    if py_files:
        py_file = py_files[0]
        files[name + ".py"] = py_file.read_text(encoding="utf-8")
        return files

    raise FileNotFoundError(f"Library {name!r} not found under {FROZEN_ROOT}")


# pytest markers are defined inside out meaning the bottom one is first in the
# list and the top is last. So use negative indices to reverse them.
@pytest.fixture
def circuitpython1(circuitpython):
    return circuitpython[-1]


@pytest.fixture
def circuitpython2(circuitpython):
    return circuitpython[-2]
