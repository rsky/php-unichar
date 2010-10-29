--TEST--
unistr_decode_entity() function
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
unicode.runtime_encoding=UTF-8
unicode.script_encoding=UTF-8
--FILE--
<?php
$s1 = 'Arts &amp; Architects';
$e1 = 'Arts & Architects';

$s2 = '&#xF8FF; Apple&trade;';
if (version_compare(phpversion(), '6.0.0-dev', 'ge')) {
    $e2 = "\uF8FF Apple\u2122";
} else {
    $e2 = "\xEF\xA3\xBF Apple\xE2\x84\xA2";
}

$s3 = '&gt;&gt';
$e3 = '>>';

$s4 = $s3;
$e4 = false;

$s5 = '[&nbsp;&#10;]';
$e5 = '[  ]';

$s6 = $s5;
if (version_compare(phpversion(), '6.0.0-dev', 'ge')) {
    $e6 = "[\u00A0&#10;]";
} else {
    $e6 = "[\xC2\xA0&#10;]";
}

$s7 = $s5;
if (version_compare(phpversion(), '6.0.0-dev', 'ge')) {
    $e7 = "[\u00A0]";
} else {
    $e7 = "[\xC2\xA0]";
}

$s8 = '(&#0;&#x00;)';
$e8 = "(\0\0)";

$s9 = $s8;
$e9 = '(&#0;&#x00;)';

$s10 = $s8;
$e10 = '()';

$s11 = $s8;
$e11 = false;

$s12 = '&#123;&#1234567890;&#125;';
$e12 = '{&#1234567890;}';

$r1 = unistr_decode_entity($s1);
$r2 = unistr_decode_entity($s2);
$r3 = unistr_decode_entity($s3);
$r4 = @unistr_decode_entity($s4, UNICHAR_ENT_STRICT);
$r5 = unistr_decode_entity($s5, UNICHAR_ENT_CTRL_TO_SPACE | UNICHAR_ENT_NBSP_TO_SPACE);
$r6 = unistr_decode_entity($s6, UNICHAR_ENT_IGNORE_CTRL);
$r7 = unistr_decode_entity($s6, UNICHAR_ENT_REMOVE_CTRL);
$r8 = unistr_decode_entity($s8, UNICHAR_ENT_ALLOW_CTRL);
$r9 = unistr_decode_entity($s9, UNICHAR_ENT_IGNORE_NUL);
$r10 = unistr_decode_entity($s10, UNICHAR_ENT_REMOVE_NUL);
$r11 = @unistr_decode_entity($s11, UNICHAR_ENT_WARN_NUL);
$r12 = unistr_decode_entity($s12);

for ($i = 1; $i <= 12; $i++) {
    $r= "r$i";
    $e= "e$i";
    if ($$r != $$e) {
        echo "$i:\n";
        var_dump($$r, $$e);
    }
}
echo "OK\n";
?>
--EXPECT--
OK
