#! /usr/bin/perl

processFile("../include/ofxCore.h");
processFile("../include/ofxParam.h");
exit;

processFile("fred.c");
exit;

$filesRaw = `ls ../include/ofx*.h`;

@files = split(/\n/,$filesRaw);

for(@files) {
    processFile($_);
}

exit;

# for each file
sub processFile()
{
    $sourceFile = $_[0];

    print "Scanning $sourceFile\n";

    open SRC, "< $sourceFile";

    $inComment = 0;
    $lastComment = "";

    while ($line = <SRC>) {
	@chars = split //, $line;

	# strip comments
	while(@chars) {
	    $ch = shift(@chars);

	    # Handle comments
	    if($inComment) {
		if($ch eq "*") {
		    $ch1 = shift @chars;
		    if($ch1 eq "/") {
			$inComment = 0;
			$ch = "";
		    }
		    else {
			unshift @chars, $ch1;
		    }
		}
		else {
		    $lastComment .= $ch;
		}
	    }
	    else {
		if($ch eq "/") {
		    $ch1 = shift @chars;
		    if ($ch1 eq "*") {
			$inComment = 1;
			$lastComment = "";
		    }
		    else {
			unshift @chars, $ch1;
		    }
		}
	    }

	    # if not in a comment output it
	    if(!$inComment) {
		#at end of line
		if($ch eq "\n") {
		    # look for our definitions
		    if($outLine ne "kOfxPropertySuite" and $outLine =~ /^#define kOfx[\w_]*Prop/) {
			#print "comment is '$lastComment'\n";
			writePropFile($outLine, $lastComment, $sourceFile);

		    }
		    $outLine = "";
		}
		else {
		    $outLine .= $ch;
		}
	    }

	}

    }

    # close the source file
    close SRC;
}

sub writePropFile()
{
    # extract the propertyname
    $define = $_[0];
    $_[0] =~ /^#define ([\w_]*)/;
    $propName = $1;
    local $propComment = $_[1];
    local $includeFile = $_[2];

    $includeFile =~ s/\.\.\/include//;

    # do some standard XML substitutions
    $propComment =~ s/</&lt;/g;
    $propComment =~ s/>/&gt;/g;

    @commentLines = split(/\n/, $propComment);

    $briefDescription = "";
    $typeDescription = "";
    $defaultDescription = "";
    $generalDescription = "";
    $vaidDescription = "";
    $validString = "";
    $inValidString = 0;

    while(@commentLines) {
	$line = shift @commentLines;
	chomp $line;

	# are we parsing the remaing options to the valid string argument
	if($inValidString) {
	    if($line =~ /\s*- (.*)/) {
		$validString .= "\n" . $line;
	    }
	    else {
		$inValidString = 0;
	    }
	}

	if($line =~ /\@brief(.*)/) {
	    $briefDescription = formatIt($1);
	}

	# Extract the type description
	elsif($line =~ /\s*- Type -\s*(.*)/) {
	    $typeField = $1;

	    if($typeField =~ /(.*) [xX] ([\dnN])/) {
		$typeName = $1;
		$typeDimension = $2;

		$typeDescription = "<literal>$propName</literal> is of type <type>$typeName</type>, and ";
		if($typeDimension eq "N" or $typeDimension eq "n") {
		    $typeDescription .= "is of variable dimension.";
		}
		else {
		    if($typeDimension eq "1") {
			$typeDescription .= "has only a single dimension.";	    
		    }
		    else {
			$typeDescription .= "has $typeDimension dimensions.";
		    }
		}
	    }
	    else {
		$typeDescription = formatIt($typeField);
	    }
	    $typeDescription = 
"<refsect2><title>Type</title>
<para>
$typeDescription
</para>  </refsect2>";
	}

	# Extract the property set description
	elsif($line =~ /\s*- Property Set -\s*(.*)/) {
	    $propSetDescription = formatIt($1);
	    $propSetDescription  = 
"<refsect2><title>Property Set</title>
<para> This is a property that belongs to the $propSetDescription.
</para> </refsect2>";
	}

	# Extract the default values
	elsif($line =~ /\s*- Default -\s*(.*)/) {
	    $defaultDescription = formatIt($1);
	    $defaultDescription  =
"<refsect2><title>Default</title> <para>
The default value is $defaultDescription.
</para> </refsect2>
";
	}
	elsif ($line =~ /\s*- Valid Values -\s*(.*)/) {
	    $validString = $1;
	    $inValidString = 1;
	}
	else {
	    if(!$inValidString) {
		# snaffle everything after the "Valid Values" statement
		$generalDescription = $generalDescription . $line . "\n";
	    }
	}
    }

    # clobber whitespace on the general description
    $generalDescription = formatParagraph($briefDescription . $generalDescription);
    # do we have a valid value string?
    if(length($validString) > 0) {
	$validString = formatParagraph($validString);
	$validString = 
"<refsect2><title>Valid Values</title>
$validString
</refsect2>
";
    }

    open PROPFILE, "> props/$propName";

print PROPFILE "
<refentry id=\"$propName\">
<refmeta>
<refentrytitle><literal>$propName</literal></refentrytitle>
</refmeta>
<refnamediv>
<refname><literal>$propName</literal></refname>
<refpurpose>$briefDescription</refpurpose>
</refnamediv>
<refsynopsisdiv><programlisting>
#include \"$includeFile\"
$define
</programlisting></refsynopsisdiv>
$typeDescription
$propSetDescription
$defaultDescription
$validString
<refsect2><title>Description</title>
$generalDescription
</refsect2>
</refentry>
";

    close PROPFILE;
}

# look for doxgen reference tags, either \ref or "::"
sub formatIt
{
    local $inString = $_[0];

    #print "raw string is '$inString'\n";
    local @meChars = split //, $inString;

    local $outString = "";

    while(@meChars) {
	local $ch = shift(@meChars);
	#print "ch = '$ch'\n";

	if($ch eq ":") {
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
		
	    }
	    else {
		unshift @chars, $ch1;
		$outString .= $ch;
	    }
	}
	else {
	    $outString .= $ch;
	}
    }

    return $outString;
}


# do some doxygen-a-like paragraph formatting
sub formatParagraph
{
    local @meLines = split /\n/, $_[0];

    local $outString = "<para>\n";
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
	    #look for '^-' which imples a itemized list
	    if($line =~ /^-\s*(.*)/) {
		$listValue = $1;
		if(!$inList) {
		    $inList = 1;
		    $outString .= "\n<itemizedlist>\n";
		}
		$outString .= "  <listitem>$listValue</listitem>\n";
	    }
	    else {
		if($inList) {
		    $inList = 0;
		    $outString .= "</itemizedlist>\n";
		}

                # more than one blank line, pump out a paragraph
		if($blankLineCount > 0 ) {
		    $outString .= "</para>\n<para>\n";
		}
		$outString .= $line . "\n";
	    }
	    $blankLineCount = 0;
	}
    }
    if($inList) {
	$outString .= "</itemizedlist>\n";
    }

    $outString .= "</para>\n";
    return formatIt($outString);
}
