--TEST--
uniord() function
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
unicode.runtime_encoding=UTF-8
unicode.script_encoding=UTF-8
--FILE--
<?php
if (version_compare(phpversion(), '6.0.0-dev', 'ge')) {
    $s = "\uF8FF";
} else {
    $s = "\xEF\xA3\xBF";
}
$o = uniord($s);

if ($o == 0xF8FF) {
    echo "OK\n";
} else {
    var_dump($o);
}
?>
--EXPECT--
OK
