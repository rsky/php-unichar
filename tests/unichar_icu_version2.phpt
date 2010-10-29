--TEST--
unichar_icu_version() function (without ICU)
--SKIPIF--
<?php
include('skipif.inc');
if (unichar_icu_version()) {
    die('skip ICU support is enabled');
}
?>
--FILE--
<?php
var_dump(unichar_icu_version());
?>
--EXPECT--
bool(false)
