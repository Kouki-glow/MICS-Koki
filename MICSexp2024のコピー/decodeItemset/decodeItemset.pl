#!/usr/bin/env perl
#
# アイテムセットのIDをデコード
#  Usage: ./decodeItemset.pl tabファイル名 アイテムセットファイル名 アイテムセットの位置
#    アイテムセットファイルの属性は 半角スペース または タブ で区切られること
#    アイテムセットIDは "," で区切られること
#
#  Time-stamp: <2021-11-23 11:37:05 shintani>

use strict;
use warnings;

unless( defined $ARGV[2] ){
  die "Usage: ./decodeFI.pl tabfile itemsetfile itemsetfield\n";
}

my $tabfile = $ARGV[0]; shift;
my $fifile = $ARGV[0]; shift;
my $fipos = $ARGV[0]; shift;

# read itemname
my %iname = ();
open(IN,"< $tabfile");
while(<IN>){
  chomp;
  chomp;
  my @row = split(/\t/);
  $iname{$row[0]} = $row[1];
}
close(IN);


# decode itemname for each frequent itemset
open(IN,"< $fifile");
while(<IN>){
  my $line = $_;
  chomp;
  chomp;
  my @row = split(/[\t|\s]/);

  unless( defined $row[$fipos] ){
    die "Error: itemset is not found in field $fipos\n"
  }

  my @ipat = ();
  foreach my $i ( split(/,/,$row[$fipos]) ){
    unless( exists $iname{$i} ){
      die "Error: itemID $i is not found\n";
    }
    push( @ipat, $iname{$i} );
  }
  my $inameset = join(",",@ipat);
  $line =~ s/$row[$fipos]/$inameset/;

  print $line;
}


exit;
