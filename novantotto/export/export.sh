#!/bin/bash
if [ -z "$RESOURCES_DIR" ]; then
    echo "Error: RESOURCES_DIR environment variable is not set."
    exit 1
fi

# Get the directory where the script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

# Check if the bin directory exists
if [ ! -d "${SCRIPT_DIR}/bin" ]; then
    # Create the virtual environment
    python3 -m venv "${SCRIPT_DIR}"
    # Activate the virtual environment
    source "${SCRIPT_DIR}/bin/activate"
    # Install requirements from requirements.txt
    pip install -r "${SCRIPT_DIR}/requirements.txt"
else
    source "${SCRIPT_DIR}/bin/activate"
fi

python "${SCRIPT_DIR}/export.py" $*
