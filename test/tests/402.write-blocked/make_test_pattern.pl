#!/usr/bin/perl -w

use strict;

#
# Hard-coded options for this test:
#
my $starttime = 1249508394.0;
my $timespan = 240;;
my $points = 20000;
my $UNIQUE = shift;

# No inputs after this

my $stoptime = $starttime + $timespan;
my $incr = ($timespan / $points);

my $x = 0;
for(my $t = $starttime; $t < $stoptime; $t += $incr){
	my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(int($t));
	# To test-pattern-han.in
	printf STDOUT "%04u-%02u-%02u %02u:%02u:%02u.%06u Morris frats '%u'\n",
		$year+1900, $mon+1, $mday, $hour, $min, $sec, ($t -  int($t)) * 1000000, 
		++$x;
	# To out.bionet-watcher
	printf STDERR "test-pattern-hab.$UNIQUE.Morris:frats = UInt32 Actuator %u @ %04u-%02u-%02u %02u:%02u:%02u.%06u\n",
		$x,
		$year+1900, $mon+1, $mday, $hour, $min, $sec, ($t -  int($t)) * 1000000;
}
