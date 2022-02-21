# foxer_IoT_screen
ESP8266 / Arduino / Wemos TFT 2.4 >> Load up to 9 temperature sensors from Foxer API and display them on TFT

Requirements: (without adapting the code)
- Wemos D1 Mini Pro
- Wemos TFT 2.4 touch shield
- Foxer IoT sensors and account that can create a group of sensors and release the API key

I had some amount of these sensors installed in my house and all the other members of my family never went online to see the readings. I thought it was a waste, because as a home owner I'm trying to keep the costs down and this temperature data is actually useful when we decide to start or stop the heaters.

Plan / done (O = not done, X = done, ? = idea)
----------------------------------------------

X Draw 3 x 3 grid of boxes that change color depending on temperature

X Show some naming inside those boxes

X Connect to WiFi

X Get JSON data from Foxer IoT server

X Parse JSON data and write on the screen, in the correct boxes

O 3D design and print box / cover for a commercial box (Schneider Exact most likely)

X/Kinda Show failure icons (disconnect from internet, failed to connect server, temperature sensor not refreshing)

? Second view after touching any of the sensor --> Show CO2 and humidity, draw a trend chart from last 24h

? Maybe fix insecure https connection -- I can't imagine any scenario where this could be a problem in my project but it is a bad feature. Problem is the alternatives seemed pretty hacky too.

? Translate the variable names and comments from Finnish to English

? Compare the timestamps in some way to see if the received value is old or new and do something with that information

It would be nice if the Wifi SSID/pw, API key and such could be able to configurate through the screen, but as I'm already running against memory issues and also for finding doing that of little interest (to me), I probably won't be doing that. Branching very welcome!

Progress images:

Before Foxer IoT api connection was done

<img src="https://user-images.githubusercontent.com/32982491/154942843-f07caf5f-347d-4796-a2e8-3503d0c5f742.JPEG" width=50% height=50%>

After connection done and added triangles + background color showing state (alerts)

<img src="https://user-images.githubusercontent.com/32982491/155036398-5cdb9df4-e846-4c3b-8d55-a7ba1db2dfdb.JPEG" width=50% height=50%>

Next up is casing, then we'll see if I have interest in continuing to develop more features. :)


