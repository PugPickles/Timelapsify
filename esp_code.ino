#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>


// Cam
int cam_inp = 5;      // D1   Makes the camera think that a hand release is plugged in (camera turns off display)
int cam_trigger = 4;  // D2   Triggers the camera to take a photo


// Wifi data (if the app is used, this must also be changed for the app)
const char* ssid = "Timelapsify";
const char* password = "g3t_th3_fuck_out";

// IP stuff, if changed must also be changed for app
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

ESP8266WebServer server(80);


// Variables for timer and so, do not change anything!
bool timer_enabled = false;
int timer_time = 0;
int timer_count = 0;

int saved_time = 0;
int saved_count = 0;

bool letGo = false;

static long timer_milis;
static long blink_milis;


// Website, the beginning (<!DOCTYPE html><html>...) comes below
const char index_html[] = R"=====(
    <link rel="icon" type="image/png" sizes="64x64"
        href="data:image/png;base64,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">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
    <title>Timelapsify</title>
    <style>
        * {
            color: #dfdfdf;
            background-color: #1d1d1d;
        }

        *:focus {
            outline: none;
        }

        input[type=number]::-webkit-inner-spin-button,
        input[type=number]::-webkit-outer-spin-button {
            -webkit-appearance: none;
            -moz-appearance: none;
            appearance: none;
        }

        input[type=number] {
            -moz-appearance: textfield;
        }

        html,
        body {
            position: relative;
            width: 100%;
            max-width: 700px;
            height: 100%;
            margin: auto;
            top: 0;
            left: 0;
            right: 0;
        }

        error {
            position: absolute;
            z-index: 9;
            width: 75%;
            margin: auto;
            left: 0px;
            right: 0px;
            color: #ff5050;
            background-color: rgb(24, 24, 24);
            padding: 15px;
            border: 1px solid;
            border-top: 0;
            border-bottom-left-radius: 10px;
            border-bottom-right-radius: 10px;
            box-shadow: 0 0 10px #ff5050;
            top: -100px;
            font-weight: bold;
            text-align: center;
            line-height: 30px;
        }

        .err_opn {
            -webkit-animation: err_opn 0.5s forwards;
            animation: err_opn 0.5s forwards;
        }

        @keyframes err_opn {
            from {
                top: -100px;
            }

            to {
                top: 0px;
            }
        }

        .err_cls {
            -webkit-animation: err_cls 0.5s forwards;
            animation: err_cls 0.5s forwards;
        }

        @keyframes err_cls {
            from {
                top: 0px;
            }

            to {
                top: -100px;
            }
        }

        .progress_wrp {
            position: relative;
            height: fit-content;
            width: 100%;
            display: grid;
            place-items: center;
            padding-top: 20px;
        }

        .prg {
            position: relative;
            width: 100%;
            border-radius: 50%;
            display: grid;
            place-items: center;
            height: 0;
            padding-bottom: 100%;
        }

        .prg:before {
            content: "";
            position: absolute;
            height: 84%;
            width: 84%;
            background-color: #181818;
            border-radius: 50%;
        }

        .prg_val {
            position: absolute;
            font-size: 25px;
            color: #aaaaaa;
            background-color: #181818;
        }

        input,
        select,
        button {
            box-sizing: border-box;
            border: 1px solid;
            border-radius: 10px;
            padding: 10px;
            width: 100%;
            margin-top: 10px;
            margin-bottom: 10px;
            text-align: center;
        }

        table {
            width: 100%;
        }

        select {
            -webkit-appearance: none;
            -moz-appearance: none;
            text-indent: 1px;
            text-overflow: '';
        }

        button {
            position: relative;
            width: 100px;
            display: block;
            margin: auto;
            margin-top: 10px;
            margin-bottom: 10px;
        }

        usrinp {
            display: block;
            width: 75%;
            margin: auto;
            margin-top: 15px;
        }

        total_time {
            display: block;
            width: 100%;
            text-align: center;
            height: 19px;
        }

        #stop {
            display: none;
            color: #ff5050;
            margin-top: 20px;
        }

        timt_img_cal {
            position: relative;
            margin: auto;
            top: 20px;
            margin-top: 20px;
            background-color: #181818;
            display: block;
            width: 75%;
            height: fit-content;
            border-radius: 10px;
            padding: 10px;
        }

        timt_img_cal>* {
            background-color: #181818;
        }

        timt_img_cal>p {
            margin-bottom: 0;
        }
    </style>
</head>

<body>
    <error id="error"></error>

    <div class="progress_wrp">
        <table style="table-layout: fixed;">
            <tr>
                <td style="text-align: center;">
                    <a style="font-size: 20px; color: #e8702a;">Time</a>
                    <div class="prg progress_time">
                        <div class="prg_val progress_time_value">--:--:--</div>
                    </div>
                </td>
                <td style="text-align: center;">
                    <a style="font-size: 20px; color: #71bc78;">Images</a>
                    <div class="prg progress_imgs">
                        <div class="prg_val progress_imgs_value">-/-</div>
                    </div>
                </td>
            </tr>
        </table>
    </div>

    <usrinp id="usrinp">
        <total_time id="total_time"></total_time>

        <table style="margin: auto;">
            <tr>
                <td>
                    <input type="number" placeholder="Timer" id="timer">
                </td>
                <td>
                    <select id="mode">
                        <option value="0" selected>Seconds</option>
                        <option value="1">Minutes</option>
                        <option value="2">Hours</option>
                    </select>
                </td>
            </tr>
        </table>

        <input type="number" placeholder="Images" id="images">
        <button id="start">Start</button>
    </usrinp>

    <stopBtn>
        <button id="stop">Stop</button>
    </stopBtn>

    <timt_img_cal id="timt_img_cal">
        <h2 style="margin: 0; text-align: center;">Timer calculator</h2>
        <p>Video</p>
        <input type="number" placeholder="FPS" id="cal_vid_fps">
        <input type="number" placeholder="Length (s)" id="cal_vid_length">

        <p>Time per image</p>
        <input type="time" id="cal_vid_timer" value="00:00:00" min="00:00:05" step="1">

        <button id="start_cal">Calculate</button>
    </timt_img_cal>
</body>


<script>
    let xhr = new XMLHttpRequest();
    let input_blocked = false;



    function error(msg) {
        let err = document.getElementById("error");

        err.innerHTML = msg;

        err.classList.remove("err_cls");
        err.classList.add("err_opn");

        setTimeout(function () {
            err.classList.remove("err_opn");
            err.classList.add("err_cls");

            setTimeout(function () {
                err.innerHTML = "";
            }, 500)
        }, 5000);
    }



    // Calculate inp to ms
    function inp_to_ms(mode, time) {
        if (mode == 0) {
            time = time * 1000;
        }

        if (mode == 1) {
            time = time * 60 * 1000;
        }

        if (mode == 2) {
            time = time * 60 * 60 * 1000;
        }

        return time;
    }



    function percent_cal(current, total) {
        let prc = (current / total) * 100;
        return prc.toFixed(0);
    }



    function inp_manager() {
        let inp = document.getElementById("usrinp");
        let cal = document.getElementById("timt_img_cal");

        if (input_blocked) {
            input_blocked = false;

            inp.style.pointerEvents = "all";
            inp.style.opacity = "1";

            cal.style.pointerEvents = "all";
            cal.style.opacity = "1";

            document.getElementById("stop").style.display = "none";

        } else {
            input_blocked = true;

            inp.style.pointerEvents = "none";
            inp.style.opacity = "0.5";

            cal.style.pointerEvents = "none";
            cal.style.opacity = "0.5";

            document.getElementById("stop").style.display = "block";
        }
    }



    // Calculate total time
    document.getElementById('images').addEventListener("input", function () {
        let timer = document.getElementById('timer').value;
        let mode = document.getElementById("mode").value;
        let images = document.getElementById('images').value;

        timer_ms = inp_to_ms(mode, timer)


        document.getElementById('total_time').innerHTML = "Total Time: " + new Date(timer_ms * images).toISOString().slice(11, 19);
    });



    // Start Timer
    document.getElementById("start").addEventListener("click", function () {
        let timer = document.getElementById('timer').value;
        let mode = document.getElementById("mode").value;
        let images = document.getElementById('images').value;

        let timer_ms = "";

        if (is_timer_running) {
            return;
        }

        // Check Input
        if (timer == "") {
            document.getElementById('timer').style.border = "1px solid #f33";
            return;

        } else {
            document.getElementById('timer').style.border = "1px solid #dfdfdf";
        }

        if (images == "") {
            document.getElementById('images').style.border = "1px solid #f33";
            return;

        } else {
            document.getElementById('timer').style.border = "1px solid #dfdfdf";
        }

        timer_ms = inp_to_ms(mode, timer)

        // Send
        xhr.onreadystatechange = function () {
            if (this.readyState == 4) {
                if (this.status == 200) {
                    is_timer_running = true;

                    inp_manager();

                    setTimeout(function () {
                        autosync();
                    }, 250);
                } else {
                    error("NETWORK ERROR<br>Could not start timer");
                }
            }
        };

        xhr.open("GET", "/timer/go?timer=" + timer_ms + "&imgs=" + images);
        xhr.send();
    });



    // Stop Timer
    document.getElementById("stop").addEventListener("click", function () {
        xhr.onreadystatechange = function () {
            if (this.readyState == 4) {
                if (this.status == 200) {
                    location.reload();
                } else {
                    error("NETWORK ERROR<br>Could not stop timer");
                }                
            }
        };
        xhr.open("GET", "/timer/stop");
        xhr.send();
    });



    function counter(timer_ms, images, total_imgs) {
        let progressBar_time = document.querySelector(".progress_time");
        let valueContainer_time = document.querySelector(".progress_time_value");
        let progressBar_imgs = document.querySelector(".progress_imgs");
        let valueContainer_imgs = document.querySelector(".progress_imgs_value");

        let progressValue_time = 0;
        let progressValue_imgs = 0;

        let progressEndValue_time = percent_cal(timer_ms * images, timer_ms * total_imgs);
        let progressEndValue_imgs = percent_cal(total_imgs - images, total_imgs);

        let progress_time_interval = setInterval(() => {
            progressValue_time++;
            valueContainer_time.textContent = `${new Date(timer_ms * images).toISOString().slice(11, 19)}`;
            progressBar_time.style.background = `conic-gradient(
            #e8702a ${progressValue_time * 3.6}deg,
            #181818 ${progressValue_time * 3.6}deg
        )`;
            if (progressValue_time == progressEndValue_time) {
                clearInterval(progress_time_interval);
            }
        }, 1);

        let progress_imgs_interval = setInterval(() => {
            progressValue_imgs++;
            valueContainer_imgs.textContent = `${total_imgs - images}/${total_imgs}`;
            progressBar_imgs.style.background = `conic-gradient(
            #71bc78 ${progressValue_imgs * 3.6}deg,
            #181818 ${progressValue_imgs * 3.6}deg
        )`;
            if (progressValue_imgs == progressEndValue_imgs) {
                clearInterval(progress_imgs_interval);
            }
        }, 1);
    }



    // After page reload, autosync
    function autosync() {
        xhr.onreadystatechange = function () {
            if (this.readyState == 4) {
                if (this.status == 200) {
                    let data = xhr.responseText.split(":");

                    let saved_time = data[0];
                    let saved_count = data[1];
                    let timer_count = data[2];

                    if (timer_count == 0) {
                        location.reload();
                        return;
                    }

                    counter(saved_time, timer_count, saved_count);

                    setTimeout(function () {
                        autosync();
                    }, saved_time);
                } else {
                    error("NETWORK ERROR<br>Could not sync data");
                }
            }
        };
        xhr.open("GET", "/timer/sync");
        xhr.send();
    }



    // Timer calculator
    document.getElementById("start_cal").addEventListener("click", function () {
        let fps = document.getElementById("cal_vid_fps").value;
        let length = document.getElementById("cal_vid_length").value;
        let timer = document.getElementById('cal_vid_timer').value.split(":");

        let h = timer[0];
        let m = timer[1];
        let s = timer[2];

        if (fps == "") {
            document.getElementById('cal_vid_fps').style.border = "1px solid #f33";
            return;

        } else {
            document.getElementById('cal_vid_fps').style.border = "1px solid #dfdfdf";
        }

        if (length == "") {
            document.getElementById('cal_vid_length').style.border = "1px solid #f33";
            return;

        } else {
            document.getElementById('cal_vid_length').style.border = "1px solid #dfdfdf";
        }

        if (h == 00 && m == 00 && s == 00) {
            document.getElementById('cal_vid_timer').style.border = "1px solid #f33";
            return;

        } else {
            document.getElementById('cal_vid_timer').style.border = "1px solid #dfdfdf";
        }


        let vid_imgs = Math.floor(fps * length);

        let timer_s = Math.floor((h * 3600) + (m * 60) + parseFloat(s));

        let img_for_vid = Math.floor(vid_imgs * timer_s).toFixed(0);

        document.getElementById('timer').value = timer_s;
        document.getElementById('images').value = img_for_vid;

        document.getElementById('images').dispatchEvent(new Event('input'));
    });



    if (is_timer_running) {
        autosync();
        inp_manager();
    }
</script>

</html>
)=====";


// Handle request, send page with the variable if the timer is running
void handle_OnConnect() {
  server.send(200, "text/html", "<!DOCTYPE html><html><head><script>let is_timer_running = " + String(timer_enabled) + ";</script>" + index_html); 
}

// Start Timer
void handle_timer_go() {
  // Get data from Client
  timer_time = server.arg("timer").toInt();
  timer_count = server.arg("imgs").toInt();

  // Save for Client
  saved_time = timer_time;
  saved_count = timer_count;
  
  // Enable Timer
  timer_enabled = true;
  digitalWrite(LED_BUILTIN, HIGH);

  // Makes camera think that manual release was plugged in
  digitalWrite(cam_inp, LOW);

  timer_count--;

  // Makes LED flash and takes photo
  letGo = true;
      
  // Non blocking delay
  timer_milis = millis();
  blink_milis =  millis();  

  // Conf req
  server.send(200);
}

// Sync data from ESP and CLient
void handle_timer_sync(){
  server.send(200, "text/plain", String(saved_time) + ":" + String(saved_count) + ":" + String(timer_count));
}

// Check if timer enabled (only app)
void handle_app() {
  if (timer_enabled) {
    server.send(204);

  } else {
    server.send(205); 
  }
}

// Well, stops the timer
void handle_timer_stop() {
  timer_time = 0;
  saved_time = 0;
  saved_count = 0;

  timer_enabled = false;
  digitalWrite(LED_BUILTIN, LOW);

  digitalWrite(cam_inp, HIGH);

  server.send(200);
}

// 404 Error
void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}



void setup() {
  // Internal led
  pinMode(LED_BUILTIN, OUTPUT);

  // Cam
  pinMode(cam_inp, OUTPUT);
  pinMode(cam_trigger, OUTPUT);

  // Would otherwise trigger photo
  digitalWrite(cam_inp, HIGH);
  digitalWrite(cam_trigger, HIGH);

  // AP config
  WiFi.softAPConfig(local_ip, gateway, subnet);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password, 1, 0, 1);
  
  // Webserver  
  server.on("/", handle_OnConnect);
  server.on("/timer/go", handle_timer_go);
  server.on("/timer/sync", handle_timer_sync);
  server.on("/app/check", handle_app);
  server.on("/timer/stop", handle_timer_stop);
  server.onNotFound(handle_NotFound);
  
  server.begin();
}



void loop() {
  server.handleClient();

  // Timer
  if (timer_enabled) {
    if (millis() - timer_milis >= timer_time) {
      timer_count--;
      letGo = true;
      
      timer_milis = millis();
      blink_milis =  millis();
    }
  }

  // Count end, stop timer
  if (timer_count == 0) {
    timer_enabled = false;
    timer_time = 0;
    saved_time = 0;
    saved_count = 0;

    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(cam_inp, HIGH);
  }

  // Blick and make photo
  if (letGo) {
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(cam_trigger, LOW);
    
    if (millis() - blink_milis >= 250) {
      digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(cam_trigger, HIGH);

      letGo = false;
    }
  }
}
