# How to test without actual device

1. Run `poetry install`
1. Set the server address in main.py to `127.0.0.1`
1. Run `poetry run inventory`
1. Open another terminal and run `nc 127.0.0.1 5555`
1. Type `dispensed`<return> into the `nc` prompt
1. To "dispense" again, run `nc` again

# How to test with device
1. Run `poetry install`
1. Connect laptop to phone hotspot
1. Run `ifconfig | grep 172` to find laptop IP
1. Set the server address in main.py to laptop IP
1. Run `poetry run inventory`
1. Go to [../devices/pill-dispenser/README.md](pill dispenser readme) to learn how to upload code to pill dispenser device
