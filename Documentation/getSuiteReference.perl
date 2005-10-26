#! /usr/bin/perl

use formating;
$suiteName = $ARGV[0];
$sourceFile = $ARGV[1];


open SRC, "< $sourceFile";

$inComment = 0;
$lastComment = "";
$lastCode = "";
$inSuite = 0;
$suiteDefinition = "";


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
		    $lastCode = "";
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
		if($inSuite) {
		    if(not($outLine =~ /^\s*$/)) {
			$suiteDefinition = $suiteDefinition . $outLine . "\n" ;
		    }
		}

		# look for our property definitions
		if( $outLine =~ /^typedef struct $suiteName/) {
		    #print "Suite comment is '$lastComment'\n";
		    $inSuite = 1;
		    $suiteDefinition = $outLine . "\n";
		    $suiteComment = $lastComment;
		}
		elsif($inSuite and $outLine =~ /} $suiteName/) {
		    #print "End of suite! it was \n$suiteDefinition '\n";
		    $inSuite = 0;
		    $suiteRef = WriteSuiteDefinition($suiteComment, $suiteDefinition);
		}
		elsif($outLine =~ /;/ and $inSuite) {
		    #print "end of statement, it was \n$lastCode\n";
		    $suiteFuncs .= WriteSuiteFunctionRef($lastComment, $lastCode);
		}

		$outLine = "";
	    }
	    else {
		$lastCode .= $ch;
		$outLine .= $ch;
	    }
	}

    }
}

print $suiteRef . $suiteFuncs;

# close the source file
close SRC;

exit ;

sub getBriefDescription
{
    my $comment = $_[0];
    # extract guff from the comment
    $briefDescription = "";

    my @commentLines = split(/\n/, $comment);
    while(@commentLines) {
	my $line = shift @commentLines;

	if($line =~ /\@brief(.*)/) {
	    $briefDescription = stripSurroundingWhite($1); #formatIt($1);
	    last;
	}
    }
    return formatIt($briefDescription);
}

sub getDescription
{
    my $comment = $_[0];
    # extract guff from the comment
    my $description = "";

    my @commentLines = split(/\n/, $comment);
    while(@commentLines) {
	my $line = shift @commentLines;

	if($line =~ /\@brief(.*)/) {
	}
	# ignore arg and return lines
	elsif($line =~ /^\s*\\arg/ or $line =~ /\@returns/) {
	    # eat up until blank 
	    while(@commentLines) {
		$line = shift @commentLines;
		if($line =~ /^\s*$/) {
		    unshift @commentLines, $line;
		    last;
		}
	    }
	}
	else {
	    $description .= $line . "\n";
	}
    }
    $description = stripSurroundingWhite($description);

    if($description eq "") {
	return "";
    }
    else {
	return formatParagraph($description, 1);
    }
}


# take the doxygen comment and code for suite definition and write out a reference for it
sub WriteSuiteDefinition
{
    my $comment = $_[0];
    my $code = $_[1];

    my $brief       = getBriefDescription($comment);
    my $description = getDescription($comment);

    return "
<refentry id=\"$suiteName\">
  <refmeta>
    <refentrytitle><type>$suiteName</type></refentrytitle>
  </refmeta>
  <refnamediv>
    <refname><function>$suiteName</function></refname>
    <refpurpose>$briefDescription</refpurpose>
  </refnamediv>
  <refsynopsisdiv>
     <programlisting>
#include \"$sourceFile\"
$code
</programlisting>
     </refsynopsisdiv>
    <refsect1>
      <title>Description</title>" .
      $description .
"    </refsect1>
</refentry>
";

}


# take the doxygen comment and the code block and write a nice reference from it in XML
sub WriteSuiteFunctionRef
{
    my $comment = $_[0];
    my $code = $_[1];
    
    # find the function name
    $funcName = $code;
    $funcName =~ s/.*\(\*//; 
    $funcName =~ s/\).*$//;

    my $args = {};
    my @argNames;
    # define a class func name
    $decoratedFuncName = "$suiteName" . "::" . "$funcName";
    
    # get the parameters out of it
    $code =~ /\(.*\)\s*\((.*)\)/;
    $paramsString = $1;
    @params = split( /,/, $paramsString);
    while(@params) {
	my $p = stripSurroundingWhite(shift @params);
	$arg = {};
	if($p =~ /\*/) {
	    $p =~ /(.*\*)(.*)/;
	    $args->{$2}->{type} = $1;
	    push @argNames, $2;
	}
	else {
	    $p =~ /(.*) (.*)/ ;
	    $args->{$2}->{type} = $1;
	    push @argNames, $2;
	}
    }

    # find return type
    $retType = $code;
    $retType =~ s/\s*\(.*$//;
    $retType =~ s/^\s*//;

    # extract guff from the comment
    my $brief       = getBriefDescription($comment);
    my $description = getDescription($comment);
    my $returnValues = "";

    # extract argument comments from the code
    my @commentLines = split(/\n/, $comment);
    while(@commentLines) {
	my $line = shift @commentLines;

	if($line =~ /\s*\\arg/) {
	    $line =~ s/\\arg//;
	    $line = stripSurroundingWhite($line);
	    $line =~ /^(\w*)(.*)/;
	    my $argName = $1;

	    my $desc = stripSurroundingWhite($2);
	    # remove any leading dashes..
	    $desc =~ s/^-//;

	    $args->{$argName}->{desc} = $desc;

	    # all you lines are belong to us, until we get to another \arg or a blank
	    while(@commentLines) {
		$line = shift @commentLines;
		if($line =~ /\\arg/ or $line =~ /^\s*$/) {
		    unshift @commentLines, $line;
		    last;
		}
		$args->{$argName}->{desc} .= "\n" . $line;
	    }
	}
	elsif($line =~ /\@returns/) {
	    $returnValues = "";

	    # all you lines are belong to us, until we get to another \arg or a blank
	    while(@commentLines) {
		$line = shift @commentLines;
		if($line =~ /^\s*$/) {
		    unshift @commentLines, $line;
		    last;
		}
		$returnValues .= "\n" . $line;
	    }
	}
    }

    my $value = "
<refentry id=\"$suiteName" . "_$funcName\">
  <refmeta>
    <refentrytitle><function>$decoratedFuncName</function></refentrytitle>
  </refmeta>
  <refnamediv>
    <refname><function>$decoratedFuncName</function></refname>
       <refpurpose>$brief</refpurpose>
  </refnamediv>
  <refsynopsisdiv>
    <funcsynopsis>
      <funcsynopsisinfo>#include \"$sourceFile\" </funcsynopsisinfo>
      <funcprototype>
        <funcdef><typename>$retType</typename><function>(*$funcName)</function></funcdef>
";
    # do the args
    for my $arg (@argNames) {
	$value .= "              <paramdef>$args->{$arg}->{type}<parameter>$arg</parameter></paramdef>\n"
    }
    
    $value .= 
"        </funcprototype>
    </funcsynopsis>
  </refsynopsisdiv>
  <refsect1>
   <title>Arguments</title>
   <itemizedlist>
";
    # do the args
    foreach my $arg (@argNames) {
	my $desc = formatParagraph($args->{$arg}->{desc}, 0);
	$value .= "
<listitem>$arg - $desc</listitem>";
    }
$value .= "
    </itemizedlist>
  </refsect1>";

    if($description ne "") {
	$value .= "
  <refsect1>
    <title>Description</title>
      $description
  </refsect1>
";
    }

    if($returnValues ne "") {
	$returnValues = formatParagraph($returnValues, 1);
	$value .= "
  <refsect1>
    <title>Returns</title>
      $returnValues
  </refsect1>";
    }
$value .= "
</refentry>
";
    return $value;
}
