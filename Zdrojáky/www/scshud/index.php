<?php
$servername = 'kvalitni.info'; 
$username = 'TEST_SUBJECT2'; 
$password = 'Dn2gdPyW8!K'; 
$dbname = 'DB_TEST'; 

// Připojení k databázi
$conn = new mysqli($servername, $username, $password, $dbname);
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}
$query = "SELECT SPEED, SPEED2, RPMS, VOLTAGE, TEMP, FUEL_RATE FROM DATA ORDER BY TIME DESC LIMIT 1";
$result = $conn->query($query);
$data = array();
if ($result->num_rows > 0) {
    $data = $result->fetch_assoc();
}

$conn->close();
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>HUD</title>
    <style>
        body {
            font-family: "Arial", sans-serif;
            background-color: #000;
            color: #0f0;
            margin: 0;
            padding: 0;
        }

        .container {
            margin-top: 5%;
            display: grid;
            grid-template-columns: repeat(2, 1fr);
            grid-gap: 5%;
            justify-content: center;
            align-items: center;
            height: 88vh;
        }

        .block {
            height: 80%;
            width: 80%;
            text-align: center;
            padding: 5%;
            border: 10px solid #0f0;
            border-radius: 25px;
        }

        .value {
            font-size: 15vw; /* 10% šířka viewportu */
            font-weight: bold;
        }

        .label {
            color: #FF0000;
            font-size: 7.5vw;
            margin-top: 10px;
        }

    </style>
</head>
<body>
    <div class="container">
        <div class="block">
            <div class="value"><?php echo isset($data['SPEED']) ? $data['SPEED'] : '-'; ?></div>
            <div class="label">Km/h</div>
        </div>
        <div class="block">
            <div class="value"><?php echo isset($data['SPEED2']) ? $data['SPEED2'] : '-'; ?></div>
            <div class="label">Km/h</div>
        </div>
        <div class="block">
            <div class="value"><?php echo isset($data['RPMS']) ? $data['RPMS'] : '-'; ?></div>
            <div class="label">RPMS</div>
        </div>
        <div class="block">
            <div class="value"><?php echo isset($data['VOLTAGE']) ? $data['VOLTAGE'] : '-'; ?></div>
            <div class="label">V</div>
        </div>
        <div class="block">
            <div class="value"><?php echo isset($data['TEMP']) ? $data['TEMP'] : '-'; ?></div>
            <div class="label">°C</div>
        </div>
        <div class="block">
            <div class="value"><?php echo isset($data['FUEL_RATE']) ? $data['FUEL_RATE'] : '-'; ?></div>
            <div class="label">L/h</div>
        </div>
        <script>
        function refreshPage() {
            location.reload();
        }
        setInterval(refreshPage, 950);
    </script>
    </div>
</body>
</html>