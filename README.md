# foxer_IoT_screen
ESP8266 / Arduino / Wemos TFT 2.4 >> Load up to 9 temperature sensors from Foxer API and display them on TFT

Requirements: (without adapting the code)
- Wemos D1 Mini Pro
- Wemos TFT 2.4 touch shield
- Foxer IoT sensors and account that can create a group of sensors and release the API key
- Schneider Exact junction box if using my 3d model (please note it needs modifications before using)
- Very short screws for attaching screen to faceplate
- Long screws that attach the plate to junctionbox usually come with the junctionbox

I had some amount of these sensors installed in my house and all the other members of my family never went online to see the readings. I thought it was a waste, because as a home owner I'm trying to keep the costs down and this temperature data is actually useful when we decide to start or stop the heaters.

Known bugs / issues
-------------------
- Sometimes (or almost every other time) the http.getString() seems to return empty or incomplete data and JSON parse fails. Considering a work around.
FIXED: I just made a simple retry 10 times before failure and haven't seen any problems in use since

Plan / done (O = not done, X = done, ? = idea)
----------------------------------------------

✔️ Draw 3 x 3 grid of boxes that change color depending on temperature

✔️ Show some naming inside those boxes

✔️ Connect to WiFi

✔️ Get JSON data from Foxer IoT server

✔️ Parse JSON data and write on the screen, in the correct boxes

✔️ 3D design and print box / cover for a commercial box (Schneider Exact most likely)

✔️ Show failure  (disconnect from internet, failed to connect server, temperature sensor not refreshing) >> did with different BG colors

❌ Add screen sleep mode and wake on touch (tried, would need to change connections so some pin can output screen led on/off, also probably need MOSFET or similar)

⌛ Second view after touching any of the sensor --> Show CO2 and humidity, draw a trend chart from last 24h

⌛ Maybe fix insecure https connection -- I can't imagine any scenario where this could be a problem in my project but it is a bad feature. Problem is the alternatives seemed pretty hacky too.

⌛ Translate the variable names and comments from Finnish to English

⌛ Compare the timestamps in some way to see if the received value is old or new and do something with that information

⌛ It would be nice if the Wifi SSID/pw, API key and such could be able to configurate through the screen, but as I'm already running against memory issues and also for finding doing that of little interest (to me), I probably won't be doing that. Branching very welcome!

Progress images:

Before Foxer IoT api connection was done

<img src="https://user-images.githubusercontent.com/32982491/154942843-f07caf5f-347d-4796-a2e8-3503d0c5f742.JPEG" width=50% height=50%>

After connection done and added triangles + background color showing state (alerts)

<img src="https://user-images.githubusercontent.com/32982491/155036398-5cdb9df4-e846-4c3b-8d55-a7ba1db2dfdb.JPEG" width=50% height=50%>

3d designed a faceplate for Schneider Exact junction box, in which I can install the Wemos d1 mini pro + tft 2.4 shield, haven't printed yet

![jbCover](https://user-images.githubusercontent.com/32982491/155220582-926cccac-ec1b-4171-a543-f148234fcb16.jpeg)

Printed the cover, WARNING several dimensions were wrong, I was able to make it work with a file and drill but this model needs adjustments before reuse
* Cut out top-bottom was not big enough and for that reason, the screw places were off too
* The screw places for the screen should be 1mm closer in distance to each other, and the base cylinder should be "filed" in 3d editor from the side that protrudes towards the cutout after that adjustment
* The screw holes were all too small, but the small hole was perfect for aligning the hole with a drill so I probably will not fix this as now it can be made work with whatever screws one has at hand
* The junction box was needleslly high, I would purchase a more shallow one next time for better looks
* The USB plug wouldn't fit, you're going to need a 90 degree plug or do it like me, mangle the connector with pliers and side cutters carefully so that what remains is the metal connector and wires. Careful to not let the bare wires touch each other!
* The cover would look a lot better with rounded corners and recessed screw hole on top
* The screen has maybe 3mm of stuff in the side that is not the actualy screen, I painted it with black marker so it looks "ok" but if I redid this I would print a 0.5mm-1mm thick cover in front of that area of the screen, perhaps whole area to hide the silver border too, as there is that amount of space there now where it's sitting (screen is not flush with the face but recessed maybe 1mm)

But overall towards the end I started to feel that this project will now either get done or get forgotten in the drawer. It works for me now and I'm pretty happy with it.

<img src="https://user-images.githubusercontent.com/32982491/156164213-7b3a7bae-20bd-42da-9ef6-b2c91a3df9fd.JPEG" width=50% height=50%>

