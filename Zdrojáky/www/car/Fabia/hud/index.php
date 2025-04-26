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
$query = "SELECT SPEED, SPEED2, RPMS, VOLT, TEMP, TIME FROM DATA ORDER BY TIME DESC LIMIT 1";

// Získání výsledků dotazu
$result = $conn->query($query);

// Zpracování výsledků
$data = array();
if ($result->num_rows > 0) {
    $data = $result->fetch_assoc();
}

// Uzavření spojení s databází
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
            <div class="value"><?php echo isset($data['VOLT']) ? $data['VOLT'] : '-'; ?></div>
            <div class="label">V</div>
        </div>
        <div class="block">
            <div class="value"><?php echo isset($data['TEMP']) ? $data['TEMP'] : '-'; ?></div>
            <div class="label">°C</div>
        </div>
        <div class="block">
            <div class="value"><!--<?php echo isset($data['TIME']) ? $data['TIME'] : '-'; ?>-->-</div>
            <div class="label">-</div>
        </div>
        <script>
        function refreshPage() {
            location.reload();
        }
        setInterval(refreshPage, 5000);
    </script>
    </div>
</body>
</html>