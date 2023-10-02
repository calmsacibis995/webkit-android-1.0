#! /usr/bin/perl
#
#   This file is part of the WebKit project
#
#   Copyright (C) 1999 Waldo Bastian (bastian@kde.org)
#   Copyright (C) 2007 Apple Inc. All rights reserved.
#   Copyright (C) 2007 Trolltech ASA
#
#   This library is free software; you can redistribute it and/or
#   modify it under the terms of the GNU Library General Public
#   License as published by the Free Software Foundation; either
#   version 2 of the License, or (at your option) any later version.
#
#   This library is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   Library General Public License for more details.
#
#   You should have received a copy of the GNU Library General Public License
#   along with this library; see the file COPYING.LIB.  If not, write to
#   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
#   Boston, MA 02110-1301, USA.
use strict;
use warnings;

open NAMES, "<CSSPropertyNames.in" || die "Could not find CSSPropertyNames.in";
my @names = ();
while (<NAMES>) {
  next if (m/#/);
  chomp $_;
  next if ($_ eq "");
  push @names, $_;
}
close(NAMES);

open GPERF, ">CSSPropertyNames.gperf" || die "Could not open CSSPropertyNames.gperf for writing";
print GPERF << "EOF";
%{
/* This file is automatically generated from CSSPropertyNames.in by makeprop, do not edit */
#include \"CSSPropertyNames.h\"
%}
struct props {
    const char* name;
    int id;
};
%%
EOF

foreach my $name (@names) {
  my $id = $name;
  $id =~ s/-/_/g;
  print GPERF $name . ", CSS_PROP_" . uc($id) . "\n";
}
print GPERF "%%\n";
close GPERF;

open HEADER, ">CSSPropertyNames.h" || die "Could not open CSSPropertyNames.h for writing";
print HEADER << "EOF";
/* This file is automatically generated from CSSPropertyNames.in by makeprop, do not edit */

#ifndef CSSPropertyNames_h
#define CSSPropertyNames_h

enum CSSPropertyID {
    CSS_PROP_INVALID = 0,
EOF

my $i = 1;
my $maxLen = 0;
foreach my $name (@names) {
  my $id = $name;
  $id =~ s/-/_/g;
  print HEADER "    CSS_PROP_" . uc($id) . " = " . $i . ",\n";
  $i = $i + 1;
  if (length($name) > $maxLen) {
    $maxLen = length($name);
  }
}
print HEADER "};\n\n";
print HEADER "const int numCSSProperties = " . $i . ";\n";
print HEADER "const size_t maxCSSPropertyNameLength = " . $maxLen . ";\n";

print HEADER << "EOF";

const char* getPropertyName(CSSPropertyID);

#endif
EOF

close HEADER;

system("gperf -a -L ANSI-C -E -C -c -o -t --key-positions=\"*\" -NfindProp -Hhash_prop -Wwordlist_prop -D -s 2 CSSPropertyNames.gperf > CSSPropertyNames.c");

open C, ">>CSSPropertyNames.c" || die "Could not open CSSPropertyNames.c for writing";
print C  "static const char * const propertyList[] = {\n";
print C  "\"\",\n";

foreach my $name (@names) {
  print C  "\"" . $name . "\", \n";
}

print C << "EOF";
    0
};
const char* getPropertyName(CSSPropertyID id)
{
    if (id >= numCSSProperties || id <= 0)
        return 0;
    return propertyList[id];
}
EOF

close C;
