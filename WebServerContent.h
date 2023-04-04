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
	
	.parameters{
		font-size: 18px;
		padding: 10px
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
    <title>Water Level Tank Bot</title>
    <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css"
        integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
    <script type="text/javascript" src="http://static.fusioncharts.com/code/latest/fusioncharts.js"></script>
	<script>
		function getParameters() 
		{
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
		document.addEventListener("DOMContentLoaded", function() {
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
                        }, 750);
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
            <span class="units">%</span><br>
			
			<div class="parameters">
				<a href="settings"><i class="fas fa-cog" style="color:#8b99b0;"></i></a>
				<span class="labels">Max Tank Depth: </span>
				<span id="maxDepthLabel">???</span>
				<span class="units">cm</span><br>
				
				<a href="settings"><i class="fas fa-cog" style="color:#8b99b0;"></i></a>
				<span class="labels">Min Tank Depth: </span>
				<span id="minDepthLabel">???</span>
				<span class="units">cm</span><br>
				
				<a href="settings"><i class="fas fa-cog" style="color:#8b99b0;"></i></a>
				<span class="labels">Trigger Alarm At: </span>
				<span id="alarmTriggerLabel">???</span>
				<span class="units">%</span><br>
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
	<title>Tank settings</title>
	<style>
	
	body {
        height: 100%;
        background-color: #f8f4f4;
		margin: auto;
        padding: auto;
        position: relative;
        text-align: center;
        vertical-align: middle;
    }
	
	.title 
	{
        font-size: 48px;
    }
	
	.settingsForm 
	{
        font-size: 42px;
    }
	</style>
	<script>
		function getParameters() 
		{
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
		document.addEventListener("DOMContentLoaded", function() {
			getParameters();
		});
	</script>
</head>
<body>
	<div class="title">
	<h1>Tank settings</h1>
	</div>
	
	<form id="settings-form" class=settingsForm>
		<label for="max">Max depth:</label> &nbsp;
		<input type="number" name="max" id="maxDepthInput">
		<label>cm</label><br>
		
		<label for="min">Min depth:</label> &nbsp;
		<input type="number" name="min" id="minDepthInput">
		<label>cm</label><br>
		
		<label for="alarmTrigger">Trigger alarm at:</label> &nbsp;
		<input type="number" name="alarmTrigger" id="alarmTriggerInput">
		<label>% of water in tank</label><br>
		
		<input type="submit" value="Save">
		<input type="submit" value="Cancel" id="cancelBtn">
	</form>
	
	<script>
		const form = document.getElementById('settings-form');
		form.addEventListener('submit', function(event) {
			event.preventDefault();
			const data = {status:"ok",
                          maxDepth: form.max.value, 
						  minDepth: form.min.value,
						  alarmTrigger: form.alarmTrigger.value};
			fetch('/setParameters', {
				method: 'POST',
				headers: {
					'Content-Type': 'application/json'
				},
				body: JSON.stringify(data)
			})
			.then(response => {
                console.log(response); // log the entire response object
                return response.json(); // parse the response body as JSON
            })
			.then(data => {
				console.log(data);
				window.location.href = "/";
			})
			.catch(error => console.error(error));
		});
	</script>
	
	<script type="text/javascript">
    document.getElementById("cancelBtn").onclick = function () {
        window.location.href = "/";
    };
</script>
</body>
</html>
)rawliteral";
}