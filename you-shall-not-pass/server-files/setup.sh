#!/usr/bin/env bash
sudo apt update && sudo apt upgrade -y
sudo apt install python3 python3-pip python3-venv
python3 -m venv venv
venv/bin/pip install -r requirements.txt
sudo mv ysnp.service /etc/systemd/system/

