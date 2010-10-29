<?php
if (!extension_loaded('unichar')) {
    die('skip unichar extension is not loaded');
}
if (!unichar_icu_version()) {
    die('skip ICU support is not enabled');
}
