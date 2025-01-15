#!/usr/bin/env perl
#
# アイテムセットから相関ルールを導出し、アイテムIDをデコード  変換する行の先頭にタグ付
#  Usage: ./deriveRuleRow.pl 最小確信度 tabファイル名 アイテムセットファイル名 アイテムセットの位置 支持度(頻度)の位置 アイテムセット行のタグ
#    アイテムセットファイルの属性は 半角スペース または タブ で区切られること
#    アイテムセットIDは "," で区切られること
#
#  Time-stamp: <2022-12-19 14:38:05 shintani>

use strict;
use warnings;

unless( defined $ARGV[5] ){
  die "Usage: ./deriveRuleRow.pl minconf tabfile itemsetfile itemsetfield supportfield itemsetrowtag\n";
}

my $minconf = $ARGV[0]; shift;
my $tabfile = $ARGV[0]; shift;
my $fifile = $ARGV[0]; shift;
my $fipos = $ARGV[0]; shift;
my $suppos = $ARGV[0]; shift;
my $firow = $ARGV[0]; shift;

# アイテムIDとアイテム名の対応を取得
my %iname = ();
open(IN,"< $tabfile");
while(<IN>){
  chomp;
  chomp;
  my @row = split(/\t/);
  $iname{$row[0]} = $row[1];
}
close(IN);


# すべての頻出アイテムセットを取得
my %fpat = ();
open(IN,"< $fifile");
while(<IN>){
  unless( /^${firow}/ ){
    next;
  }
  my $line = $_;
  chomp;
  chomp;
  my @row = split(/[\t|\s]/);

  unless( defined $row[$fipos] ){
    die "Error: itemset is not found in field $fipos\n"
  }
  unless( defined $row[$suppos] ){
    die "Error: support is not found in field $suppos\n"
  }

  $fpat{$row[$fipos]} = $row[$suppos];
}
close(IN);

# 頻出アイテムセットから相関ルールを導出
my %ar = ();
foreach my $p ( keys %fpat ){
  my @ci = split(/,/,$p);

  if( $#ci == 1 ){
    # 長さ2のアイテムセット
    for( my $i = 0; $i <= $#ci; $i++ ){
      my $assump = $ci[$i];
      my $conq = "";
      for( my $j = 0; $j <= $#ci; $j++ ){
	if( $j != $i ){
	  $conq = $ci[$j];
	}}
      my $conf = $fpat{$p}*1.0 / $fpat{$assump};
      if( $conf >= $minconf ){
	$ar{$assump}{$conq}[0] = $conf;
	$ar{$assump}{$conq}[1] = $fpat{$p};
      }}}
  elsif( $#ci == 2 ){
    # 長さ3のアイテムセット
    for( my $i = 0; $i <= $#ci; $i++ ){
      my $assump = "";
      my $conq = "";

      $assump = $ci[$i];

      my @tmp = ();
      for( my $j = 0; $j <= $#ci; $j++ ){
	if( $j != $i ){
	  push( @tmp, $ci[$j] );
	}}
      $conq = join(",",@tmp);

      # 2 => 1
      my $conf = $fpat{$p}*1.0 / $fpat{$assump};
      if( $conf >= $minconf ){
	$ar{$assump}{$conq}[0] = $conf;
	$ar{$assump}{$conq}[1] = $fpat{$p};
      }
      # 1 => 2
      $conf = $fpat{$p}*1.0 / $fpat{$conq};
      if( $conf >= $minconf ){
	$ar{$conq}{$assump}[0] = $conf;
	$ar{$conq}{$assump}[1] = $fpat{$p};
      }}}
}


# output selected association rules
foreach my $aa ( sort keys %ar ){
  foreach my $cc ( sort keys %{$ar{$aa}} ){
    my @ipat = ();
    foreach my $i ( split(/,/,$aa) ){
      unless( exists $iname{$i} ){
	die "Error: itemID $i is not found\n";
      }
      push( @ipat, $iname{$i} );
    }
    my $assumpset = join(",", @ipat);

    @ipat = ();
    foreach my $i ( split(/,/,$cc) ){
      unless( exists $iname{$i} ){
	die "Error: itemID $i is not found\n";
      }
      push( @ipat, $iname{$i} );
    }
    my $conqset = join(",", @ipat);

    print "$assumpset\t=>\t$conqset\t";
    if( $ar{$aa}{$cc}[1] > 1 ){
      printf("%d\t%.02f\n", $ar{$aa}{$cc}[1], $ar{$aa}{$cc}[0]);
    } else {
      printf("%.02f\t%.02f\n", $ar{$aa}{$cc}[1], $ar{$aa}{$cc}[0]);
    }}
}

exit;
