#!/bin/sh
rm -rf ../default/HTML/tool/*
rsync -a --exclude ".svn" ../default/HTML5/tool/* ../default/HTML/tool/
rsync -a --exclude ".svn" ../default/HTML5/plugins/* ../default/HTML/plugins/
rsync -a --exclude ".svn" ../default/HTML5/keyword/*.csv ../default/HTML/keyword/

rm -rf ../default/JavaScript/tool/*
rsync -a --exclude ".svn" ../default/HTML5/tool/JavaScript/* ../default/JavaScript/tool/
rsync -a --exclude ".svn" ../default/HTML5/plugins/* ../default/JavaScript/plugins/
rsync -a --exclude ".svn" ../default/HTML5/keyword/JavaScript.csv ../default/JavaScript/keyword/
rsync -a --exclude ".svn" ../default/HTML5/keyword/DOM.csv ../default/JavaScript/keyword/
rsync -a --exclude ".svn" ../default/HTML5/keyword/jQuery.csv ../default/JavaScript/keyword/

rm -rf ../default/PHP/tool/*
rsync -a --exclude ".svn" ../default/HTML5/tool/PHP/* ../default/PHP/tool/
rsync -a --exclude ".svn" ../default/HTML5/keyword/PHP.csv ../default/PHP/keyword/

rm -rf ../default/CSS/tool/*
rsync -a --exclude ".svn" ../default/HTML5/tool/CSS/* ../default/CSS/tool/
rsync -a --exclude ".svn" ../default/HTML5/keyword/CSS.csv ../default/CSS/keyword/
