<!DOCTYPE html>
<meta charset="UTF-8">

<! Simple server script to do an online sudoku website >
<! Tunc Kayikcioglu, 2017>

<?php
	if (isset($_POST["clearAll"])) {
		unset($_POST["var0"]);
	}

	if (!isset($_POST["var0"])) {
		$entries = new SplFixedArray(81);
		for ($i=0; $i<81; $i++)
			$entries[$i] = '0';
	}
	else {
		for($i=0; $i<9; $i++) {
			for($j=0; $j<9; $j++) {
				$id = 9*$i + $j;
				$inmatrix = $inmatrix . " " . $_POST["var$id"];
			}
		}
		
		$output = shell_exec("./sudoku.out $inmatrix");
		if ($output[0] == '0') { // No error
			$outmatrix = substr($output, 2, strlen($output)-2);
			$entries = explode(" ", $outmatrix);
		}
		else { // Error occured, report
			$errorMessage = $output;
			for ($i=0; $i<81; $i++)
				$entries[$i] = $_POST["var$i"];
		}
	}
?>


<html>
<head>
	<title>Tunc Sudoku solver</title>
</head>

<body>
	<br/>
	<br/>
	<hl>Enter given entries in your Sudoku and click "Solve" to save time!</hl>
	<br/>
	<br/>
	<br/>

	<form method="post" action="index.php">
		<fieldset>
		<legend>Sudoku matrix</legend>
		
		<?php	
		for($i=0;$i<9;$i++) {
			if ($i%3==0)
				echo "<br> \n";
			for($j=0;$j<9;$j++) {
				$id = 9*$i+$j;
				if ($j%3==0)
					echo '&nbsp ';
				echo '<input type="text" size="1" style="text-align:center" name="var' . $id . '" width="20px" value="' . $entries[$id] . '" required pattern="^[0-9]$" />' . "\n";
			}
			echo '<br>';
		}
		?>
	
		</fieldset>
		<br>
		<br>
		<input type="submit" name="solve" value="Solve" />
		<input type="submit" name="clearAll" value="Clear" />
	</form>


	<br/>
	<br/>
	<br/>


	<?php
	if(isset($errorMessage))
		echo $errorMessage . "\n";
	?>


</body>
</html>

