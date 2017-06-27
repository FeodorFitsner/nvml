
#
# dummy RUNTESTS.ps1 script
#
# Runs TEST.ps1 10 times in a loop, in a similar fashion as the real
# NVML tests are executed (a new job is spawned for each test).
#
# Since TEST.ps1 does only 'sleep 1', the total execution time is expected
# to be about 10s.  However, the overhead of spawning new job is about 1s,
# so the total time is close to 20s.
#

#
# runtest -- run tests script passed as an argument
#
function runtest {
    $runscript = $args[0]

    $script:tm = [system.diagnostics.stopwatch]::startNew()

    # repeat 10 times
    for ($i=1; $i -le 10;  $i++){
        Write-Host "RUNTESTS: Test: $runscript $i"

        $sb = {
            Write-Host "  job started"
            cd $args[0]
            Invoke-Expression $args[1]
        }
        $j1 = Start-Job -ScriptBlock $sb -ArgumentList (pwd).Path, ".\$runscript"

        # execute with timeout
        $timeout = New-Timespan -Seconds 180
        $stopwatch = [diagnostics.stopwatch]::StartNew()
        while (($stopwatch.Elapsed.ToString('hh\:mm\:ss') -lt $timeout) -And `
                ($j1.State -eq "NotStarted" -or $j1.State -eq "Running")) {
            Receive-Job -Job $j1
        }

        if ($stopwatch.Elapsed.ToString('hh\:mm\:ss') -ge $timeout) {
            Stop-Job -Job $j1
            Receive-Job -Job $j1
            Remove-Job -Job $j1 -Force
            throw "RUNTESTS: stopping: $runscript TIMED OUT"
        }

        Write-Host "  job completed"

        Remove-Job -Job $j1 -Force
    }

    $end_time = $script:tm.Elapsed.ToString('hh\:mm\:ss\.fff') -Replace "^(00:){1,2}",""
    $script:tm.reset()

    Write-Host "RUNTESTS TOTAL`t`t`t [ $end_time s ]"
}

####################

try {
    $LASTEXITCODE = 0
    runtest "TEST.PS1"
} catch {
    Write-Error "RUNTESTS FAILED"
    $status = 1
}

Exit $status
