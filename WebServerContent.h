#pragma once

namespace WebServerContent {

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">

<head>
    <title>HydroNotify</title>
    <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css"
        integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body {
            font-family: Arial, sans-serif;
            display: flex;
            justify-content: normal;
            align-items: center;
            height: 95vh;
            margin: 0px;
            background-color: #f4f4f9;
            flex-direction: column;
            padding: 2vh;
        }

        .content {
            display: flex;
            justify-content: center;
            align-items: center;
            /*height: 100vh;*/
            margin: 2vh;
            flex-direction: column;
        }

        .title {
            font-size: 40px;
        }

        .main-data {
            font-size: 24px;
        }

        .parameters {
            font-size: 14px;
            padding: 10px;
        }


        #container {
            width: 200px;
            height: 400px;
            border: 5px solid #000;
            position: relative;
            background-color: #cce7ff;
            border-radius: 10px;
            overflow: visible;
        }

        #water {
            width: 100%;
            position: absolute;
            bottom: 0;
            background-color: #007bff;
            transition: height 1s ease;
        }

        #percentage {
            position: absolute;
            width: 100%;
            text-align: center;
            font-size: 24px;
            font-weight: bold;
            color: #fff;
            top: 50%;
            transform: translateY(-50%);
        }
    </style>
    <script> //Get parameters once at beginning. 
        function getParameters() {
            fetch('/getParameters')
                .then(response => response.json())
                .then(data => {
                    const maxLabel = document.getElementById('maxDepthLabel');
                    maxLabel.innerText = data.maxDepth;
                    const minLabel = document.getElementById('minDepthLabel');
                    minLabel.innerText = data.minDepth;
                    const alarmLabel = document.getElementById('alarmTriggerLabel');
                    alarmLabel.innerText = data.alarmTrigger;
                })
                .catch(error => console.error(error));
        }
        document.addEventListener("DOMContentLoaded", function () {
            getParameters();
        });
    </script>
    <script> // Get level every 1500ms
        async function fetchWaterLevel() {
            try {
                const response = await fetch('/level'); // URL de tu API
                const data = await response.json();
                console.log(data);
                return data.level; // Se asume que la respuesta es un objeto con la propiedad "level"
            } catch (error) {
                console.error('Error al obtener el nivel de agua:', error);
                return 0; // En caso de error, regresa 0
            }
        }

        function updateWaterLevel(level) {
            const water = document.getElementById('water');
            const percentage = document.getElementById('percentage');
            const levelLabel = document.getElementById('level');
            const height = `${level}%`;

            water.style.height = height;
            percentage.textContent = height;
            levelLabel.textContent = height;
        }

        async function updateTank() {
            const level = await fetchWaterLevel();
            updateWaterLevel(level);
        }

        // Actualizar el tanque cada 1.5 segundos
        setInterval(updateTank, 1500);

        // Cargar la primera vez
        updateTank();
    </script>
    <script> // Get distance every 1500ms
        function updateDistance() {
            setInterval(function () {
                var xhttp2 = new XMLHttpRequest();
                xhttp2.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        document.getElementById("distance").innerHTML = this.responseText;
                        console.log("La distancia medida es: " + this.responseText);
                    }
                };
                xhttp2.open("GET", "/distance", true);
                xhttp2.send();

            }, 1500);
        }
        updateDistance();
    </script>
</head>

<body>
    <div class="title">
        HydroNotify
    </div>

    <div class="content">
        <div class="main-data">
            <center>
                <i class="fas fa-tint" style="color:#80bfff;"></i>
                <span class="labels">Nivel</span>
                <span id="level">? %</span>
            </center>

            <div class="parameters">
                <a href="settings"><i class="fas fa-cog" style="color:#8b99b0;"></i></a>
                <span class="labels">Profundidad Maxima del Tanque: </span>
                <span id="maxDepthLabel">???</span>
                <span class="units">cm</span><br>

                <a href="settings"><i class="fas fa-cog" style="color:#8b99b0;"></i></a>
                <span class="labels">Profundidad Minima del Tanque: </span>
                <span id="minDepthLabel">???</span>
                <span class="units">cm</span><br>

                <a href="settings"><i class="fas fa-cog" style="color:#8b99b0;"></i></a>
                <span class="labels">Distancia medida: </span>
                <span id="distance">???</span>
                <span class="units">cm</span><br>

                <!--
                <a href="settings"><i class="fas fa-cog" style="color:#8b99b0;"></i></a>
                <span class="labels">Disparar Alarma a: </span>
                <span id="alarmTriggerLabel">???</span>
                <span class="units">%</span><br>
                -->
            </div>
        </div>

        <br>

        <!--Tank container-->
        <div id="container">
            <div id="water" style="height: 0;"></div>
            <div id="percentage">0%</div>
        </div>
    </div>



</body>

</html>
)rawliteral";

const char settings_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>

<head>
    <meta charset="UTF-8">
    <title>Settings</title>
    <style>
        body {
            height: 100%;
            background-color: rgb(197, 197, 186);
            background-color: #f8f4f4;
            margin: auto;
            padding: auto;
            position: relative;
            text-align: center;
            vertical-align: middle;
        }

        .title {
            font-size: 48px;
            color: rgb(0, 0, 0);
        }

        .settingsForm {
            font-size: 42px;
            color: rgb(56, 56, 17);
            text-align: center;
        }

        .factoryResetButton {
            background-color: rgb(95, 23, 23);
            color: white;
            margin-top: 50px;
            padding: 10px 15px;
            font-size: 16px;
            border: none;
            cursor: pointer;
        }

        .saveButton {
            background-color: rgb(34, 23, 95);
            color: white;
            padding: 10px 15px;
            font-size: 16px;
            border: none;
            cursor: pointer;
        }

        .cancelButton {
            background-color: rgb(136, 130, 46);
            color: white;
            padding: 10px 15px;
            font-size: 16px;
            border: none;
            cursor: pointer;
        }

        .buttonContainer {
            text-align: center;
        }
    </style>
    <script>
        function getParameters() {
            fetch('/getParameters')
                .then(response => response.json())
                .then(data => {
                    const maxInput = document.getElementById('maxDepthInput');
                    maxInput.value = data.maxDepth;
                    const minInput = document.getElementById('minDepthInput');
                    minInput.value = data.minDepth;
                    const alarmInput = document.getElementById('alarmTriggerInput');
                    alarmInput.value = data.alarmTrigger;
                })
                .catch(error => console.error(error));
        }
        document.addEventListener("DOMContentLoaded", function () {
            getParameters();
        });
    </script>
</head>

<body>
    <div class="title">
        <h1>Configuración del Tanque</h1>
    </div>

    <form id="settings-form" class=settingsForm>
        <label for="max">Profundidad Máxima:</label> &nbsp;
        <input type="number" name="max" id="maxDepthInput">
        <label>cm</label><br>

        <label for="min">Profundidad Mínima:</label> &nbsp;
        <input type="number" name="min" id="minDepthInput">
        <label>cm</label><br>

        <label for="alarmTrigger">Alertar al:</label> &nbsp;
        <input type="number" name="alarmTrigger" id="alarmTriggerInput">
        <label>%</label><br>

        <label for="telegramToken">Cambiar Token:</label> &nbsp;
        <input type="text" name="telegramToken" id="telegramTokenInput">
        <br>

        <label for="telegramChatID">Cambiar ChatID:</label> &nbsp;
        <input type="text" name="telegramChatID" id="telegramChatIDInput">
        <br>

        <div class="buttonContainer">
            <button class="saveButton" type="submit">Guardar</button>
            <button class="cancelButton" type="submit" id="cancelBtn">Cancelar</button>
        </div>
    </form>

    <button class="factoryResetButton" onclick="factoryResetAlert()">Restablecer a Valores de Fábrica</button>

    <script>
        const form = document.getElementById('settings-form');
        form.addEventListener('submit', function (event) {
            event.preventDefault();
            const data = {
                "status": "ok",
                maxDepth: form.max.value,
                minDepth: form.min.value,
                alarmTrigger: form.alarmTrigger.value,
            };

            const telegramTokenInput = document.getElementById('telegramTokenInput');
            const telegramToken = telegramTokenInput.value.trim();
            if (telegramToken !== '') {
                data.telegramToken = telegramToken;
            }

            const telegramChatIDInput = document.getElementById('telegramChatIDInput');
            const telegramChatID = telegramChatIDInput.value.trim();
            if (telegramChatID !== '') {
                data.telegramChatID = telegramChatID;
            }

            fetch('/setParameters', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json; charset=utf-8'
                },
                body: JSON.stringify(data)
            })
                .then(response => response.json())
                .then(data => {
                    console.log(data);
                    window.location.href = "/";
                })
                .catch(error => console.error(error));
        });

    </script>

    <script type="text/javascript">
        document.getElementById("cancelBtn").onclick = function () {
            location.href = "/";
        };
    </script>

    <script>
        function factoryResetAlert() {
            var respuesta = confirm("¿Restablecer a Valores de Fábrica?");

            if (respuesta) {
                window.location.href = "/factoryReset";
            } else {
                // No hacer nada si se presiona Cancelar
            }
        }
    </script>

</body>

</html>
)rawliteral";

}  // namespace WebServerContent
