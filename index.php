

<html>
    <body>
        <form action=<?= htmlentities($_SERVER['PHP_SELF'])?> method="post">
            <input type="submit" value="FORWARD" name="FORWARD">
            <input type="submit" value="MAZE" name="MAZE">
            <input type="submit" value="LINE" name="LINE">
            <input type="submit" value="RACE" name="RACE">
            <input type="submit" value="STOP" name="STOP">
        </form>
    </body>
</html>
<?php
    $ipAddress = "192.168.137.251";
    if($_SERVER['REQUEST_METHOD'] == "POST"){
        foreach($_POST as $postRequest){
            if(isset($postRequest)){
                $url = curl_init("http://" . $ipAddress . "/" . $postRequest);
                curl_exec($url);
                curl_close($url);
            }
        }
    }

?>