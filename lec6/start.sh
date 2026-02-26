#!/bin/env bash

VENV_DIR="./venv"

activate_venv() {
    if [ -f "$VENV_DIR/bin/activate" ]; then
		# Unix-like
        source "$VENV_DIR/bin/activate"
    elif [ -f "$VENV_DIR/Scripts/activate" ]; then
        # Windows
        source "$VENV_DIR/Scripts/activate"
    else
        echo "Cannot find venv activate script"
        exit 1
    fi
}

if [ -d $VENV_DIR ]; then
	activate_venv
else
	echo "venv doesn't exist, creating one..."
	python -m venv venv
	activate_venv
	pip install -r requirements.txt
fi

python generate.py
python -m http.server -d output/
