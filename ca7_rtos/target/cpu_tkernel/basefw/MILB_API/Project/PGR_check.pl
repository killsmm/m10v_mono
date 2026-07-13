#! /usr/bin/env perl
##
## PG-Reliefのチェック結果CSVファイルの先頭カラムに誤検出チェック済みマークを挿入する
##


use strict;
use FileHandle;
use Encode qw/encode decode/;
use utf8;

&main;


sub main {
	if( scalar(@ARGV) == 0 ) {
		printf( "Usage %s <csv file ...>\n", $0 );
		exit 64;
	}

	binmode STDOUT, ':encoding(utf8)';
	binmode STDERR, ':encoding(utf8)';

	foreach my $s_file_path ( @ARGV ) {
		my( $s_out_file_path ) = $s_file_path;
		unless( $s_out_file_path =~ s/(\.[cC][sS][vV])$/_tmp$1/ ) {
			$s_out_file_path .= '_tmp.csv';
		}

		unless( defined(&check_csv_main( $s_out_file_path, $s_file_path )) ) {
			# check error.
			unlink( $s_out_file_path );
			next;
		}

		unlink( $s_file_path );
		rename( $s_out_file_path, $s_file_path );
	}
}


sub check_csv_main {
	my( $s_outCsvFilePath, $s_inCsvFilePath ) = @_;

	my( $source_in_dat ) = {	# ソースファイル情報管理
		'fh_in'        => undef,	# FileHandle
		'file_path'    => undef,	# オープン中のファイルパス
		'opened'       => undef,	# オープン中かどうか
		'cur_line_str' => undef,	# 最後に読んだ行の文字列
	};

	my( $fh_csv_in );					# CSV入力FileHandle
	my( $csv_line_str );				# CSV入力の現在行文字列
	my( $csv_dat_array ) = [];			# CSV入力の情報配列

	my( $fh_csv_out );					# CSV出力FileHandle

	$fh_csv_in = new FileHandle;
	unless( $fh_csv_in->open( "< $s_inCsvFilePath" ) ) {
		printf( "File open error: %s\n", $s_inCsvFilePath );
		return undef;
	}

	$fh_csv_out = new FileHandle;
	unless( $fh_csv_out->open( "> $s_outCsvFilePath" ) ) {
		printf( "File open error: %s\n", $s_outCsvFilePath );
		return undef;
	}

	$fh_csv_out->printf( encode( 'CP932', "対処,対象ファイル,行番号,指摘グループ,指摘ID,指摘内容,指摘行\n" ) );

	while( defined($csv_line_str = $fh_csv_in->getline) ) {
		# 改行を削除
		$csv_line_str =~ s/[\r\n]+$//;

		# CSVファイルの1行から指摘ファイルパス、行番号、指摘番号を抽出
		unless( $csv_line_str =~ /^"([^"]+)",(\d+),"[^"]*","(pgr\d\d\d\d)"/ ) {
			printf( "Error: Unknown line:. \"%s\"(%d)\n", $s_inCsvFilePath, $fh_csv_in->input_line_number );
			$fh_csv_out->printf( "\"?\",%s,\"\"\n", $csv_line_str );
			next;
		}

		my( $csv_dat ) = {
			'file_path' => $1,
			'line_no'   => $2,
			'pgr_id'    => $3,
			'line_str'  => $csv_line_str,
		};

		push( @$csv_dat_array, $csv_dat );
	}

	if( scalar(@$csv_dat_array) != 0 ) {
		check_csv_dat_array( $fh_csv_out, $source_in_dat, $csv_dat_array );
	}

	$fh_csv_in->close;
	file_close( $source_in_dat );

	return 1;
}

sub check_csv_dat_array {
	my( $fh_csv_out, $source_in_dat, $csv_dat_array ) = @_;

	# ファイルアクセスを効率的に行うためにファイル名と行番号でソートする
	@$csv_dat_array = sort {
		if( $a->{'file_path'} eq $b->{'file_path'} ) {
			return $a->{'line_no'} <=> $b->{'line_no'};
		}
		return $a->{'file_path'} cmp $b->{'file_path'};
	} @$csv_dat_array;


	# CSV1行ずつ処理
	foreach my $csv_dat ( @$csv_dat_array ) {
		unless( defined(file_read( $source_in_dat, $csv_dat->{'file_path'}, $csv_dat->{'line_no'} )) ) {
			$fh_csv_out->printf( "\"?\",%s,\"\"\n", $csv_dat->{'line_str'} );
			next;
		}

		if( $source_in_dat->{'cur_line_str'} =~ /$csv_dat->{'pgr_id'}/ ) {
			# 指摘番号がソース行に入っている ... 誤検出チェック済み
			my( $source_line_str_print ) = $source_in_dat->{'cur_line_str'};
			$source_line_str_print =~ s/"/""/g;
			$source_line_str_print =~ s/^\s+//;
			# 行頭に「"o",」、行末にソースの指摘行を「,"<行文字列>"」として追加する
			$fh_csv_out->printf( "\"o\",%s,\"%s\"\n", $csv_dat->{'line_str'}, $source_line_str_print );
		}
		else {
			# 指摘番号がソース行に入っていない
			my( $source_line_str_print ) = $source_in_dat->{'cur_line_str'};
			$source_line_str_print =~ s/"/""/g;
			$source_line_str_print =~ s/^\s+//;
			# 行頭に「"",」、行末にソースの指摘行を「,"<行文字列>"」として追加する
			$fh_csv_out->printf( "\"\",%s,\"%s\"\n", $csv_dat->{'line_str'}, $source_line_str_print );
		}
	}
}

sub file_open {
	my( $source_in_dat, $s_inFilePath ) = @_;
	local( $_ );

	# すでにオープン済みならクローズする
	if( defined($source_in_dat->{'opened'}) ) {
		file_close( $source_in_dat );
	}

	# ファイルオープン
	$source_in_dat->{'fh_in'} = new FileHandle;
	unless( $source_in_dat->{'fh_in'}->open( "< $s_inFilePath" ) ) {
		printf( "Error: source file open NG. \"%s\"\n", $s_inFilePath );
		return undef;
	}

	$source_in_dat->{'opened'} = 1;
	$source_in_dat->{'file_path'} = $s_inFilePath;
	$source_in_dat->{'cur_line_str'} = undef;

	return 1;
}

sub file_read {
	my( $source_in_dat, $s_inFilePath, $line_num ) = @_;

	# 指摘ファイルがオープンされていなければオープンする
	if( $source_in_dat->{'file_path'} ne $s_inFilePath ) {
		unless( defined(file_open( $source_in_dat, $s_inFilePath )) ) {
			return undef;
		}
	}

	# 読み込み済みの行なら読まずに抜ける
	if( $line_num == $source_in_dat->{'fh_in'}->input_line_number ) {
		# カレント行がマッチ
		return 1;
	}

	# すでに読み込み行を超えている場合は再度オープンし直す
	if( $line_num < $source_in_dat->{'fh_in'}->input_line_number ) {
		file_close( $source_in_dat );
		unless( defined(file_open( $source_in_dat, $s_inFilePath )) ) {
			return undef;
		}
	}

	# $line_numまでファイルを読み進める
	while( defined($_ = $source_in_dat->{'fh_in'}->getline) ) {
		if( $line_num == $source_in_dat->{'fh_in'}->input_line_number ) {
			$_ =~ s/[\r\n]+$//;
			$source_in_dat->{'cur_line_str'} = $_;
			return 1;
		}
		if( $line_num < $source_in_dat->{'fh_in'}->input_line_number ) {
			printf( "Error: source line not found. \"%s\"(%d)\n", $source_in_dat->{'file_path'}, $line_num );
			return undef;
		}
	}

	printf( "Error: source file read error. \"%s\"(%d)\n", $source_in_dat->{'file_path'}, $line_num );

	return undef;
}

sub file_close {
	my( $source_in_dat ) = @_;

	unless( defined($source_in_dat->{'opened'}) ) {
		# クローズ済みなので抜ける
		return;
	}

	$source_in_dat->{'fh_in'}->close;

	$source_in_dat->{'fh_in'} = undef;
	$source_in_dat->{'opened'} = undef;
	$source_in_dat->{'file_path'} = undef;
	$source_in_dat->{'cur_line_str'} = undef;
}

