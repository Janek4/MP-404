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
$sql = "SELECT TIME, ID, TEMP, SPEED, SPEED2, RPMS, FUEL_RATE, VOLTAGE FROM DATA ORDER BY TIME DESC";
$result = $conn->query($sql);
$data = array();
if ($result->num_rows > 0) {
    while($row = $result->fetch_assoc()) {
        $data[] = $row;
    }
}

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
        <h1>vypiss dat</h1>
        <table>
            <tr>
                <th>ID</th>
                <th>Time</th>
                <th>Speed</th>
                <th>Speed2</th>
                <th>RPMS</th>
                <th>Temperature</th>
                <th>Voltage</th>
                <th>Fuel Rate</th>  
            </tr>
            <?php foreach ($data as $row): ?>
            <tr>
                <td><?php echo $row['ID']; ?></td>
                <td><?php echo $row['TIME']; ?></td>
                <td><?php echo $row['SPEED']; ?></td>
                <td><?php echo $row['SPEED2']; ?></td>
                <td><?php echo $row['RPMS']; ?></td>
                <td><?php echo $row['TEMP']; ?></td>
                <td><?php echo $row['VOLTAGE']; ?></td>
                <td><?php echo $row['FUEL_RATE']; ?></td>
            </tr>
            <?php endforeach; ?>
        </table>
    </div>
</body>
</html>
