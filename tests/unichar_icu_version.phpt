--TEST--
unichar_icu_version() function
--SKIPIF--
<?php include('skipif_icu.inc'); ?>
--FILE--
<?php
var_dump(unichar_icu_version());
?>
--EXPECTREGEX--
(string|unicode)\([1-9]\) \"[1-9]\.[0-9](\.[0-9]+)*\"
