# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""Tests for web workflow on native_sim."""

from __future__ import annotations

import json
import re

import pytest
import requests


pytestmark = pytest.mark.native_sim_rt

WEB_WORKFLOW_PORT = 8090
WEB_WORKFLOW_PASSWORD = "testpass"

WEB_WORKFLOW_CODE = """\
import time

# Keep the VM alive while the web workflow starts.
time.sleep(3)
"""

WEB_WORKFLOW_UPDATED_CODE = """\
print("updated")
"""

WEB_WORKFLOW_SETTINGS = f"""\
CIRCUITPY_WEB_API_PASSWORD="{WEB_WORKFLOW_PASSWORD}"
CIRCUITPY_WEB_API_PORT={WEB_WORKFLOW_PORT}
"""

WEB_WORKFLOW_SETTINGS_PORT_80 = f"""\
CIRCUITPY_WEB_API_PASSWORD="{WEB_WORKFLOW_PASSWORD}"
CIRCUITPY_WEB_API_PORT=80
"""

WEB_WORKFLOW_BOOT = """\
import storage

storage.remount("/", readonly=False)
"""


@pytest.mark.circuitpy_drive(
    {
        "code.py": WEB_WORKFLOW_CODE,
        "settings.toml": WEB_WORKFLOW_SETTINGS,
    }
)
def test_web_workflow_hostnetwork(circuitpython):
    """Ensure web workflow responds over hostnetwork."""
    circuitpython.serial.wait_for(f"127.0.0.1:{WEB_WORKFLOW_PORT}")
    response = requests.get(f"http://127.0.0.1:{WEB_WORKFLOW_PORT}/edit/", timeout=1.0)

    assert response.status_code == 401


@pytest.mark.circuitpy_drive(
    {
        "code.py": WEB_WORKFLOW_CODE,
        "settings.toml": WEB_WORKFLOW_SETTINGS,
    }
)
def test_web_workflow_version_json_hostnetwork_ip_and_port(circuitpython):
    """Ensure /cp/version.json reports hostnetwork endpoint with configured port."""
    circuitpython.serial.wait_for(f"127.0.0.1:{WEB_WORKFLOW_PORT}")
    response = requests.get(
        f"http://127.0.0.1:{WEB_WORKFLOW_PORT}/cp/version.json",
        auth=("", WEB_WORKFLOW_PASSWORD),
        timeout=1.0,
    )

    assert response.status_code == 200

    payload = json.loads(response.text)
    assert payload["ip"] == "127.0.0.1"
    assert payload["port"] == WEB_WORKFLOW_PORT


@pytest.mark.circuitpy_drive(
    {
        "code.py": WEB_WORKFLOW_CODE,
        "settings.toml": WEB_WORKFLOW_SETTINGS,
    }
)
def test_web_workflow_status_line_hostnetwork_non_default_port(circuitpython):
    """Status line should include hostnetwork IP and non-default port."""
    circuitpython.wait_until_done()
    output = circuitpython.serial.all_output

    # Remove ANSI control sequences before matching.
    output = re.sub(r"\x1b\[[0-9;]*[A-Za-z]", "", output)
    assert "127.0.0.1:8090" in output


@pytest.mark.circuitpy_drive(
    {
        "code.py": WEB_WORKFLOW_CODE,
        "settings.toml": WEB_WORKFLOW_SETTINGS_PORT_80,
    }
)
def test_web_workflow_status_line_hostnetwork_default_port(circuitpython):
    """Status line should show IP without :80 for default HTTP port."""
    circuitpython.wait_until_done()
    output = circuitpython.serial.all_output

    output = re.sub(r"\x1b\[[0-9;]*[A-Za-z]", "", output)
    assert "127.0.0.1" in output
    assert "127.0.0.1:80" not in output


@pytest.mark.circuitpy_drive(
    {
        "boot.py": WEB_WORKFLOW_BOOT,
        "code.py": WEB_WORKFLOW_CODE,
        "settings.toml": WEB_WORKFLOW_SETTINGS,
    }
)
def test_web_workflow_write_code_py_remount(circuitpython):
    """Ensure web workflow can update code.py after remounting."""
    circuitpython.serial.wait_for(f"127.0.0.1:{WEB_WORKFLOW_PORT}")
    body = WEB_WORKFLOW_UPDATED_CODE.encode("utf-8")

    response = requests.put(
        f"http://127.0.0.1:{WEB_WORKFLOW_PORT}/fs/code.py",
        auth=("", WEB_WORKFLOW_PASSWORD),
        data=body,
        timeout=1.0,
    )
    assert response.status_code in (201, 204)

    response = requests.get(
        f"http://127.0.0.1:{WEB_WORKFLOW_PORT}/fs/code.py",
        auth=("", WEB_WORKFLOW_PASSWORD),
        timeout=1.0,
    )
    assert response.status_code == 200
    assert WEB_WORKFLOW_UPDATED_CODE in response.text
