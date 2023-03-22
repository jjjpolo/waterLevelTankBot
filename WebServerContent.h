#pragma once

namespace WebServerContent
{
    const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<style>
    .raphael-group-7-background rect {
        fill: rgb(218, 218, 218) !important;
    }

    [id*=flat-slider].ui-slider.ui-slider-vertical {
        height: 220px;
        margin-top: 90px;
        margin-right: 15%;
        margin-bottom: 90px;
        margin-left: 15%;
    }

    $bg: #434d5a;

    [id*=flat-slider].ui-slider,
    [id*=flat-slider].ui-slider .ui-slider-pip .ui-slider-line {
        background: lighten($bg, 15%);
    }

    [id*=flat-slider].ui-slider .ui-slider-handle .ui-slider-tip:after {
        border-left-color: #434d5a;
    }

    [id*=flat-slider].ui-slider .ui-slider-handle.ui-state-hover,
    [id*=flat-slider].ui-slider .ui-slider-handle.ui-state-focus,
    [id*=flat-slider].ui-slider .ui-slider-handle.ui-state-active {
        border-color: white;
    }

    .stuff {
        padding: 10px 5px 5px;
        max-width: 150px;
        max-height: 550px;
    }

    div.inline {
        float: left;
    }

    .clearBoth {
        clear: both;
    }
</style>

<head>
    <title>Water Level Tank Bot - Web application</title>
    <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
    <script type="text/javascript" src="http://static.fusioncharts.com/code/latest/fusioncharts.js"></script>
</head>

<body>
    <div style="width:600px;margin:auto;">
        <i class="fas fa-thermometer-half"  style="color:#059e8a;"></i>
        <span class="dht-labels">Level</span> 
        <span id="level">%LEVEL%</span>
        <sup class="units">&deg;cm</sup>
    </div>
    <br>
    <div style="width:600px;margin:auto;">
        <div style="margin-left: 10px;" class="inline" id="chart-container">FusionCharts XT will load here!</div>
    </div>
</body>
<script type="text/javascript">
    FusionCharts.ready(function () {
        var fusioncharts = new FusionCharts({
            "type": "cylinder",
            "dataFormat": "json",
            "id": "fuelMeter",
            "renderAt": "chart-container",
            "width": "200",
            "height": "350",
            "dataSource": {
                "chart": {
                    "theme": "fint",
                    "caption": "Water Level Tank Bot",
                    "subcaption": "at my Home",
                    "lowerLimit": "0",
                    "upperLimit": "15",
                    "lowerLimitDisplay": "Empty",
                    "upperLimitDisplay": "Full",
                    "numberSuffix": " ltrs",
                    "showValue": "1",
                    "chartBottomMargin": "25",
                    "cylfillcolor": "#80bfff",
                    "backgroundColor":"#ff4d88"
                },
                "value": "12.5"
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
</html>
    )rawliteral";
}