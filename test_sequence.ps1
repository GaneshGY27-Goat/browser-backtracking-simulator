Write-Host "--- Starting Automated College Viva Test Sequence ---" -ForegroundColor Cyan

Write-Host "`n[Step 1] Visiting YouTube..."
$r1 = Invoke-RestMethod -Method Post -Uri "http://localhost:8080/api/visit" -ContentType "application/json" -Body '{"url":"https://www.youtube.com/"}'
Write-Host "Current: $($r1.currentPage) | History Size: $($r1.historySize)"

Write-Host "`n[Step 2] Visiting Google Chrome..."
$r2 = Invoke-RestMethod -Method Post -Uri "http://localhost:8080/api/visit" -ContentType "application/json" -Body '{"url":"https://www.google.com/intl/en_in/chrome/"}'
Write-Host "Current: $($r2.currentPage) | History Size: $($r2.historySize)"

Write-Host "`n[Step 3] Visiting Instagram..."
$r3 = Invoke-RestMethod -Method Post -Uri "http://localhost:8080/api/visit" -ContentType "application/json" -Body '{"url":"https://www.instagram.com/?hl=en"}'
Write-Host "Current: $($r3.currentPage) | History Size: $($r3.historySize)"

Write-Host "`n[Step 4] Visiting Learner RSCOE Campus..."
$r4 = Invoke-RestMethod -Method Post -Uri "http://localhost:8080/api/visit" -ContentType "application/json" -Body '{"url":"https://learnerrscoe.edupluscampus.com/"}'
Write-Host "Current: $($r4.currentPage) | History Size: $($r4.historySize)"

Write-Host "`n[Step 5] Pressing Back (1st time)..."
$b1 = Invoke-RestMethod -Method Post -Uri "http://localhost:8080/api/back"
Write-Host "Result: $($b1.message)"
Write-Host "Current: $($b1.currentPage) | History Size: $($b1.historySize)"

Write-Host "`n[Step 5 cont.] Pressing Back (2nd time)..."
$b2 = Invoke-RestMethod -Method Post -Uri "http://localhost:8080/api/back"
Write-Host "Result: $($b2.message)"
Write-Host "Current: $($b2.currentPage) | History Size: $($b2.historySize)"

Write-Host "`n[Step 6 & 7] Verifying state..."
$state = Invoke-RestMethod -Method Get -Uri "http://localhost:8080/api/state"
if ($state.currentPage -eq "https://www.google.com/intl/en_in/chrome/") {
    Write-Host ">> PASS: Current Page is indeed Google Chrome!" -ForegroundColor Green
} else {
    Write-Host ">> FAIL: Current Page is $($state.currentPage)" -ForegroundColor Red
}

if ($state.history.Count -eq 1 -and $state.history[0] -eq "https://www.youtube.com/") {
    Write-Host ">> PASS: History stack is intact! Only YouTube remains at TOP." -ForegroundColor Green
} else {
    Write-Host ">> FAIL: History stack is corrupted" -ForegroundColor Red
}

Write-Host "`n[Step 8] Clearing History..."
$clr = Invoke-RestMethod -Method Post -Uri "http://localhost:8080/api/clear"
Write-Host "Result: $($clr.message)"
Write-Host "Current: '$($clr.currentPage)' | History Size: $($clr.historySize)"
if ($clr.historySize -eq 0 -and $clr.currentPage -eq "") {
    Write-Host ">> PASS: Empty state verified!" -ForegroundColor Green
}

Write-Host "`n[Step 9] Pressing Back on Empty Stack..."
$bEmpty = Invoke-RestMethod -Method Post -Uri "http://localhost:8080/api/back"
Write-Host "Success flag: $($bEmpty.success)"
Write-Host "Message: $($bEmpty.message)"
if ($bEmpty.success -eq $false) {
    Write-Host ">> PASS: Handled gracefully with warning message!" -ForegroundColor Green
}

Write-Host "`n--- College Viva Test Sequence Finished Successfully ---" -ForegroundColor Cyan
