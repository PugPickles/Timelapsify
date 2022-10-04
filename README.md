
<img src="https://github.com/PugPickles/Timelapsify/blob/7068fa27c50102a97d95cfb147f8dd687d9ee444/stuff/icon.png" width="256" heigh="256">

# Timelapsify

### Tested with Canon EOS cameras, may be that for other cameras things must be changed


<img src="https://github.com/PugPickles/Timelapsify/blob/a6e96a25002bcc3cf8ba47c89c66e3b3aa1654fc/stuff/scheme.png">

## Note

In my case it is important to connect GND, otherwise the triggering will not work.

The pins for the connector do not have to be the same, test best with a cable which pin is for what (And check if the pins to trigger and detect (manual release) are high (3.3V))


## Use

* Connect ESP with power
* Use the app (automatically connects to wifi and displays website in webview)

or

* Connect to the wifi ```Timelapsify``` with pass ```g3t_th3_fuck_out```
* Call the page ```http://192.168.1.1``` in the browser


<img src="https://github.com/PugPickles/Timelapsify/blob/4c4e57f397a1dd98b67742ae611588ed06f26919/stuff/Screenshot.jpg" width="400">

The Android app is quick and dirty, so is a bit buggy


## ToDo
* Website > Error detection for XMLHttpRequest and display
* App > No longer call the page directly but put it in the app (problems loading the page)
* App > Detect when wifi was disconnected
* Timer Calculator > Change from total images/total time to video time (e.g. 60spf) and time between images, Calculation for required number of images
