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
        height: 10%;
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

    .chart-wrapper {
        height: 85%;
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
    <title>Water Level Tank Bot</title>
    <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css"
        integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
    <script type="text/javascript" src="http://static.fusioncharts.com/code/latest/fusioncharts.js"></script>
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
                        "caption": "Real-time",
                        "subcaption": "visualization",
                        "lowerLimit": "0",
                        "upperLimit": "100",
                        "lowerLimitDisplay": "Empty",
                        "upperLimitDisplay": "Full",
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
                            var xhttp = new XMLHttpRequest();
                            xhttp.onreadystatechange = function () {
                                if (this.readyState == 4 && this.status == 200) {
                                    document.getElementById("level").innerHTML = this.responseText;
                                    evtObj.sender.feedData("&value=" + this.responseText);
                                    console.log("Measured value is:" + this.responseText);
                                }
                            };
                            xhttp.open("GET", "/level", true);
                            xhttp.send();
                        }, 1000);
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
            Water Level Tank Bot
        </div>
        <div class="main-data">
            <i class="fas fa-tint" style="color:#80bfff;"></i>
            <span class="labels">Level</span>
            <span id="level">???</span>
            <span class="units">%</span>
        </div>
    </div>
    <br>
    <div class="chart-wrapper">
        <div class="tank" id="chart-container"></div>
    </div>

</body>

</html>
    )rawliteral";
}