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

## Firmware (ESP32-S3-DevKitC-1, N16R8)

The firmware is a [PlatformIO](https://platformio.org/) project in `firmware/`.
Install the **PlatformIO IDE** extension in VS Code, or the CLI with `pip install platformio`.

| Wiring (MPU-6050 / OLED / INA219) | ESP32-S3 |
|-----------------------------------|----------|
| VCC                               | 3V3      |
| GND                               | GND      |
| SDA                               | GPIO 8   |
| SCL                               | GPIO 9   |

Connect the board through the USB-C port labelled **UART** (or COM), then:

```bash
pio run -d firmware -t upload     # build and flash
pio device monitor -d firmware    # serial output, 115200 baud (Ctrl+C to quit)
```

The current firmware is a board check: it verifies 16 MB flash and 8 MB PSRAM,
scans the I2C bus, reads the MPU-6050's WHO_AM_I register and blinks the RGB LED
green (all checks passed) or red. CI builds the firmware on every pull request.
