#!/usr/bin/env bash
while true; do
	python3 bridge.py ./Logo/Logo0.png /dev/serial/by-id/usb*
	sleep 1
	python3 bridge.py ./Logo/Logo1.png /dev/serial/by-id/usb*
	sleep 1
	python3 bridge.py ./Logo/Logo2.png /dev/serial/by-id/usb*
	sleep 1
	python3 bridge.py ./Logo/Logo3.png /dev/serial/by-id/usb*
	sleep 1
	python3 bridge.py ./Logo/Logo4.png /dev/serial/by-id/usb*
	sleep 1
	python3 bridge.py ./Logo/Logo5.png /dev/serial/by-id/usb*
	sleep 1
done
