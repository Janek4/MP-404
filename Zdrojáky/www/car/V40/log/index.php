<?php
 $servername = 'kvalitni.info'; 
$username = 'V40_WEB'; 
$password = ''; 
$dbname = 'MP_404_V40';

// Připojení k databázi
$conn = new mysqli($servername, $username, $password, $dbname);

// Kontrola připojení
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Dotaz pro získání dat
$sql = "SELECT TIME, ID, TEMP, SPEED, SPEED2, RPMS, VOLT, FUEL FROM DATA ORDER BY TIME DESC LIMIT 5000";
$result = $conn->query($sql);

// Zpracování výsledků
$data = array();
if ($result->num_rows > 0) {
    while($row = $result->fetch_assoc()) {
        $data[] = $row;
    }
}

// Uzavření spojení s databází
$conn->close();
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>vypiss</title>
    <style>
        body {
            font-family: "Courier New", Courier, monospace;
            background-color: #000000;
            color: #0f0;
            margin: 0;
            padding: 0;
        }

        #container {
            padding: 20px;
            text-align: center;
        }

        table {
            border-collapse: collapse;
            width: 100%;
            font-size: 20px;
        }

        th, td {
            padding: 10px;
            text-align: center;
            border-bottom: 1px solid #0f0;
        }
    </style>
</head>
<body>
    <div id="container">
        <h1>vypis dat</h1>
        <table>
            <tr>
                <th>ID</th>
                <th>Time</th>
                <th>Speed</th>
                <th>Speed2</th>
                <th>RPMS</th>
                <th>Temperature</th>
                <th>Voltage</th>
                <th>Fuel rate</th>
            </tr>
            <?php foreach ($data as $row): ?>
            <tr>
                <td><?php echo $row['ID']; ?></td>
                <td><?php echo $row['TIME']; ?></td>
                <td><?php echo $row['SPEED']; ?></td>
                <td><?php echo $row['SPEED2']; ?></td>
                <td><?php echo $row['RPMS']; ?></td>
                <td><?php echo $row['TEMP']; ?></td>
                <td><?php echo $row['VOLT']; ?></td>
                <td><?php echo $row['FUEL']; ?></td>
            </tr>
            <?php endforeach; ?>
        </table>
    </div>
</body>
</html>
