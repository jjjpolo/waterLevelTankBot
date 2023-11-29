#pragma once

namespace WebServerContent
{
    const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<style>
    html,
    body {
        height: 100%;
        background-color: #f8f4f4;
    }

    .header {
        height: 20%;
        width: 100%;
        margin: auto;
        padding: auto;
        position: relative;
        text-align: center;
        vertical-align: middle;
    }

    .title {
        font-size: 48px;
    }

    .main-data {
        font-size: 32px;
    }

    .parameters {
        font-size: 18px;
        padding: 10px;
    }

    .chart-wrapper {
        height: 75%;
        width: 75%;
        margin: auto;
        text-align: center;
    }

    #chart-container {
        height: 100%;
        width: 100%;
    }
</style>

<head>
    <title>HydroNotify</title>
    <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css"
        integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
    <script type="text/javascript" src="http://static.fusioncharts.com/code/latest/fusioncharts.js"></script>
    <script>
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
    <script type="text/javascript">
        FusionCharts.ready(function () {
            var fusioncharts = new FusionCharts({
                "type": "cylinder",
                "dataFormat": "json",
                "id": "fuelMeter",
                "renderAt": "chart-container",
                "width": "100%",
                "height": "100%",
                "dataSource": {
                    "chart": {
                        "theme": "fint",
                        "caption": "Visualizacion en Tiempo Real",
                        "subcaption": "",
                        "lowerLimit": "0",
                        "upperLimit": "100",
                        "lowerLimitDisplay": "Vacio",
                        "upperLimitDisplay": "Lleno",
                        "numberSuffix": " %",
                        "showValue": "1",
                        "chartBottomMargin": "25",
                        "cylfillcolor": "#80bfff",
                        "backgroundColor": "#ff4d88",
                        "baseFont": "Verdana",
                        "baseFontSize": "22",
                    },
                    "value": "3.3"
                },
                "events": {
                    "rendered": function (evtObj, argObj) {
                        setInterval(function () {
                            // Get level
                            var xhttp = new XMLHttpRequest();
                            xhttp.onreadystatechange = function () {
                                if (this.readyState == 4 && this.status == 200) {
                                    document.getElementById("level").innerHTML = this.responseText;
                                    evtObj.sender.feedData("&value=" + this.responseText);
                                    console.log("El valor medido es: " + this.responseText);
                                }
                            };
                            xhttp.open("GET", "/level", true);
                            xhttp.send();
                            // Get distance
                            var xhttp2 = new XMLHttpRequest();
                            xhttp2.onreadystatechange = function () {
                                if (this.readyState == 4 && this.status == 200) {
                                    document.getElementById("distance").innerHTML = this.responseText;
                                    console.log("La distancia medida es: " + this.responseText);
                                }
                            };
                            xhttp2.open("GET", "/distance", true);
                            xhttp2.send();
                        }, 500);
                    }
                }
            });
            fusioncharts.render();
        });
    </script>
</head>

<body>
    <div class="header">
        <div class="title">
            HydroNotify
        </div>
        <div class="main-data">
            <i class="fas fa-tint" style="color:#80bfff;"></i>
            <span class="labels">Nivel</span>
            <span id="level">???</span>
            <span class="units">%</span><br>

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
    </div>
    <br>
    <div class="chart-wrapper">
        <div class="tank" id="chart-container"></div>
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
}