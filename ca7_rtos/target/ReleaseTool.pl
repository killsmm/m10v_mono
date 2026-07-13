#! /bin/env perl
#
# ReleaseTool
#

use strict;
use DirHandle;
use FileHandle;
use Encode qw/encode decode/;
use utf8;

# REMOVE_COMMENT_xxxx 対象となるファイルの拡張子(ファイル名後方一致)
my( @A_ext_list ) = qw/
	.C
	.H
	.S
	.INC
	.OPT
	.TXT
	.BAT
	.SCAT
	.VIA
	.CFG
	.DOXY
	.MK
	.SH
	Makefile
/;


# デバッグログフラグ
my( $B_dbglog ) = undef;
#my( $B_dbglog ) = 1;
#



sub main {
	binmode STDOUT, ':encoding(utf8)';
	binmode STDERR, ':encoding(utf8)';

	if( scalar(@ARGV) == 0 ) {
		print_usage();
		exit 64;
	}


	my( $s_mode ) = shift @ARGV;

	if( $s_mode eq 'print_ini' ) {
		unless( defined(print_ini_main()) ) {
			exit 1;
		}
	}
	elsif( $s_mode eq 'remove_ini' ) {
		unless( defined(remove_ini_main()) ) {
			exit 1;
		}
	}
	elsif( $s_mode eq 'remove_comment' ) {
		unless( defined(remove_comment_main()) ) {
			exit 1;
		}
	}
	elsif( $s_mode eq 'search_str' ) {
		unless( defined(search_str_main()) ) {
			exit 1;
		}
	}
	else {
		printf( STDERR "Unknown option.\n", $s_mode );
		print_usage();
		exit 64;
	}

	exit 0;
}

sub print_usage {
	my( $s_cmd_name ) = $0;
	$s_cmd_name =~ s/^.*[\\\/]([^\\\/]+)$/$1/;

	my( $a_usage_str ) = [
		'print_ini      <ini path ...>',
		'remove_ini     <ini path ...>',
		'    ステートメントの法則：',
		'      REMOVE_AAA BEGIN',
		'        AAAのrelease/remove設定に従う。',
		'      REMOVE_BBB ELSEIF',
		'        BBBのrelease/remove設定に従う様切り替わる。',
		'      REMOVE_BBB ELSE',
		'        直前のBEGIN or ELSEIF(この場合はBBB)のrelease/remove設定の逆になる。',
		'        REMOVE_BBBは直前のものと同じ値である必要がある。',
		'      REMOVE_BBB END',
		'        REMOVEの終了。',
		'        REMOVE_BBBは直前のものと同じ値である必要がある。',
		'      ステートメントの前後には何が記載されていても良い。(つまりコメント行でなくともよい。)',
		'    ネストした場合の法則：',
		'      release設定中からネストした場合は新たなREMOVE_???のrelease/remove設定に従って処理される。',
		'      remove設定中(ELSEルートによるものも含む)からネストした場合は',
		'      REMOVE_???のrelease/remove設定によらず必ずremoveとなる。',
		'remove_comment <target path> <remove strings> <release|remove>',
		'    ステートメントの法則：',
		'      remove_iniを参照。',
		'search_str     <target path> <search strings>',
	];

	foreach my $s_usage_str ( @$a_usage_str ) {
		printf( STDERR "Usage: %s %s\n", $s_cmd_name, $s_usage_str );
	}
}

sub print_ini_main {
	my( $h_arg ) = {
		'as_target_path'   => \@ARGV,
		'b_recursive'      => 1,	# true:1 false:undef
	};
	my( $a_ini_dat_array ) = [];

	# iniファイルをパースして内容を受け取る
	foreach my $s_target_path ( @{$h_arg->{'as_target_path'}} ) {
		unless( defined(remove_ini_parse( $s_target_path, $a_ini_dat_array )) ) {
			return undef;
		}
	}



	# iniファイルの内容を表示
	if( defined($B_dbglog) ) {
		foreach my $h_ini_dat (@$a_ini_dat_array) {
			printf( "IP name: %s\n", $h_ini_dat->{'IP name'} );
			printf( "Target path:\n\t" );
			print( join( "\n\t", @{$h_ini_dat->{'a_target_path_array'}} ) . "\n" );
			printf( "Unlink path:\n\t" );
			print( join( "\n\t", @{$h_ini_dat->{'a_unlink_path_array'}} ) . "\n" );
		}
	}



	# IP名の最大長を求める
	my( $i_name_len ) = 0;
	foreach my $h_ini_dat (@$a_ini_dat_array) {
		if( length($h_ini_dat->{'IP name'}) > $i_name_len ) {
			$i_name_len = length($h_ini_dat->{'IP name'});
		}
	}


	# 表示
	foreach my $h_ini_dat (@$a_ini_dat_array) {
		printf( "%*s: %s\n", $i_name_len, $h_ini_dat->{'IP name'}, $h_ini_dat->{'b_release_remove'} );
	}

	return 1;
}

sub remove_ini_main {
	my( $h_arg ) = {
		'as_target_path'   => \@ARGV,
		'b_recursive'      => 1,	# true:1 false:undef
	};
	my( $a_ini_dat_array ) = [];
	my( $retcd ) = 1;

	# iniファイルをパースして内容を受け取る
	foreach my $s_target_path ( @{$h_arg->{'as_target_path'}} ) {
		unless( defined(remove_ini_parse( $s_target_path, $a_ini_dat_array )) ) {
			return undef;
		}
	}



	# iniファイルの内容を表示
	if( defined($B_dbglog) ) {
		foreach my $h_ini_dat (@$a_ini_dat_array) {
			printf( "IP name: %s\n", $h_ini_dat->{'IP name'} );
			printf( "Target path:\n\t" );
			print( join( "\n\t", @{$h_ini_dat->{'a_target_path_array'}} ) . "\n" );
			printf( "Unlink path:\n\t" );
			print( join( "\n\t", @{$h_ini_dat->{'a_unlink_path_array'}} ) . "\n" );
		}
	}



	# REMOVE_xxxを処理
	foreach my $h_ini_dat (@$a_ini_dat_array) {
		$B_dbglog && printf( "### %s\n", $h_ini_dat->{'IP name'} );

		foreach my $s_unlink_path (@{$h_ini_dat->{'a_unlink_path_array'}}) {
			$B_dbglog && printf( "   ##> %s\n", $s_unlink_path );
			printf( "\"%s\": %s file/directory %s ...\n", $h_ini_dat->{'IP name'}, $h_ini_dat->{'b_release_remove'}, $s_unlink_path );

			if( $h_ini_dat->{'b_release_remove'} eq 'release' ) {
				next;
			}

			if( -d $s_unlink_path ) {
				remove_dir_recursive( $s_unlink_path );
			}
			else {
				unlink( $s_unlink_path );
			}
		}
	}



	# カレントディレクトリから一致するファイルのパス文字列を配列で得る
	my( $s_ext_regexp );
	$s_ext_regexp = '(' . join( '|', @A_ext_list ) . ')';
	$s_ext_regexp =~ s/\./\\./g;

	my( @as_all_file_path ) = search_file( '.', $s_ext_regexp, $h_arg->{'b_recursive'} );
	# 行頭の"./"を削除
	grep( $_ =~ s/^\.\///o, @as_all_file_path );



	# REMOVE_COMMENT_xxxを処理
	$h_arg->{'s_line_sep'} = ':lf';

	foreach my $h_ini_dat (@$a_ini_dat_array) {
		$B_dbglog && printf( "*** %s %s\n", $h_ini_dat->{'IP name'}, $h_ini_dat->{'s_search_str'} );
	}

	$h_arg->{'h_ini_dat_hash'} = conv_ini_dat_array_to_hash( $a_ini_dat_array );
	# ファイル・ディレクトリパスの配列を作る
	my( $a_target_path_array ) = [];
	map( push( @$a_target_path_array, @{$_->{'a_target_path_array'}} ) ,@$a_ini_dat_array );
	# uniq
	@$a_target_path_array = uniq_by_hash( @$a_target_path_array );

	foreach my $s_target_path ( @$a_target_path_array ) {
		$s_target_path =~ s/\\/\//g;
		$B_dbglog && printf( "target file %s\n", $s_target_path );

		if( -d $s_target_path ) {
			# ディレクトリなら、@as_all_file_pathから一致するパスを取得して1ファイルずつコメント削除処理を実施
			printf( "process comment %s ...\n", $s_target_path );
			foreach my $s_path ( grep( /^$s_target_path/, @as_all_file_path ) ) {
				unless( defined(remove_ini_comment( $s_path, $h_arg )) ) {
					$retcd = undef;
				}
			}
		}
		if( -f $s_target_path ) {
			# 通常ファイル
			printf( "process comment %s ...\n", $s_target_path );
			unless( defined(remove_ini_comment( $s_target_path, $h_arg )) ) {
				$retcd = undef;
			}
		}
	}

	return $retcd;
}

sub uniq_by_hash {
	my( @array ) = @_;
	my %hash;

	@hash{@array} = ();
	return sort keys %hash;
}

sub get_ip_entry {
	my( $a_ini_dat_array, $s_search_str, $s_ip_name_for_new_entry ) = @_;
	my( $h_ini_dat );

	my( @a_result ) = grep( $_->{'s_search_str'} eq $s_search_str, @$a_ini_dat_array );

	if( scalar(@a_result) == 0 ) {
		$h_ini_dat = {
			# 's_search_str'
			# 'IP name'
			# 'b_release_remove'
			# 'a_target_path_array'
			# 'a_unlink_path_array'
		};
		$h_ini_dat->{'s_search_str'} = $s_search_str;
		$h_ini_dat->{'IP name'} = $s_ip_name_for_new_entry;
		$h_ini_dat->{'b_release_remove'} = 'remove';
		$h_ini_dat->{'a_target_path_array'} = [];
		$h_ini_dat->{'a_unlink_path_array'} = [];
		push( @$a_ini_dat_array, $h_ini_dat );

		return $h_ini_dat;
	}

	return @a_result[0];
}

sub remove_ini_parse {
	my( $s_ini_file_path, $a_ini_dat_array ) = @_;

	my( $h_ini_dat ) = undef;
	my( $linebuf );
	my( $fh_in );
	my( $s_search_str );
	my( $s_ip_name );
	my( $i_remove_release );

	$fh_in = new FileHandle;
	unless( $fh_in->open( "< $s_ini_file_path" ) ) {
		$B_dbglog && printf( "File open error: %s\n", $s_ini_file_path );
		return undef;
	}


	while( defined($linebuf = $fh_in->getline) ) {
		$linebuf =~ s/[\r\n]+$//;	# Remove Cr or/and Lf


		if( $linebuf =~ /^ONOFF_([a-zA-Z0-9_]+)=([01])\b/o ) {
			$s_search_str = 'REMOVE_' . $1;
			$s_ip_name = $1;
			$i_remove_release = $2;
			$h_ini_dat = get_ip_entry( $a_ini_dat_array, $s_search_str, $s_ip_name );
			if( $i_remove_release == 0 ) {
				$h_ini_dat->{'b_release_remove'} = 'remove';
			}
			else {
				$h_ini_dat->{'b_release_remove'} = 'release';
			}
			next;
		}

		if( $linebuf =~ /^(REMOVE_[a-zA-Z0-9_]+)="([^"]*)"/o ) {
			$s_search_str = $1;
			$s_ip_name = $2;
			$h_ini_dat = get_ip_entry( $a_ini_dat_array, $s_search_str, $s_ip_name );
			$h_ini_dat->{'IP name'} = $s_ip_name;
			next;
		}

		unless( defined($h_ini_dat) ) {
			next;
		}

		if( $linebuf =~ /^REMOVE_(COMMENT_DIR|COMMENT_FILE)=(.+)$/o ) {
				push( @{$h_ini_dat->{'a_target_path_array'}}, $2 );
				next;
		}
		if( $linebuf =~ /^REMOVE_(DIR|FILE)=(.+)$/o ) {
				push( @{$h_ini_dat->{'a_unlink_path_array'}}, $2 );
				next;
		}
	}

	$fh_in->close;

	return 1;
}

sub conv_ini_dat_array_to_hash {
	my( $a_ini_dat_array ) = @_;

	my( $h_ini_dat_hash ) = {};
	map( $h_ini_dat_hash->{$_->{'s_search_str'}} = $_, @$a_ini_dat_array );

	return $h_ini_dat_hash;
}

# remove_ini_comment()
# remove_commentとの違いは、
# BEGIN ～ ELSEIF ～ ELSE ～ ENDが使える事。
sub remove_ini_comment {
	my( $s_in_file_path, $h_arg ) = @_;

	my( $param ) = {
		's_in_file_path' => $s_in_file_path,
		's_out_file_path' => $s_in_file_path . '.tmp',
		'b_is_matched' => undef,	# 一度でも探索中のREMOVE文字列が発見されたかどうか。
		'fh_in' => undef,
		'fh_out' => undef,
		'out_linebuf' => '',
		'retcd' => 1,
	};

	$param->{'fh_in'} = new FileHandle;
	unless($param->{'fh_in'}->open( "< $s_in_file_path" ) ) {
		printf( "File open error: %s\n", $s_in_file_path );
		return undef;
	}
#	binmode $param->{'fh_in'}, $h_arg->{'s_encoding'};
	binmode $param->{'fh_in'}, $h_arg->{'s_line_sep'};


	remove_ini_comment_sub( $param, $h_arg );


	$param->{'fh_in'}->close;

	unless( defined($param->{'b_is_matched'}) ) {
		# 入力ファイルから改変なし。何もせずに終了。
		return $param->{'retcd'};
	}

	if( $param->{'out_linebuf'} ne '' ) {
		if( $param->{'b_is_matched'} == 1 ) {
			unless( defined($param->{'fh_out'}) ) {
				# Open output file and write spooled linebuffer.
				unless( defined(remove_ini_comment_open_out_file( $param, $h_arg )) ) {
					return undef;
				}
			}
		}
	}

	unless( defined($param->{'fh_out'}) ) {
		return $param->{'retcd'};
	}

	$param->{'fh_out'}->close;

	my( $filemode ) = get_file_mode( $s_in_file_path );
	unlink( $s_in_file_path );
	rename( $param->{'s_out_file_path'}, $s_in_file_path );
	set_file_mode( $filemode, $s_in_file_path );

	return $param->{'retcd'};
}

sub remove_ini_comment_open_out_file {
	my( $param, $h_arg ) = @_;

	$B_dbglog && printf( "Open output file %s\n", $param->{'s_out_file_path'} );
	$param->{'fh_out'} = new FileHandle;
	unless( $param->{'fh_out'}->open( "> $param->{'s_out_file_path'}" ) ) {
		printf( "File open error: %s\n", $param->{'s_out_file_path'} );
		$param->{'retcd'} = undef;
		return undef;
	}
#	binmode $param->{'fh_out'}, $h_arg->{'s_encoding'};
	binmode $param->{'fh_out'}, $h_arg->{'s_line_sep'};
	if( $param->{'out_linebuf'} ne '' ) {
		$param->{'fh_out'}->print( $param->{'out_linebuf'} );
		$param->{'out_linebuf'} = '';
	}

	return 1;
}

sub remove_ini_comment_sub_process_normal {
	my( $param, $h_arg, $a_statement_status, $linebuf ) = @_;

		if( $a_statement_status->[0]{'s_release_remove'} eq 'remove' ) {
			$param->{'b_is_matched'} = 1;
			$B_dbglog && printf( "%u: remove\n", $param->{'fh_in'}->input_line_number );
			next;
		}

		$B_dbglog && printf( "%u: release\n", $param->{'fh_in'}->input_line_number );
		unless( defined($param->{'b_is_matched'}) ) {
			$param->{'out_linebuf'} .= $linebuf . "\n";
			next;
		}

		unless( defined($param->{'fh_out'}) ) {
			unless( defined(remove_ini_comment_open_out_file( $param, $h_arg )) ) {
				return undef;
			}
		}
		$param->{'fh_out'}->print( $linebuf . "\n" );
}

sub remove_ini_comment_sub {
	my( $param, $h_arg ) = @_;

	my( $linebuf );
	my( $s_remove_xxx );
	my( $s_statement );
	my( $a_statement_status ) = [];	# remove/releaseを管理する配列。要素番号が大きいほどBEGIN～ENDのネストが浅い側。
	my( $h_ini_dat ) = undef;

	unshift( @$a_statement_status, {
			's_release_remove' => 'release',
			's_remove_xxx' => 'top',
		} );


	while( defined($linebuf = $param->{'fh_in'}->getline) ) {
		$linebuf =~ s/[\r\n]+$//;	# Remove Cr or/and Lf

#		$B_dbglog && printf( "%u: %s\n", $param->{'fh_in'}->input_line_number, $a_statement_status->[0]{'s_release_remove'} );

		if( $linebuf =~ /(^|\b)(remove_\w+)\s+(begin|else|elseif|end|remove)(\b|$)/i ) {
			# process "REMOVE_xxxx" statement line

			$s_remove_xxx = $2;
			$s_remove_xxx =~ y/a-z/A-Z/;
			$s_statement = $3;
			$s_statement =~ y/a-z/A-Z/;

			unless( exists($h_arg->{'h_ini_dat_hash'}{$s_remove_xxx}) ) {
				$B_dbglog && printf( "Undefined remove %s at %s line %u\n", $s_remove_xxx, $param->{'s_in_file_path'}, $param->{'fh_in'}->input_line_number );
				remove_ini_comment_sub_process_normal( $param, $h_arg, $a_statement_status, $linebuf );
				next;
			}

			$B_dbglog && printf( "%u: statement %s %s\n", $param->{'fh_in'}->input_line_number, $s_remove_xxx, $s_statement );


			$param->{'b_is_matched'} = 1;
			$h_ini_dat = $h_arg->{'h_ini_dat_hash'}{$s_remove_xxx};


			if( $s_statement eq 'REMOVE' ) {
				next;
			}
			elsif( $s_statement ne 'BEGIN' ) {
				if( scalar(@$a_statement_status) <= 1 ) {
					printf( "Warning: BEGIN statement not found at %s line %u\n", $param->{'s_in_file_path'}, $param->{'fh_in'}->input_line_number );
					$param->{'retcd'} = undef;
					next;
				}
			}


			if( $s_statement eq 'END' ) {
				shift( @$a_statement_status );
				$h_ini_dat = undef;
				next;
			}
			elsif( $s_statement eq 'ELSE' ) {
				if( $a_statement_status->[1]{'s_release_remove'} eq 'remove' ) {
					# ネストが若い側がremoveの場合はremoveを引き継ぐ。
					next;
				}

				if( $a_statement_status->[0]{'s_release_remove'} eq 'release' ) {
					$a_statement_status->[0]{'s_release_remove'} = 'remove';
				}
				else {
					$a_statement_status->[0]{'s_release_remove'} = 'release';
				}
				next;
			}
			elsif( $s_statement eq 'ELSEIF' ) {
				shift( @$a_statement_status );
			}


			# case "BEGIN" and "ELSEIF"

			if( $a_statement_status->[0]{'s_release_remove'} eq 'remove' ) {
				# 現在位置(ELSEIFの場合ネストが若い側)がremoveの場合はremoveを引き継ぐ。
				unshift( @$a_statement_status, {
						's_release_remove' => 'remove',
						's_remove_xxx' => $s_remove_xxx,
					} );
				next;
			}

			unshift( @$a_statement_status, {
					's_release_remove' => $h_ini_dat->{'b_release_remove'},
					's_remove_xxx' => $s_remove_xxx,
				} );

			next;
		}


		# process normal line
		remove_ini_comment_sub_process_normal( $param, $h_arg, $a_statement_status, $linebuf );
	}

	$B_dbglog && printf( "nest: %u\n", scalar(@$a_statement_status) );
}

sub remove_comment_main {
	my( $h_arg ) = {
		's_target_path'    => $ARGV[0],
		'b_recursive'      => 1,	# true:1 false:undef
		's_search_str'     => $ARGV[1],
		'b_release_remove' => $ARGV[2],	# "release", "remove"
	};
	my( $retcd ) = 1;
	my( $a_ini_dat_array ) = [];

	# iniファイル相当の構造体を作成する
	my( $s_ip_name ) = $h_arg->{'s_search_str'};
	$s_ip_name =~ s/^REMOVE_//;
	my( $h_ini_dat ) = get_ip_entry( $a_ini_dat_array, $h_arg->{'s_search_str'}, $s_ip_name );
	$h_ini_dat->{'b_release_remove'} = $h_arg->{'b_release_remove'};
	push( @{$h_ini_dat->{'a_target_path_array'}}, $h_arg->{'s_target_path'} );


	# カレントディレクトリから一致するファイルのパス文字列を配列で得る
	my( $s_ext_regexp );
	$s_ext_regexp = '(' . join( '|', @A_ext_list ) . ')';
	$s_ext_regexp =~ s/\./\\./g;

	my( @as_all_file_path );
	if( -d $h_arg->{'s_target_path'} ) {
		@as_all_file_path = search_file( $h_arg->{'s_target_path'}, $s_ext_regexp, $h_arg->{'b_recursive'} );
	}
	else {
		@as_all_file_path = ( $h_arg->{'s_target_path'} );
	}



	# REMOVE_COMMENT_xxxを処理
	$h_arg->{'s_line_sep'} = ':lf';

	foreach my $h_ini_dat (@$a_ini_dat_array) {
		$B_dbglog && printf( "*** %s %s\n", $h_ini_dat->{'IP name'}, $h_ini_dat->{'s_search_str'} );
	}

	$h_arg->{'h_ini_dat_hash'} = conv_ini_dat_array_to_hash( $a_ini_dat_array );
	# ファイル・ディレクトリパスの配列を作る
	my( $a_target_path_array ) = [ @as_all_file_path ];
	# uniq
	@$a_target_path_array = uniq_by_hash( @$a_target_path_array );

	foreach my $s_target_path ( @$a_target_path_array ) {
		$s_target_path =~ s/\\/\//g;
		$B_dbglog && printf( "target file %s\n", $s_target_path );

		if( -d $s_target_path ) {
			# ディレクトリなら、@as_all_file_pathから一致するパスを取得して1ファイルずつコメント削除処理を実施
#			printf( "process comment %s ...\n", $s_target_path );
			foreach my $s_path ( grep( /^$s_target_path/, @as_all_file_path ) ) {
				unless( defined(remove_ini_comment( $s_path, $h_arg )) ) {
					$retcd = undef;
				}
			}
		}
		if( -f $s_target_path ) {
			# 通常ファイル
			printf( "process comment %s ...\n", $s_target_path );
			unless( defined(remove_ini_comment( $s_target_path, $h_arg )) ) {
				$retcd = undef;
			}
		}
	}

	return $retcd;
}

sub remove_comment {
	my( $s_in_file_path, $h_arg ) = @_;

	my( $s_out_file_path ) = $s_in_file_path . '.tmp';
	my( $linebuf );
	my( $b_is_matched ) = undef;
	my( $fh_in );
	my( $fh_out ) = undef;
	my( $out_linebuf ) = '';
	my( $filemode );

	$filemode = get_file_mode( $s_in_file_path );
	$fh_in = new FileHandle;
	unless( $fh_in->open( "< $s_in_file_path" ) ) {
		printf( "File open error: %s\n", $s_in_file_path );
		return undef;
	}
#	binmode $fh_in, $h_arg->{'s_encoding'};
	binmode $fh_in, $h_arg->{'s_line_sep'};



	while( defined($linebuf = $fh_in->getline) ) {
		$linebuf =~ s/[\r\n]+$//;	# Remove Cr or/and Lf

		unless( $linebuf =~ /$h_arg->{'s_search_str'}/ ) {
			if( $b_is_matched == 1 ) {
				unless( defined($fh_out) ) {
					$fh_out = new FileHandle;
					unless( $fh_out->open( "> $s_out_file_path" ) ) {
						printf( "File open error: %s\n", $s_out_file_path );
						return undef;
					}
#					binmode $fh_out, $h_arg->{'s_encoding'};
					binmode $fh_out, $h_arg->{'s_line_sep'};
					if( $out_linebuf ne '' ) {
						$fh_out->print( $out_linebuf );
						$out_linebuf = '';
					}
				}
				$fh_out->print( $linebuf . "\n" );
			}
			else {
				$out_linebuf .= $linebuf . "\n";
			}
			next;
		}

		# Inside between $h_arg->{'s_search_str'}
		$b_is_matched = 1;
		if( $h_arg->{'b_release_remove'} eq 'remove' ) {
			while( defined($linebuf = $fh_in->getline) ) {
				if( $linebuf =~ /$h_arg->{'s_search_str'}/ ) {
					last;
				}
			}
		}
	}

	$fh_in->close;

	unless( defined($b_is_matched) ) {
		return 1;
	}

	if( $out_linebuf ne '' ) {
		if( $b_is_matched == 1 ) {
			unless( defined($fh_out) ) {
				$fh_out = new FileHandle;
				unless( $fh_out->open( "> $s_out_file_path" ) ) {
					printf( "File open error: %s\n", $s_out_file_path );
					return undef;
				}
#				binmode $fh_out, $h_arg->{'s_encoding'};
				binmode $fh_out, $h_arg->{'s_line_sep'};
				$fh_out->print( $out_linebuf );
				$out_linebuf = '';
			}
		}
	}

	unless( defined($fh_out) ) {
		return 1;
	}

	$fh_out->close;

	unlink( $s_in_file_path );
	rename( $s_out_file_path, $s_in_file_path );
	set_file_mode( $filemode, $s_in_file_path );

	return 1;
}

sub search_str_main {
	my( $h_arg ) = {
		's_target_path'    => $ARGV[0],
		'b_recursive'      => 1,	# true:1 false:undef
		's_search_str'     => $ARGV[1],
	};

	$h_arg->{'s_line_sep'} = ':lf';

	my( $s_ext_regexp );
	$s_ext_regexp = '(' . join( '|', @A_ext_list ) . ')';
	$s_ext_regexp =~ s/\./\\./g;

	my( @as_all_file_path );
	if( -d $h_arg->{'s_target_path'} ) {
		@as_all_file_path = search_file( $h_arg->{'s_target_path'}, $s_ext_regexp, $h_arg->{'b_recursive'} );
	}
	else {
		@as_all_file_path = ( $h_arg->{'s_target_path'} );
	}

	foreach my $s_path ( @as_all_file_path ) {
		search_str( $s_path, $h_arg );
	}
}

sub search_str {
	my( $s_in_file_path, $h_arg ) = @_;

	my( $linebuf );
	my( $fh_in );

	$fh_in = new FileHandle;
	unless( $fh_in->open( "< $s_in_file_path" ) ) {
		printf( "File open error: %s\n", $s_in_file_path );
		return undef;
	}
#	binmode $fh_in, $h_arg->{'s_encoding'};
	binmode $fh_in, $h_arg->{'s_line_sep'};


	while( defined($linebuf = $fh_in->getline) ) {
		$linebuf =~ s/[\r\n]+$//;	# Remove Cr or/and Lf

		if( $linebuf =~ /$h_arg->{'s_search_str'}/ ) {
			printf( "%s,%d,%s\n", $s_in_file_path, $fh_in->input_line_number, $linebuf );
			next;
		}
	}

	$fh_in->close;
	return 1;
}

##
## 指定ディレクトリ配下のファイルのパス一覧を取得する
## (再帰コールされる)
##
## in: $s_search_dir : 検索対象とするディレクトリ
## in: $s_search_file_suffix : 検索対象とするファイルの拡張子("."も入れる。指定しない場合は全て)
## out: @as_all_file_path : ファイルパスの配列

sub search_file {
	my( $s_search_dir, $s_search_file_suffix, $b_is_recursive ) = @_;

	# $s_search_dirにあるファイル/ディレクトリ一覧取得
	my @as_all_dir_path;
	my @as_all_file_path;


	my @asl_all_path = &get_dir_entry_list( $s_search_dir );

	# ディレクトリ一覧作成
	@as_all_dir_path = grep( -d $_, @asl_all_path );
	# ファイル一覧作成
	@as_all_file_path = grep( ! -d $_, @asl_all_path );


	# カレントディレクトリ、親ディレクトリを除く
	@as_all_dir_path = grep( $_ !~ /\/\.\.?$/o, @as_all_dir_path );

	# 拡張子が合致するファイルを検索し戻り値に設定
	if( length($s_search_file_suffix) > 0 ) {
		@as_all_file_path = grep( /${s_search_file_suffix}$/i, @as_all_file_path );
	}


	# ディレクトリ内のファイルを検索
	if( $b_is_recursive != 0 ) {
		foreach my $s_file_path ( @as_all_dir_path ) {
			# $s_dir_pathディレクトリの内部を探索(再帰コール)
			# 得られたCソースファイル一覧を配列に追加
			push( @as_all_file_path, &search_file( $s_file_path, $s_search_file_suffix, $b_is_recursive ) );
		}
	}


	return @as_all_file_path;
}

## ディレクトリにあるエントリの一覧をパス付の形で得る
##
## in: $s_dir_path ディレクトリパス
## ret: @as_file_path ディレクトエントリのパス配列
sub get_dir_entry_list {
	my( $s_dir_path ) = @_;

	# ディレクトリオープン
	my( $dh ) = new DirHandle( $s_dir_path );
	unless( defined $dh ) {
		return undef;
	}

	# ディレクトリ内のエントリ一覧を取得
	my @as_file_path =  map "$s_dir_path/$_", $dh->read;

	return @as_file_path;
}


## 指定ディレクトリを削除する
## (再帰コールされる)
##
## in: $s_remove_dir : 検索対象とするディレクトリ
## ret: 成功:1 失敗:0
##
sub remove_dir_recursive {
	my( $s_remove_dir ) = @_;
	my( $i_ret ) = 0;

	# $s_remove_dirにあるファイル/ディレクトリ一覧取得
	my @as_all_dir_path;
	my @as_all_file_path;
	my @asl_all_path = &get_dir_entry_list( $s_remove_dir );

	# ディレクトリ一覧作成
	@as_all_dir_path = grep( -d $_, @asl_all_path );
	# ファイル一覧作成
	@as_all_file_path = grep( ! -d $_, @asl_all_path );

	# カレントディレクトリ、親ディレクトリを取り除く
	@as_all_dir_path = grep( $_ !~ /\/\.\.?$/o, @as_all_dir_path );


	# ファイルを全て削除
	$i_ret += unlink @as_all_file_path;


	# ディレクトリ内の全てのエントリを削除してからディレクトリを削除
	map { &remove_dir_recursive( $_ ); } @as_all_dir_path;


	# 引数で指定されたディレクトリ削除
	$i_ret += rmdir( $s_remove_dir );

	return ($i_ret != 0)?(1):(0);
}

sub get_file_mode {
	my( $file_path ) = @_;

	my($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,$atime,$mtime,$ctime,$blksize,$blocks) = stat( $file_path );

	$mode &= 07777;

	return $mode;
}

sub set_file_mode {
	my( $mode, $file_path ) = @_;

	chmod( $mode, $file_path );
}

&main;

