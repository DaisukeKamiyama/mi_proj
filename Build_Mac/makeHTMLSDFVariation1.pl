#!/usr/bin/perl

open(IN, "../App/sdf/html.sdf");
open(OUT, "> ../App/sdf/html_variation1.sdf");
while (<IN>) {
	s/GlobalAttribute_SDFName\("HTML"\)/GlobalAttribute_SDFName\("HTML \(indent enabled inside <pre>\)"\)/;
	s/\s*(.+#variation0)/#$1/;
	s/\s*#(.+#variation1)/$1/;
    print OUT "$_";
}
close(IN);
close(OUT);

