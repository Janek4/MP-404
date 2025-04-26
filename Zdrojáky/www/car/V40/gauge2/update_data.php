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

// SQL dotaz
$query = "SELECT SPEED, RPMS, VOLT, TEMP FROM DATA ORDER BY TIME DESC LIMIT 1";

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
<?php echo $data['SPEED']; ?> <?php echo $data['RPMS']; ?> <?php echo $data['VOLT']; ?> <?php echo $data['TEMP']; ?>