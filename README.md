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
