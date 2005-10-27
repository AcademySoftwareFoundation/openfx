#! /usr/bin/perl

sub stripSurroundingWhite
{
    $_[0] =~ /\s*(.*)\s*/;
    return $1;
}

# look for doxgen reference tags, either \ref or "::"
# todo, tidy this up muchly
sub formatIt
{
    local $inString = $_[0];
    local $outString = "";
    
    # swap some no-no xml chars about
    $inString =~ s/\&/\&amp;/g;
    $inString =~ s/</\&lt;/g;
    $inString =~ s/>/\&gt;/g;

    while($inString =~ /::/) {
	$inString =~ /(\w*)::(\w*)/;
	my $obj = $1;
	my $member = $2;

	my $referredToObject = $member;
	my $referredToName = $member;
	if($obj ne "") {
	    $referredToObject = $obj . "_" . $referredToObject;
	    $referredToName = $obj . "XXXXXX" . $member;
	}

	$inString =~ s/(\w*)::(\w*)/<link linkend=\"$referredToObject\">$referredToName<\/link>/;
    }

    while($inString =~ /\\ref/) {
	$inString =~ /\\ref\s*(\w*)/;
	my $obj = $1;

	$inString =~ s/\\ref\s*(\w*)/<link linkend=\"$1\">$1<\/link>/;
    }

    while($inString =~ /\\e\s*\w*/) {
	$inString =~  s/\\e\s*(\w*)/<emphasis> $1<\/emphasis>/;
    }

    $inString =~ s/XXXXXX/::/g;
    return $inString;
}

# do some doxygen-a-like paragraph formatting
sub formatParagraph
{
    local @meLines = split /\n/, $_[0];
    local $openParagraph = $_[1];

    local $outString = "";
    if ($openParagraph != 0) {$outString = "<para>\n" ;}
    local $blankLineCount = 0;
    local $inList = 0;


    while(@meLines) {
	local $line = shift @meLines;
	chomp($line);
	
	# remove any whitespace at the beginning of a line
	$line =~ s/^\s*//g;

	# is it an empty line
	if(length($line) == 0) {
	    $blankLineCount += 1;
	}
	else {	    
	    $line = formatIt($line);
	    #look for '^-' which imples a itemized list
	    if($line =~ /^-\s*(.*)/) {
		$listValue = $1;
		if(!$inList) {
		    $inList = 1;
		    $outString .= "\n<itemizedlist>\n";
		}
		$outString .= "  <listitem>$listValue</listitem>\n";
	    }
	    elsif($line =~ /^\@verbatim/) {
		$outString .= "<programlisting>";
	    }
	    elsif($line =~ /^\@endverbatim/) {
		$outString .= "</programlisting>";
	    }
	    else {
		if($inList) {
		    $inList = 0;
		    $outString .= "</itemizedlist>\n";
		}

                # more than one blank line, pump out a paragraph
		if($blankLineCount > 0 ) {
		    if ($openParagraph != 0) {  $outString .= "</para>\n"; }
		    $outString .= "<para>\n";
		    $openParagraph = 1;
		}
		$outString .= $line . "\n";
	    }
	    $blankLineCount = 0;
	}
    }
    if($inList) {
	$outString .= "</itemizedlist>\n";
    }

    if ($openParagraph != 0) {$outString .= "</para>\n" ;}
    return $outString;
}

return 1;
