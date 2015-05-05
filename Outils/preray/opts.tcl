#
# getopt.tcl
#     simple tk-like option parser
# usage getopt arrname args
# where arrname - array which have entries for all possible options
# without leading dash (possible empty)
#
proc getopt {arrname args} {
if {[llength $args]==1} {eval set args $args}
if {![llength $args]} return
if {[llength $args]%2!=0} {error "Odd count of opt. arguments"}
array set tmp $args
foreach i [uplevel array names $arrname] {
   if [info exists tmp(-$i)] {
      uplevel set "$arrname\($i\)" $tmp(-$i)
      unset tmp(-$i)
   }
}
set wrong_list [array names tmp]
if [llength $wrong_list] { 
   set msg "Unknown option(s) $wrong_list. Must be one of:"
   foreach i [uplevel array names $arrname] {
     append msg " -$i"
   }
  error $msg 
}
} 

proc test {args} {
set opt(delay) 1000
set opt(width) 150
set opt(color) #FFFFFF
getopt opt $args
foreach i [array names opt] {
puts $i:$opt($i)
}
}
