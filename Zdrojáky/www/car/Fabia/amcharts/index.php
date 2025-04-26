<!DOCTYPE html>
<html>
<head>
    <title>amCharts 4 PHP Example</title>
    <script src="https://cdn.amcharts.com/lib/4/core.js"></script>
    <script src="https://cdn.amcharts.com/lib/4/charts.js"></script>
    <script src="https://cdn.amcharts.com/lib/4/themes/animated.js"></script>
    <style>
        body {
            background-color: #f0f0f0;
            font-family: Arial, sans-serif;
            transition: background-color 0.5s;
        }

        .dark-mode {
            background-color: #121212;
            color: #fff;
        }

        #container {
            width: 90%;
            margin: 0 auto;
        }

        #top,h2 {
            text-align: center;
            margin: 20px 0;
        }

        canvas {
            margin-bottom: 20px;
        }

        form {
            text-align: center;
        }

        #toggleMode {
            cursor: pointer;
            padding: 10px;
            background-color: #000000;
            color: #fff;
            border: none;
            border-radius: 5px;
        }
        .chart {
            width: 100%;
            height: 400px;
            margin-bottom: 20px;
        }
        #pur {
            color: #A020F0;
        }
        #blu {
            color: #0092ff;
        }
    </style>
</head>
<body>
<div id="container">
        <div id="top">
            <h1>Grafy z databáze Škoda Fabia 1</h1>
        </div>
        <form method="post">
            <label for="start">Počátek:</label>
            <input type="datetime-local" id="start" name="start" required>

            <label for="end">Konec:</label>
            <input type="datetime-local" id="end" name="end" required>

            <button type="submit">Zobrazit data</button>
        </form>
        <button id="toggleMode" onclick="toggleMode()">?</button>
        <h2>Rychlost <span id="pur">na tachometru</span> a <span id="blu">z OBD-II</span></h2>
        <div class="chart" id="chart_speed"></div>
        <h2>Otáčky</h2>
        <div class="chart" id="chart_rpms"></div>
        <h2>Teplota</h2>
        <div class="chart" id="chart_temp"></div>
        <h2>Napětí</h2>
        <div class="chart" id="chart_volt"></div>
</div>
    <?php
        // Připojení k databázi
        $servername = 'kvalitni.info'; 
        $username = 'WEB'; 
        $password = ''; 
        $dbname = 'MP_404';  
        $conn = new mysqli($servername, $username, $password, $dbname);


        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        }

        if ($_SERVER["REQUEST_METHOD"] == "POST") {
            $start = $_POST["start"];
            $end = $_POST["end"];

            $sql = "SELECT TIME, TEMP, SPEED, SPEED2, RPMS, VOLT FROM DATA WHERE TIME BETWEEN '$start' AND '$end'";
        } else {
            $sql = "SELECT TIME, TEMP, SPEED, SPEED2, RPMS, VOLT FROM DATA ORDER BY TIME ASC LIMIT 25";
        }
        $result = $conn->query($sql);

        $data = array();
        if ($result && $result->num_rows > 0) {
            while ($row = $result->fetch_assoc()) {
                $data[] = $row;
            }
        } else {
            echo "Není k dispozici žádná data.";
        }

        $conn->close();
    ?>

    <script>
        function toggleMode() {
                document.body.classList.toggle('dark-mode');
        }
        // Vložení dat do formátu požadovaného amCharts
        var chartData = <?php echo json_encode($data); ?>;
        // Vytvoření instancí grafů
        createChart("chart_speed", ["SPEED", "SPEED2"], ["Rychlost", "Rychlost2"]);
        createChart("chart_rpms", ["RPMS"], ["RPMS"]);
        createChart("chart_temp", ["TEMP"], ["Temperature"]);
        createChart("chart_volt", ["VOLT"], ["Voltage"]);

        // Funkce pro vytvoření grafu
        function createChart(container, dataFields, titles) {
            var chart = am4core.create(container, am4charts.XYChart);

            // Přidání dat do grafu
            chart.data = chartData;

            // Definice osy X
            var categoryAxis = chart.xAxes.push(new am4charts.CategoryAxis());
            categoryAxis.dataFields.category = "TIME";
            categoryAxis.renderer.grid.template.location = 0;
            categoryAxis.renderer.minGridDistance = 30;
            categoryAxis.renderer.labels.template.disabled = true;

            // Definice osy Y
            var valueAxis = chart.yAxes.push(new am4charts.ValueAxis());   
            chart.colors.list = [
                am4core.color("#0092ff"), // zelená
                am4core.color("#A020F0"), // pur
            ];
            // Definice liniového grafu
            for (var i = 0; i < dataFields.length; i++) {
                var series = chart.series.push(new am4charts.LineSeries());
                series.dataFields.valueY = dataFields[i];
                series.dataFields.categoryX = "TIME";
                series.name = titles[i];
                series.strokeWidth = 2;
                series.minBulletDistance = 20;

                // Definice animace
                var hoverState = series.states.create("hover");
                hoverState.properties.strokeWidth = 3;

                // Nastavení bodů
                var bullet = series.bullets.push(new am4charts.CircleBullet());
                bullet.circle.radius = 3;
                bullet.circle.strokeWidth = 2;

                // Nastavení tooltipu
                series.tooltipText = "[bold]{valueY}[/]";
            }

            // Nastavení tématu
            chart.cursor = new am4charts.XYCursor();
            chart.cursor.lineX.strokeOpacity = 0;
            chart.cursor.lineY.strokeOpacity = 0;
            chart.cursor.behavior = "panXY";
            
            
        }
    </script>
</body>
</html>
