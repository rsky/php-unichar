--TEST--
unistr_validate() function
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
$r1 = unistr_validate($s, $l);

if (version_compare(phpversion(), '6.0.0-dev', 'ge')) {
    $r2 = false;
} else {
    $r2 = unistr_validate("\xFF\xFF");
}

if ($r1 === true && $l == 6 && $r2 === false) {
    echo "OK\n";
} else {
    var_dump($r1, $l, $r2);
}
?>
--EXPECT--
OK
