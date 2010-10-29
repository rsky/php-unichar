--TEST--
unistr_ngram() function
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
unicode.fallback_encoding=UTF-8
unicode.output_encoding=UTF-8
unicode.runtime_encoding=UTF-8
unicode.script_encoding=UTF-8
--FILE--
<?php
if (version_compare(phpversion(), '6.0.0-dev', 'ge')) {
    $s = "\u96F2\u4E39CODE";
} else {
    $s = "\xE9\x9B\xB2\xE4\xB8\xB9CODE";
}
foreach (unistr_ngram($s, 2) as $n) {
    var_dump($n);
}
?>
--EXPECTREGEX--
(string\(6\)|unicode\(2\)) ".+?"
(string\(4\)|unicode\(2\)) ".+?C"
(string\(2\)|unicode\(2\)) "CO"
(string\(2\)|unicode\(2\)) "OD"
(string\(2\)|unicode\(2\)) "DE"
