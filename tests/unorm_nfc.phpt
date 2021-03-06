--TEST--
unorm_nfc() function
--SKIPIF--
<?php include('skipif_icu.inc'); ?>
--INI--
unicode.runtime_encoding=UTF-8
unicode.script_encoding=UTF-8
--FILE--
<?php
if (version_compare(phpversion(), '6.0.0-dev', 'ge')) {
    $s = chr(12459) . chr(12441) . chr(12483) . chr(12484);
} else {
    $s = "\xE3\x82\xAB\xE3\x82\x99\xE3\x83\x83\xE3\x83\x84";
}

if (version_compare(phpversion(), '6.0.0-dev', 'ge')) {
    $r = chr(12460) . chr(12483) . chr(12484);
} else {
    $r = "\xE3\x82\xAC\xE3\x83\x83\xE3\x83\x84";
}

$c = unorm_nfc($s);
if ($c == $r) {
    echo "OK\n";
} else {
    var_dump($c);
}
?>
--EXPECT--
OK
