const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
  <body>
    <h1>TELEMETRY DATA:</h4><br>
    <h1>Sensor Value:<span id="GyroValue">0</span></h1>
    <br>
    Turn Motor <a href="On">ON</a><br>
    Turn Motor <a href="Off">OFF</a><br>
    <hr>
  <script>
    setInterval(function() {
      getData();
    }, 500); //update rate
    
    function getData() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("GyroValue").innerHTML =
          this.responseText;
        }
      };
      xhttp.open("GET", "readGyro", true);
      xhttp.send();
    }
  </script>
  </body>
</html>
)=====";
