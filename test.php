<?php
ini_set('php-tesseract.language', 'chi_sim');
ini_set('php-tesseract.tessdata-dir', '/vagrant/tessdata');
var_dump(tesseract_recognize(__DIR__ . '/test.png'));
