#!/bin/sh
mkdir -p ../help/ja/sdfsample
mkdir -p ../help/ja/colorschemesample
mkdir -p ../help/ja/csvsample
mkdir -p ../help/ja/javascriptsample
cp ../App/sdf/* ../help/ja/sdfsample/
cp ../App/data/ColorScheme/* ../help/ja/colorschemesample
cp ../default/HTML5/keyword/* ../help/ja/csvsample
cp ../default/HTML5/plugins/syntaxchecker_jshint.js ../help/ja/javascriptsample
iconv -f utf8 -t cp932 ../help/ja/javascriptsample/syntaxchecker_jshint.js > ../help/ja/javascriptsample/syntaxchecker_jshint_sjis.js
