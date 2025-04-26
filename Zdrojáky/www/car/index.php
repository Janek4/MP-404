<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Maturitní projekt</title>
    <style>
        body {
            font-family: "Arial", sans-serif;
            color: #ffffff;
            background-color: #000000;
        }
        #top {
            font-size: 2vw;
            text-align: center;
        }
        #main {
            font-size: 1.5vw;
            margin: auto;
            width: 90%;
        }
        a {
            color: #ffffff;
            text-decoration: none;
        } 
        /*li {
            font-size:1vw;
        }*/
    </style>
</head>
<body>
    <div id="container">
        <div id="top">
        <h1><?php echo "ESP32 dashboard rozcestník" ?></h1>
        </div>
        <div id="main">
        <br>
        <br>
        <ul>
            <a href="http://zkds.cz/mp-404/car/Fabia/"><li style="color:gray">Škoda Fabia 1</li></a>
            <a href="http://zkds.cz/mp-404/car/Golf/"><li style="color:white">Volkswagen Golf 3</li></a>
            <a href="http://zkds.cz/mp-404/car/V40/"><li style="color:blue">Volvo V40</li></a>
        </ul>
        </div>
    </div>
</body>
</html>