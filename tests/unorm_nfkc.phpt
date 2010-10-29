--TEST--
unorm_nfkc() function
--SKIPIF--
<?php include('skipif_icu.inc'); ?>
--INI--
unicode.runtime_encoding=UTF-8
unicode.script_encoding=UTF-8
--FILE--
<?php
if (version_compare(phpversion(), '6.0.0-dev', 'ge')) {
    $s = chr(12849);
} else {
    $s = "\xE3\x88\xB1";
}

if (version_compare(phpversion(), '6.0.0-dev', 'ge')) {
    $r = '(' . chr(26666) . ')';
} else {
    $r = "(\xE6\xA0\xAA)";
}

$c = unorm_nfkc($s);
if ($c == $r) {
    echo "OK\n";
} else {
    var_dump($c);
}
?>
--EXPECT--
OK
