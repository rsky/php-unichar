--TEST--
unichr() function
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
unicode.runtime_encoding=UTF-8
unicode.script_encoding=UTF-8
--FILE--
<?php
$c = unichr(0xF8FF);

if (version_compare(phpversion(), '6.0.0-dev', 'ge')) {
    $s = "\uF8FF";
} else {
    $s = "\xEF\xA3\xBF";
}

if ($c == $s) {
    echo "OK\n";
} else {
    var_dump($c);
}
?>
--EXPECT--
OK
