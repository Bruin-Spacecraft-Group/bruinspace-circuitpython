# SPDX-FileCopyrightText: Copyright (c) 2026 Adafruit Industries LLC
#
# SPDX-License-Identifier: MIT

"""Set IDF_CONSTRAINT_FILE for CI.

CI installs requirements-dev.txt for all ports, but on Espressif builds we must
also apply the matching ESP-IDF constraints file so pip does not upgrade shared
packages (for example click) beyond what ESP-IDF allows. This script derives the
active ESP-IDF major.minor version from version.cmake and exports the exact
constraints file path into GITHUB_ENV for later install steps.
"""

import os
import pathlib
import re

TOP = pathlib.Path(__file__).resolve().parent.parent


def main() -> None:
    version_cmake = TOP / "ports" / "espressif" / "esp-idf" / "tools" / "cmake" / "version.cmake"
    data = version_cmake.read_text(encoding="utf-8")

    major = re.search(r"IDF_VERSION_MAJOR\s+(\d+)", data)
    minor = re.search(r"IDF_VERSION_MINOR\s+(\d+)", data)
    if major is None or minor is None:
        raise RuntimeError(f"Unable to parse IDF version from {version_cmake}")

    idf_tools_path = os.environ.get("IDF_TOOLS_PATH")
    if not idf_tools_path:
        raise RuntimeError("IDF_TOOLS_PATH is not set")

    constraint = (
        pathlib.Path(idf_tools_path) / f"espidf.constraints.v{major.group(1)}.{minor.group(1)}.txt"
    )

    github_env = os.environ.get("GITHUB_ENV")
    if github_env:
        with open(github_env, "a", encoding="utf-8") as f:
            f.write(f"IDF_CONSTRAINT_FILE={constraint}\n")

    print(f"Set IDF_CONSTRAINT_FILE={constraint}")


if __name__ == "__main__":
    main()
