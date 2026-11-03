# firmware-edge-AI

| Folder      | Purpose                                                  |
|-------------|----------------------------------------------------------|
| `data/`     | Datasets: `raw/`, `processed/`, `external/` (not in git) |
| `ml/`       | Notebooks, training code, exported models, tests         |
| `firmware/` | Embedded code and the converted `.tflite` model          |
| `hil/`      | Hardware-in-the-loop test scripts and configs            |
| `infra/`    | Docker, CI, and helper scripts                           |

## Setup (Windows, PowerShell)

The venv lives at a short path outside the repo because Windows long paths are
disabled on this machine and TensorFlow's install tree exceeds 260 characters.

```powershell
py -3.13 -m venv $HOME\.venvs\edge-ai
& $HOME\.venvs\edge-ai\Scripts\Activate.ps1
python -m pip install --upgrade pip
pip install -r requirements.txt          # or requirements.lock.txt for exact versions
python -m ipykernel install --user --name edge-ai --display-name "Python (edge-ai)"
python infra\scripts\check_env.py        # smoke test incl. TFLite conversion
jupyter lab
```

Pick the **Python (edge-ai)** kernel in Jupyter.

## Firmware (ESP32-S3-DevKitC-1 N16R8)

PlatformIO project in `firmware/`. Logic lives in `firmware/lib/` so it can be unit
tested on the host; `firmware/src/` is the thin hardware layer.

```powershell
pip install platformio
cd firmware
pio test -e native        # host unit tests, no board needed
pio check -e esp32-s3     # static analysis (cppcheck)
pio run -e esp32-s3       # build
pio run -e esp32-s3 -t upload; pio device monitor
```

## Deployment pipeline

`.github/workflows/firmware.yml` is the commit stage: host unit tests, static
analysis and a single build run on every PR that touches `firmware/`. The build
produces one versioned artifact (`firmware-esp32-s3-<sha>`: binaries, `SHA256SUMS`,
`build-info.json`, size report). Later stages (hardware-in-the-loop, staged OTA)
promote that exact artifact instead of rebuilding.
