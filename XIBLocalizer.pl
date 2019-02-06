#!/usr/bin/perl

use strict;
use warnings;
use utf8;
binmode STDIN, ':encoding(utf8)';
binmode STDOUT, ':encoding(utf8)';
binmode STDERR, ':encoding(utf8)';

if( @ARGV == 3 )
{
	print STDERR "[XIBLocalizer]\n";
	print STDERR "Source XIB File: ".$ARGV[0]."\n";
	print STDERR "Translation File: ".$ARGV[1]."\n";
	print STDERR "Destination XIB File: ".$ARGV[2]."\n";
	
	# srcファイル
	open(SRCFILE,"<:utf8",$ARGV[0]);
	my @srcText = <SRCFILE>;
	close(SRCFILE);
	
	# 翻訳辞書ファイル
	open(DICFILE,"<:utf8",$ARGV[1]);
	my @dicTextRaw = <DICFILE>;
	close(DICFILE);
	
	# <, >, &, "文字を変換
	my @dicText = ();
	foreach my $line (@dicTextRaw)
	{
		$line =~ s/&/&amp;/g; # 正規表現置換順番修正 #879 &を先にしないと、&lt;, &gt;, &quot;の&を置換してしまう
		$line =~ s/</&lt;/g;
		$line =~ s/>/&gt;/g;
		$line =~ s/"/&quot;/g;
		push(@dicText,$line);
	}
	
	# 実行チェックフラグ
	my $executeFlag = 0;
	
	# 書き込み先ファイルがまだ存在していないなら常に実行
	if( -f $ARGV[2] )
	{
		$executeFlag = 1;
	}
	
	# 更新日時チェック
	my $srcDateTime = (stat $ARGV[0])[9] ;
	my $dicDateTime = (stat $ARGV[1])[9] ;
	my $dstDateTime = (stat $ARGV[2])[9] ;
	#（srcファイルの変更日時）＞（書き込み先ファイルの変更日時）なら実行
	if( $srcDateTime > $dstDateTime )
	{
		$executeFlag = 1;
	}
	#（翻訳ファイルの変更日時）＞（書き込み先ファイルの変更日時）なら実行
	if( $dicDateTime > $dstDateTime )
	{
		$executeFlag = 1;
	}
	# 実行フラグがoffなら終了
	if( $executeFlag == 0 )
	{
		print STDERR "Not translated because source file and translation file is not changed.\n";
		exit;
	}
	
	# 辞書読み込み
	my @keyArray = ();
	my @valueArray = ();
	my @find1 = ();
	my @replace1 = ();
	my @find2 = ();
	my @replace2 = ();
	my @find3 = ();
	my @replace3 = ();
	my $cnt = 0;
	foreach my $line (@dicText)
	{
		chomp($line);
		if( $cnt%2 == 0 )
		{
			push(@keyArray,$line);
			push(@find1,"title=\"".(quotemeta $line)."\"");
			push(@find2,"<string key=\"title\">".(quotemeta $line)."</string>");
			push(@find3,"content=\"".(quotemeta $line)."\"");
		}
		else
		{
			push(@valueArray,$line);
			push(@replace1,"title=\"".$line."\"");
			push(@replace2,"<string key=\"title\">".$line."</string>");
			push(@replace3,"content=\"".$line."\"");
		}
		$cnt++;
	}
	
	# 置換実行
	open(DSTFILE,">:utf8",$ARGV[2]);
	foreach my $line (@srcText)
	{
		if( $line =~ /title=".+"/ )
		{
			my $i;
			for( $i = 0; $i < $cnt/2; $i++ )
			{
				$line =~ s/$find1[$i]/$replace1[$i]/;
			}
		}
		if( $line =~ /<string key=".+">/ )
		{
			my $i;
			for( $i = 0; $i < $cnt/2; $i++ )
			{
				$line =~ s/$find2[$i]/$replace2[$i]/;
			}
		}
		if( $line =~ /content=".+"/ )
		{
			my $i;
			for( $i = 0; $i < $cnt/2; $i++ )
			{
				$line =~ s/$find3[$i]/$replace3[$i]/;
			}
		}
		print DSTFILE $line;
	}
	close DSTFILE;
	print STDERR "Translation done.\n";
}
else
{
	print STDERR "Argument count error!\n";
}

