<!DOCTYPE html>
<html>
<head>
    <title>Grafy z databáze</title>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/Chart.js/3.7.0/chart.min.js"></script>
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

        #top {
            text-align: center;
            margin: 20px 0;
        }

        canvas {
            margin-bottom: 20px;
        }

        form {
            text-align: center;
        }

        label {
            margin-right: 10px;
        }

        #toggleMode {
            cursor: pointer;
            padding: 10px;
            background-color: #000000;
            color: #fff;
            border: none;
            border-radius: 5px;
        }
    </style>
</head>
<body>
    <div id="container">
        <div id="top">
            <h1>Grafy z databáze</h1>
        </div>
        
        <form method="post">
            <label for="start">Počátek:</label>
            <input type="datetime-local" id="start" name="start" required>

            <label for="end">Konec:</label>
            <input type="datetime-local" id="end" name="end" required>

            <button type="submit">Zobrazit data</button>
        </form>
        <button id="toggleMode" onclick="toggleMode()">?</button>
        <canvas id="grafRychlost"></canvas>
        <br>
        <canvas id="grafTeplota"></canvas>
        <br>
        <canvas id="grafOtacky"></canvas>
        <br>
        <canvas id="grafSpotreba"></canvas>
        <br>
        <canvas id="grafNapeti"></canvas>

        <?php
        $servername = 'kvalitni.info'; 
        $username = 'TEST_SUBJECT2'; 
        $password = 'Dn2gdPyW8!K'; 
        $dbname = 'DB_TEST'; 

        $conn = new mysqli($servername, $username, $password, $dbname);

        if ($conn->connect_error) {
            die("Připojení k databázi selhalo: " . $conn->connect_error);
        }
        if ($_SERVER["REQUEST_METHOD"] == "POST") {
            $start = $_POST["start"];
            $end = $_POST["end"];
            $query = "SELECT TIME, TEMP, SPEED, SPEED2, RPMS, FUEL_RATE, VOLTAGE FROM DATA WHERE TIME BETWEEN '$start' AND '$end'";
        } else {
            $query = "SELECT TIME, TEMP, SPEED, SPEED2, RPMS, FUEL_RATE, VOLTAGE FROM DATA ORDER BY TIME ASC LIMIT 25";
        }

        $result = $conn->query($query);

        $TIME = [];
        $TEMP = [];
        $SPEED = [];
        $SPEED2 = [];
        $RPMS = [];
        $VOLTAGE = [];
        $FUEL_RATE = [];
       

        while ($row = $result->fetch_assoc()) {
            $TIME[] = $row['TIME'];
            $TEMP[] = $row['TEMP'];
            $SPEED[] = $row['SPEED'];
            $SPEED2[] = $row['SPEED2'];
            $RPMS[] = $row['RPMS'];
            $VOLTAGE[] = $row['VOLTAGE'];
            $FUEL_RATE[] = $row['FUEL_RATE'];
            
        }

        $conn->close();
        ?>
        <script>
            function toggleMode() {
                document.body.classList.toggle('dark-mode');
            }
            var casData = <?php echo json_encode($TIME); ?>;
            var teplotaData = <?php echo json_encode($TEMP); ?>;
            var rychlostData = <?php echo json_encode($SPEED); ?>;
            var rychlost2Data = <?php echo json_encode($SPEED2); ?>;
            var otackyData = <?php echo json_encode($RPMS); ?>;
            var napetiData = <?php echo json_encode($VOLTAGE); ?>;
           // var spotrebaData = <?php echo json_encode($FUEL_RATE); ?>;
            
            var ctxTeplota = document.getElementById('grafTeplota').getContext('2d');
            var ctxRychlost = document.getElementById('grafRychlost').getContext('2d');
            var ctxOtacky = document.getElementById('grafOtacky').getContext('2d');
            var ctxNapeti = document.getElementById('grafNapeti').getContext('2d');
            //var ctxSpotreba = document.getElementById('grafSpotreba').getContext('2d');
            

            var grafTeplota = new Chart(ctxTeplota, {
                type: 'line',
                data: {
                    labels: casData,
                    datasets: [{
                        label: 'Teplota chladící kapaliny',
                        data: teplotaData,
                        borderColor: 'red',
                        borderWidth: 2,
                        fill: true
                    }]
                }
            });

            var grafRychlost = new Chart(ctxRychlost, {
                type: 'line',
                data: {
                    labels: casData,
                    datasets: [{
                        label: 'Reálná rychlost',
                        data: rychlostData,
                        borderColor: 'green',
                        borderWidth: 2,
                        fill: false
                    },{
                        label: 'Rychlost na budíku',
                        data: rychlost2Data,
                        borderColor: 'purple',
                        borderWidth: 2,
                        fill: false
                    }]
                }
            });

            var grafOtacky = new Chart(ctxOtacky, {
                type: 'line',
                data: {
                    labels: casData,
                    datasets: [{
                        label: 'Otáčky',
                        data: otackyData,
                        borderColor: 'blue',
                        borderWidth: 2,
                        fill: false
                    }]
                }
            });

            var grafNapeti = new Chart(ctxNapeti, {
                type: 'line',
                data: {
                    labels: casData,
                    datasets: [{
                        label: 'Napětí baterie',
                        data: napetiData,
                        borderColor: 'gold',
                        borderWidth: 2,
                        fill: true
                    }]
                }
            });
            
        /*  var grafSpotreba = new Chart(ctxSpotreba, {
                type: 'line',
                data: {
                    labels: casData,
                    datasets: [{
                        label: 'Spotřeba',
                        data: spotrebaData,
                        borderColor: 'black',
                        borderWidth: 2,
                        fill: false
                    }]
                }
            });*/
        </script>
    </div>
</body>
</html>
