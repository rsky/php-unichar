--TEST--
uniord_array() function
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
unicode.runtime_encoding=UTF-8
unicode.script_encoding=UTF-8
--FILE--
<?php
if (version_compare(phpversion(), '6.0.0-dev', 'ge')) {
    $s = "\u96F2\u4E39CODE";
} else {
    $s = "\xE9\x9B\xB2\xE4\xB8\xB9CODE";
}

$e = array(0x96F2, 0x4E39, ord('C'), ord('O'), ord('D'), ord('E'));
$r = uniord_array($s);

if ($e == $r) {
    echo "OK\n";
} else {
    var_dump($r);
}
?>
--EXPECT--
OK
