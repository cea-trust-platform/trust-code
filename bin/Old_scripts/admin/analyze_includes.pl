#!/usr/bin/perl -w
#use strict;
use Data::Dumper;

# Detects:
#- First include of cpp files not being the header file for that class or precompiled header.
#- Redundant includes
#- Unnecessary includes because nothing in that file is used as:
#   - Member variable in the class declaration (classes and enums)
#   - Base class for the current class declaration
#   - Types (including enums) used as parameters
#   - Types (including enums) used as return values
#
# Still need to detect:
#- Pointers or references used in header files (can it even do that??)
#- Look for useless includes in cpp files as well
#- Defines? (go the other way around, from the unused headers--I don't think this is possible here)
#- Templates??



#require "docs/perlmod/DoxyDocs.pm";
require "DoxyDocs.pm";

my $precompiled_filename = "stdafx";
my $cpp_extension = "cpp";
my $header_extension = "h";


print "C++ Include Analyzer\n";

my %filemap;
my %classmap;
my %enummap;
&GenerateMap($doxydocs->{files}, \%filemap);
&GenerateMap($doxydocs->{classes}, \%classmap);
&GenerateEnumMap($doxydocs->{files}, \%enummap);

&UpdateDoxyDocs($doxydocs->{classes}, $doxydocs->{files});

&CheckFirstInclude($doxydocs->{files});
&CheckDuplicateIncludes($doxydocs->{files}, $header_extension);
&CheckDuplicateIncludes($doxydocs->{files}, $cpp_extension);
&CheckUselessIncludesInHeaders($doxydocs->{files}, $doxydocs->{classes});
&DisplayMostIncludedHeaders($doxydocs->{files});


sub UpdateDoxyDocs()
{
    my ($classes, $files) = @_;

    foreach my $class (@$classes) {
        my $file = $filemap{$class->{includes}->{name}};
        next if !$file;

        my %classinfo;
        $classinfo{name} = $class->{name};
        push @{$file->{classes}->{members}}, {%classinfo};
    }
}


sub GenerateMap()
{
    my ($files, $filemap) = @_;
    foreach my $file (@$files) {
        $filemap->{$file->{name}} = $file;
    }    
}

sub GenerateEnumMap()
{
    my ($files, $enummap) = @_;
    foreach my $file (@$files) {
        next if !($file->{enums});
        foreach my $enum (@{$file->{enums}->{members}}) {
            $enum->{filename} = $file->{name};
            $enummap->{$enum->{name}} = $enum;
        }
    }
}


sub CheckFirstInclude()
{
    my ($files) = @_;

    print "\nLooking for .$cpp_extension files whose first include is not the header file...\n";
    foreach my $file (@$files) {
        my ($filename, $extension) = split(/\./,$file->{name});
        next if !($extension =~ /$cpp_extension/);
        next if (@{$file->{includes}}) == 0;
        
        # Ignore precompiled header
        my $index = 0;
        if ($file->{includes}[0]->{name} =~ /^$precompiled_filename/) {
            $index++;
        }
        if (!($file->{includes}[$index]->{name} =~ /^$filename/)) {
            print "\t$file->{name} --> $file->{includes}[$index]->{name}\n";
        }
   }
}


sub CheckDuplicateIncludes()
{
    my ($files, $extension) = @_;

    print "\nLooking for duplicate includes in $extension files...\n";
    foreach my $file (@$files) {
        next if !($file->{name} =~ /\.$extension$/);
    
        my $num_included = @{$file->{includes}};
        next if (@{$file->{includes}}) < 2;

        foreach my $include_to_traverse (@{$file->{includes}}) {
            foreach my $include_to_check (@{$file->{includes}}) {
                if (&FileIsInIncludeChain($include_to_traverse->{name}, $include_to_check->{name})) {
                    print "\t$file->{name} --> $include_to_check->{name}\n";
                }
            }            
        }
    }
}

sub FileIsInIncludeChain()
{
    my ($filename_to_traverse, $filename) = @_;

    my $file_info = $filemap{$filename_to_traverse};
    if ($file_info) {
        foreach my $include (@{$file_info->{includes}}) {
            if (($include->{name} eq $filename) ||
                (&FileIsInIncludeChain($include->{name}, $filename))) {
                return 1;
            }
        }
    }        
    return 0;
}


sub CheckUselessIncludesInHeaders()
{
    my ($files, $classes) = @_;

    print "\nLooking for useless includes in header files...\n";
    foreach my $file (@$files) {
        next if !($file->{name} =~ /\.$header_extension$/);
        next if (@{$file->{includes}}) == 0;
    
        my %used_includes;
        
        foreach my $class_info (@{$file->{classes}->{members}}) {
            my $class = $classmap{$class_info->{name}};
            next if !$class;
        
            &FlagIncludesFromMembers($class->{public_members}->{members}, \%used_includes);
            &FlagIncludesFromMembers($class->{protected_members}->{members}, \%used_includes);
            &FlagIncludesFromMembers($class->{private_members}->{members}, \%used_includes);

            &FlagIncludesFromInheritance($class->{base}, \%used_includes);

            &FlagIncludesFromMethods($class->{public_methods}->{members}, \%used_includes);
            &FlagIncludesFromMethods($class->{pritected_methods}->{members}, \%used_includes);
            &FlagIncludesFromMethods($class->{private_methods}->{members}, \%used_includes);
        }
        
        foreach my $include (@{$file->{includes}}) {
            next if !($filemap{$include->{name}});
            
            if (!$used_includes{$include->{name}}) {
                print "\t$file->{name} --> $include->{name} (useless)\n";
            }
            elsif ($used_includes{$include->{name}} == 2) {
                print "\t$file->{name} --> $include->{name} (replace with forward decl)\n";
            }
        }
    }        
}

sub FlagIncludesFromMembers()
{
    my ($members, $used_includes) = @_;
    
    foreach my $member (@{$members}) {
        &FlagIncludesFromType($member->{type}, $used_includes);
    }    
}

sub FlagIncludesFromMethods()
{
    my ($methods, $used_includes) = @_;

    foreach my $method (@{$methods}) {
        if ($method->{type}) { 
            &FlagIncludesFromType($method->{type}, $used_includes, 1);
        }
        foreach my $param (@{$method->{parameters}}) {
            &FlagIncludesFromType($param->{type}, $used_includes, 1);
        }       
    }
}

sub FlagIncludesFromType()
{
    my ($typename, $used_includes, $forward_decl_ok) = @_;

    if ($enummap{$typename}) {
        $used_includes->{$enummap{$typename}->{filename}} = 1;
    }
    elsif ($classmap{$typename}) {
        my $filename = $classmap{$typename}->{includes}->{name};
        if (!$forward_decl_ok) {      
            $used_includes->{$filename} = 1;
        }
        elsif (!$used_includes->{$filename}) {
            $used_includes->{$filename} = 2;
        }
    }
    else {
        my ($new_typename, $ref_type) = split(/ /,$typename);
        if ($ref_type && ($ref_type eq "*" || $ref_type eq "&")) {
            if ($classmap{$new_typename}) {
                my $filename = $classmap{$new_typename}->{includes}->{name};
                if (!$used_includes->{$filename}) {
                    $used_includes->{$filename} = 2;
                }
            }
        }
    }    
}


sub FlagIncludesFromInheritance()
{
    my ($bases, $used_includes) = @_;

    foreach my $base (@{$bases}) {            
        my $member_type = $classmap{$base->{name}};
        next if !$member_type;
        $used_includes->{$member_type->{includes}->{name}} = 1;        
    }
}


sub DisplayMostIncludedHeaders()
{
    my ($files) = @_;

    print "\nCounting number of times each file is included...\n";
    
    # This will take up to n*n iterations--brrrr....
    my $loops = 0;
    while (&UpdateIncludeCounts($files) && ($loops++ < 100000)) {};

    my @sorted_files = sort {$b->{included_count} <=> $a->{included_count}} @$files;
    my $num_to_display = @sorted_files*0.10;
    print "\tShowing top 20% files ($num_to_display):\n";
    for (my $i=0; $i < $num_to_display; $i++) {
        my $file = $sorted_files[$i];
        print "\t$file->{name}: $file->{included_count}\n";
    }
}

sub UpdateIncludeCounts()
{
    my $updated = 0;

    my ($files) = @_;
    foreach my $file (@$files) {
        my $count = 0;
        foreach my $included_by (@{$file->{included_by}}) {
            $count++;
            if ($filemap{$included_by->{name}} && $filemap{$included_by->{name}}->{included_count}) {
                $count += $filemap{$included_by->{name}}->{included_count};
            }
        }
        if (!defined $file->{included_count} || ($count != $file->{included_count})) {
            #print "$file->{name}: $count\n";
            $file->{included_count} = $count;
            $updated = 1;
        }
    }

    return $updated;
}


 
