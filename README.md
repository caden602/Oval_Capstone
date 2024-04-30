# Oval_Capstone

**<span style="font-size:24px;">Getting Started</span>**
1. **Library Installation**
  - *Ginn add text here*
2. **Port Selection**
  - The one thing that you'll have to do manually is select the port the *Base-Station* is connected to:
    1. Find the name of your device (It should be something similar to /dev/tty.usbmodem<numbers>)
    2. Navigate and open *../Plotter/serial_helper.py* using VSCode or any desired coding IDE
    3. Replace the text on line 5 for *serial_port = "<your device's serial address>"*
        Ex: *serial_port = "/dev/tty.usbmodem<numbers>"*
       - If python can't find your device you will receive this error, please ensure your device is propperly connected and that you have the right port address entered:
         ![image](https://github.com/caden602/Oval_Capstone/assets/59580833/b958010f-9e4b-41eb-a125-2662f880e5bc)
3. **Run The Binary**
  - Once you have connected your *Base-Station* and you have active *LunaSats* brodcasting information to your *Base-Station*, run the included binary to view the plotted data:
    1. In your terminal, navigate to *../Plotter/dist/*
    2. Run the command *./qtpy*
       - If you receive the error *"zsh: permission denied: ./qtpy"*
         run: *chmod +x qtpy* to resolve permission issues
    3. After data has been sent from your connected *LunaSats*, the graphs should begin populating with data
