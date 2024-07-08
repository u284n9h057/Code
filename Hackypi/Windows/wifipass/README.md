All scripts are for Windows computers.

wifipassv4.0 saves all the Wi-Fi connection details and appends to a file on the hackypi sd card

wifipassv4.1 saves the current connection details and appends to a file on the hackypi sd card

For some reason once you write a file to the root of the HackyPi it cannot be deleted while the script is running.  something to do with security or so whst i found so far reckons.  so script now appends to the file on the sd but overwrites the root file.  this way the file deosn't get too big as there is only 1mb of space for scripts. 

Next step is to run them straight from the sd card rather than the root.