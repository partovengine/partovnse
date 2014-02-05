#!/usr/bin/env perl
open(LOGFILE, ">>failure.img") or die "Can't open logfile: $!";
@months = qw(Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec);
@weekDays = qw(Sun Mon Tue Wed Thu Fri Sat Sun);
for (;;) {
	print "####################################\n";
	print "## (Re)Starting Server......\n";
	print "####################################\n";
	system("./launcher.bash");
	my $exit_value  = $? >> 8;
	my $signal_num  = $? & 127;
	my $dumped_core = $? & 128;
	($second, $minute, $hour, $dayOfMonth,
	$month, $yearOffset, $dayOfWeek,
	$dayOfYear, $daylightSavings) = localtime();
	my $year = 1900 + $yearOffset;
	my $theTime = "$hour:$minute:$second, $weekDays[$dayOfWeek] $months[$month] $dayOfMonth, $year";
	if (!$signal_num) {
		$signal_num = "<Nothing!>";
	}
	print LOGFILE "Server died with signal ",
		$signal_num, ", ",
		($dumped_core ? 'with' : 'without'),
		" coredump.\n";
	print LOGFILE "At ", $theTime, "\n";
	system("./kill_server.sh");
	system("sleep 5");
}

