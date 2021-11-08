const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>

<head>
</head>

<body class="backscreen">
  <section>

    <h1 class="glow">AEOTHER ONE</h1>
    <h2 class="leading_text"><u>TELEMETRY DATA: </u></h2>
    <article>
      <h2 class="sensore_value"><u>Sensor Value: </u><span id="GyroValue"> 0</span></h1>
        <br>
        <!-- Turn Motor <a href="On">ON</a><br>
        Turn Motor <a href="Off">OFF</a><br> -->

    </article>
    <article>
      <h2 class="motor_value"><u>Motor: </u></h2>
      <label class="toggle">
        <span class="onoff">Sleep</span>
        <div id='toggleEngine'>
          <input type="checkbox" />
          <span class="slider round"></span>
        </div>
      </label>
    </article>
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
    const toggle = document.querySelector('.toggle input')

    toggle.addEventListener('click', () => {
      const onOff = toggle.parentNode.querySelector('.onoff')
      onOff.textContent = toggle.checked ? 'ON' : 'OFF'
    })
  </script>
  <style>
    * {
      margin: 0;
      padding: 0;
      box-sizing: border-box;
    }

    body {
      display: block;
      justify-content: center;
      align-items: center;
    }

    h2{
      font-size: 3.5em;
    }
    section {
      width: 100%;
      border: 5px solid #000;
    }

    article {
      border: 5px solid #23262E;
      margin-left: 40px;
      margin-right: 40px;
    }

    .backscreen {
      background-color: #383838;
    }

    .leading_text {
      text-align: left;
      padding-top: 0.4em;
      padding-left: 0.4em;
      margin-bottom: 0.2em;
      font-size: 4em;
    }

    .sensore_value {
      padding-left: 1em;
    }

    .motor_value {
      padding-left: 1em;
    }






    .toggle {
      position: relative;
    }

    .toggle input {
      position: absolute;
      opacity: 0;
      width: 0;
      height: 0;
    }

    .toggle .onoff {
      color: #fff;
      font-size: 2em;
      text-align: center;
      display: block;
      font-family: Arial, Helvetica, sans-serif;
    }

    .slider {
      position: relative;
      display: block;
      cursor: pointer;
      background-color: #333;
      transition: 0.4s;
      width: 200px;
      height: 120px;

    }

    .slider:before {
      justify-content: center;
      align-items: center;
      content: "";
      position: absolute;
      height: 100px;
      width: 100px;
      background-color: #fff;
      transition: 0.4s;
      top: 10px;
      left: 10px;
    }

    input:checked+.slider {
      background-color: #fd5d00;
      box-shadow: 0 0 12px #fd5d00;
    }

    input:checked+.slider:before {
      transform: translateX(80px);
    }

    .slider.round {
      border-radius: 20px;
    }

    .slider.round::before {
      border-radius: 20px;
    }


    .glow {
      font-size: 6em;
      color: #fff;
      text-align: center;
      -webkit-animation: glow 1s ease-in-out infinite alternate;
      -moz-animation: glow 1s ease-in-out infinite alternate;
      animation: glow 1s ease-in-out infinite alternate;
    }

    @-webkit-keyframes glow {
      from {
        text-shadow: 0 0 10px #fff, 0 0 20px #fff, 0 0 30px #23262E, 0 0 40px #23262E, 0 0 50px #23262E, 0 0 60px #23262E, 0 0 70px #23262E;
      }

      to {
        text-shadow: 0 0 20px #fff, 0 0 30px #4C4C4C, 0 0 40px #4C4C4C, 0 0 50px #4C4C4C, 0 0 60px #4C4C4C, 0 0 70px #4C4C4C, 0 0 80px #4C4C4C;
      }
    }
    #toggleEngine{
      margin-left: 38%;
    }
  </style>
</body>

</html>
)=====";
