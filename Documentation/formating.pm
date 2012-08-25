#! /usr/bin/perl

# these are references that we should ignore if encountered
%ignoredReference = (
kOfxChangePluginEdited => 1,
kOfxChangeTime => 1,
kOfxChangeUserEdited => 1,
kOfxFlagInfiniteMax => 1,
kOfxFlagInfiniteMin => 1,
kOfxImageClipPropFrameRange => 1,
kOfxImageEffectContextGeneral => 1,
kOfxImageEffectInstancePropFieldToRender => 1,
kOfxImageEffectRenderFullySafe => 1,
kOfxImageEffectRenderInstanceSafe => 1,
kOfxImageEffectRenderUnsafe => 1,
kOfxImageFieldBoth => 1,
kOfxImageFieldDoubled => 1,
kOfxImageFieldLower => 1,
kOfxImageFieldNone => 1,
kOfxImageFieldSingle => 1,
kOfxImageFieldUpper => 1,
kOfxImageOpaque => 1,
kOfxImagePreMultiplied => 1,
kOfxImageUnPreMultiplied => 1,
kOfxParamDoubleTypeAbsoluteTime => 1,
kOfxParamDoubleTypeAngle => 1,
kOfxParamDoubleTypeNormalisedX => 1,
kOfxParamDoubleTypeNormalisedXAbsolute => 1,
kOfxParamDoubleTypeNormalisedXY => 1,
kOfxParamDoubleTypeNormalisedXYAbsolute => 1,
kOfxParamDoubleTypeNormalisedY => 1,
kOfxParamDoubleTypeNormalisedYAbsolute => 1,
kOfxParamDoubleTypePlain => 1,
kOfxParamDoubleTypeScale => 1,
kOfxParamDoubleTypeTime => 1,
kOfxParamEventUserEdited => 1,
kOfxParamInvalidateAll => 1,
kOfxParamInvalidateValueChange => 1,
kOfxParamInvalidateValueChangeToEnd => 1,
kOfxParamPageSkipColumn => 1,
kOfxParamPageSkipRow => 1,
kOfxParamStringIsDirectoryPath => 1,
kOfxParamStringIsFilePath => 1,
kOfxParamStringIsLabel => 1,
kOfxParamStringIsMultiLine => 1,
kOfxParamStringIsSingleLine => 1,
kOfxParamTypeBoolean => 1,
kOfxParamTypeChoice => 1,
kOfxParamTypeCustom => 1,
kOfxParamTypeDouble => 1,
kOfxParamTypeDouble2D => 1,
kOfxParamTypeDouble3D => 1,
kOfxParamTypeGroup => 1,
kOfxParamTypeInteger => 1,
kOfxParamTypeInteger2D => 1,
kOfxParamTypeInteger3D => 1,
kOfxParamTypePage => 1,
kOfxParamTypePushButton => 1,
kOfxParamTypeRGB => 1,
kOfxParamTypeRGBA => 1,
kOfxParamTypeString => 1,
kOfxPropKeyString => 1,
kOfxPropKeySym => 1,
kOfxTypeClip => 1,
kOfxTypeImage => 1,
kOfxTypeImageEffect => 1,
kOfxTypeImageEffectHost => 1,
kOfxTypeImageEffectInstance => 1,
kOfxTypeParameter => 1,
kOfxTypeParameterInstance => 1 );

sub stripSurroundingWhite
{
    my $str =  $_[0];
    
    my @lines = split(/\n/, $str);
    my $str = "";

    # remove blank lines at the start and leading /trailing white space
    my $blankSoFar = 1;

    while(@lines) {
        my $line = shift @lines;
        $line =~ /^\s*(.*)\s*$/;  
        if ($1 eq "" and $blankSoFar) {
        }
        else {
            $blankSoFar = 0;
            $str .= $1 . "\n";
        }
    }

    chomp($str);
    return $str;
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

        if(not $ignoredReference{$member}) {
            $inString =~ s/(\w*)::(\w*)/<link linkend=\"$referredToObject\"><code>$referredToName<\/code><\/link>/; 
        }
        else {
            $inString =~ s/(\w*)::(\w*)/<code>$referredToName<\/code>/; 
        }
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
