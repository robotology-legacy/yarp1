#!/usr/bin/perl

print "
#ifndef YARPBottleCodes_INC
#define YARPBottleCodes_INC

// Automatically generated file -- do not edit directly.
";

print "// Generated in directory ",  `pwd`, "\n\n";

@vocab = ();
while (<>)
{
  chomp;
  $_ =~ s/[\n\r]//;  # get rid of ^Ms from windows editing
  if ($_ =~ /[a-zA-Z]/)
  {
    push(@vocab,$_);
  }
}

print "enum {\n";
foreach $v (@vocab)
{
  print "  YBC_$v,\n";
}
print "};\n\n";

$first = 1;

print "template <class T>\n";
print "const char *ybc_label(T x) {\n";
print "  switch (x) {\n";

foreach $v (@vocab)
{
  $vl = $v;
  $vl =~ tr/A-Z/a-z/;
  print "    case YBC_$v: return \"$vl\";\n";
}
print "  }\n";
print "  return \"UNKNOWN\";\n";
print "};\n";

print "\n\n#endif\n";
