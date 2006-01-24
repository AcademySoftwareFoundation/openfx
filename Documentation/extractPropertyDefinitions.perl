#! /usr/bin/perl

use formating;

@badlyNamedProperties = 
    ("kOfxImageEffectFrameVarying",
     "kOfxImageEffectPluginRenderThreadSafety",
    );

processFile("../include/ofxCore.h");
processFile("../include/ofxParam.h");
processFile("../include/ofxImageEffect.h");
processFile("../include/ofxInteract.h");
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
		    # look for our property definitions
		    if($outLine ne "kOfxPropertySuite" and $outLine =~ /^#define kOfx[\w_]*Prop/) {
			#print "comment is '$lastComment'\n";
			writePropFile($outLine, $lastComment, $sourceFile);

		    }
		    else {
			# look for badly named properties too
			foreach $bad (@badlyNamedProperties) {
			    if($outLine =~ /^#define $bad/) {
				writePropFile($outLine, $lastComment, $sourceFile);
			    }
			}
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

    $WIBBLE = $propName eq "kOfxImageEffectPropProjectSize";

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
    $generalDescription = $briefDescription . $generalDescription;
    $generalDescription = formatParagraph($generalDescription, 1);
    # do we have a valid value string?
    if(length($validString) > 0) {
	$validString = formatParagraph($validString, 1);
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
<refsynopsisdiv><blockquote><programlisting>
#include \"$includeFile\"
$define
</programlisting></blockquote></refsynopsisdiv>
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
