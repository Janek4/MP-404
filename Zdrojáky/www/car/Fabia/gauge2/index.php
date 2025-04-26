<?php
$servername = 'kvalitni.info'; 
$username = 'WEB'; 
$password = ''; 
$dbname = 'MP_404'; 

// Připojení k databázi
$conn = new mysqli($servername, $username, $password, $dbname);

// Kontrola připojení
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// SQL dotaz
$query = "SELECT ID, TIME, SPEED, RPMS, VOLT, TEMP FROM DATA ORDER BY TIME DESC LIMIT 1";

// Získání výsledků dotazu
$result = $conn->query($query);

// Zpracování výsledků
$data = array();
if ($result->num_rows > 0) {
    $data = $result->fetch_assoc();
}

// Uzavření spojení s databází
$conn->close();

sleep(1);
?>
<!-- Styles -->
<style>
    body {
        background-color: #000000;
        color: #333;
    }

    .container {
        text-align: center;
        padding: 20px;
    }

    button {
        padding: 10px 20px;
        font-size: 16px;
        cursor: pointer;
    }

    #chartdiv {
        width: 100%;
        height: 100%;
    }
</style>

<!-- Resources -->
<script src="https://cdn.amcharts.com/lib/4/core.js"></script>
<script src="https://cdn.amcharts.com/lib/4/charts.js"></script>
<script src="https://cdn.amcharts.com/lib/4/themes/animated.js"></script>

<!-- Chart code -->
<script>
    am4core.ready(function() {
        // create chart
        var chart = am4core.create("chartdiv", am4charts.GaugeChart);
        chart.hiddenState.properties.opacity = 0; // this makes initial fade in effect
        chart.startAngle = 0;
        chart.endAngle = 360;

        function createAxis(min, max, start, end, color) {
            var axis = chart.xAxes.push(new am4charts.ValueAxis());
            axis.min = min;
            axis.max = max;

            axis.strictMinMax = true;
            axis.renderer.useChartAngles = false;
            axis.renderer.startAngle = start;
            axis.renderer.endAngle = end;
            axis.renderer.minGridDistance = 100;

            axis.renderer.line.strokeOpacity = 1;
            axis.renderer.line.strokeWidth = 10;
            axis.renderer.line.stroke = am4core.color(color);

            axis.renderer.ticks.template.disabled = false;
            axis.renderer.ticks.template.stroke = am4core.color(color);
            axis.renderer.ticks.template.strokeOpacity = 1;
            axis.renderer.grid.template.disabled = true;
            axis.renderer.ticks.template.length = 10;
            axis.renderer.labels.template.fill = am4core.color('#828282');
            return axis;
        }

        function createHand(axis, value) {
            var hand = chart.hands.push(new am4charts.ClockHand());
            hand.fill = axis.renderer.line.stroke;
            hand.stroke = axis.renderer.line.stroke;
            hand.axis = axis;
            hand.pin.disabled = true;
            hand.startWidth = 10;
            hand.endWidth = 0;
            hand.radius = am4core.percent(90);
            hand.innerRadius = am4core.percent(70);
            hand.value = value;
            return hand;
        }

        var axis1 = createAxis(0, 200, -85, -5, "#0092ff");     //rychlost
        var axis2 = createAxis(-20, 100, 5, 85, "#ffb300");     //teplota
        var axis3 = createAxis(0, 16, 95, 175, "#20f06c");      //napětí
        var axis4 = createAxis(0, 5500, 185, 265, "#A020F0");   //otáčky

        var hand1 = createHand(axis1, <?php echo $data['SPEED']; ?>);
        var hand2 = createHand(axis2, <?php echo $data['TEMP']; ?>);
        var hand3 = createHand(axis3, <?php echo $data['VOLT']; ?>);
        var hand4 = createHand(axis4, <?php echo $data['RPMS']; ?>);
    });

    // Funkce pro aktualizaci dat grafu
    function updateChart() {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
            if (this.readyState == 4 && this.status == 200) {
                var newData = JSON.parse(this.responseText);
                hand1.value = newData.SPEED;
                hand2.value = newData.TEMP;
                hand3.value = newData.VOLT;
                hand4.value = newData.RPMS;
            }
        };
        xhttp.open("GET", "zkds.cz/mp-404/gauge2/update_data.php", true); // Změňte update_data.php na skript, který vrátí aktualizovaná data
        xhttp.send();
    }

    // Spustí aktualizaci dat grafu každých 5 sekund
    setInterval(updateChart, 1000); 
</script>

<!-- HTML -->
 <h2><?php echo $data['ID']; ?> | <?php echo $data['TIME']; ?></h2>
<div class="container">
    <div id="chartdiv"></div>
</div>

<button onclick="changeColor()">Change Color</button>
