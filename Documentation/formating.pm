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

    #print "raw string is '$inString'\n";
    local @meChars = split //, $inString;

    local $outString = "";

    while(@meChars) {
	local $snatched = 0; # has this character been snatched?
	local $ch = shift(@meChars);
	#print "ch = '$ch'\n";

	if($ch eq "&") {
	    $snatched = 1;
	    $outString .= "&amp;";
	}

	elsif($ch eq "<") {
	    $snatched = 1;
	    $outString .= "&lt;";
	}

	elsif($ch eq ">") {
	    $snatched = 1;
	    $outString .= "&gt;";
	}

	# look for doxygen "\ref" so we can put a link in instead
	elsif($ch eq "\\") {
	    
	    local $ref = join "", @meChars;
	    #print "eek = $ref\n" if($WIBBLE);
	    if($ref =~ /ref\s+/) {
		#remove the \ref and resplit the thing
		$ref =~ s/ref\s+//g;
		@meChars = split //, $ref;
		$snatched = 1;

		# put this in a sub, but how to pass a list?
		#snaffle up to next white space as this will be a link id and the link text
		local $id = "";
		local $going = 1;
		local $ch1 = 1;
		while($going and @meChars) {
		    $ch1 = shift @meChars;
		    if($ch1 =~ /\w/) {
			$id .= $ch1;
		    }
		    else {
			unshift @meChars, $ch1;
			$going = 0;
		    }
		}

		local $niceID = $id;
		$niceID =~ s/([A-Z])/ $1/g;

		#print "id = $niceID\n" if($WIBBLE);
		$outString .= "<link linkend=\"$id\">$niceID</link>";		
	    }
	}

	elsif($ch eq ":") {
	    local $ch1 = shift @meChars;
	    #print "ch1 = '$ch1'\n";
	    if($ch1 eq ":") {
		#snaffle up to next white space as this will be a link id and the link text
		local $id = "";
		local $going = 1;
		while($going and @meChars) {
		    $ch1 = shift @meChars;
		    if($ch1 =~ /\w/) {
			$id .= $ch1;
		    }
		    else {
			unshift @meChars, $ch1;
			$going = 0;
		    }
		}
		#print "found id of '$id'\n";

		$outString .= "<link linkend=\"$id\">$id</link>";
		
		$snatched = 1;
	    }
	    else {
		unshift @chars, $ch1;
		$snatched = 0;
	    }
	}
	$outString .= $ch unless $snatched;
    }

    return $outString;
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
