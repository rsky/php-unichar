--TEST--
unistr_rotate() function
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

if (version_compare(phpversion(), '6.0.0-dev', 'ge')) {
    $r = "\u9712\u4E59code";
} else {
    $r = "\xE9\x9C\x92\xE4\xB9\x99code";
}

$r1 = unistr_rotate($s, 32);
$r2 = unistr_rotate('IBM', -1);

if ($r1 == $r && $r2 == 'HAL') {
    echo "OK\n";
} else {
    var_dump($r1, $r2);
}
?>
--EXPECT--
OK
