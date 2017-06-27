
#
# dummy TEST.ps1 script
#
# Does nothing but 'sleep 1' and prints the actual execution time, which
# is expected to be... ~1s.
#

Write-Host "    TEST: SETUP"
$script:tm = [system.diagnostics.stopwatch]::startNew()

sleep 1

$end_time = $script:tm.Elapsed.ToString('hh\:mm\:ss\.fff') -Replace "^(00:){1,2}",""
$script:tm.reset()

Write-Host "    TEST: PASS`t`t`t [ $end_time s ]"
