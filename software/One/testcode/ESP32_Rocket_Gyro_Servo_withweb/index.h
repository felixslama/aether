const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>

<head>
</head>

<body style="width: 99%; height: 99%; background-color:#141441;">
  <section>

    <h1 style="font-size: 5em; padding-left: 0.4em; text-align: center; color:white;">AETHER ONE</h1>
    <h2 style="font-size: 3em; padding-left: 15%; color:white;"><u>TELEMETRY DATA STATUS</u></h2>
    <h2 style="font-size: 2.5em; padding-top:2.5em;  padding-left: 10%; color:white;" class="sensore_value"><u>Sensor Value: </u><span id="GyroValue"> 0</span></h1>
    <h2 style="font-size: 2.5em; padding-top:2.5em; padding-left: 10%; color:white;"><u>About Motor:</u></h2>
    <button id="current" onclick="changestatus()">Turn ON</button>
        <!-- Turn Motor <a href="On">ON</a><br>
        Turn Motor <a href="Off">OFF</a><br> -->

  </section>

  <script>
    setInterval(function () {
      getData();
    }, 500); //update rate

    function getData() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("GyroValue").innerHTML =
            this.responseText;
        }
      };
      xhttp.open("GET", "readGyro", true);
      xhttp.send();
    }
   
    function changestatus() {
      //Will be used for Motor later
      //Code needs to be added
      btn = document.getElementById("current");
      if (btn.innerHTML == "STATUS: ON"){
        btn.innerHTML = "STATUS: OFF";
        btn.style.color = "red";
      } else if (btn.innerHTML == "STATUS: OFF" || btn.innerHTML == "Turn ON") {
        btn.innerHTML = "STATUS: ON";
        btn.style.color = "lime";
      } 
  }

  </script>
  <style>
    button {
        background: #5a5a7a;
        color: #fff;
        
        /* For button Size */
        font-size: 340%;
        padding-top: 5%;
        width: 70%;
        padding-bottom: 5%;

        margin-top: 10px;
        margin-left: 15%;
        border: none;
        border-radius: 4px;
      }
  </style>
</body>
</html>

)=====";
