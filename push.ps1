# Script to push files to GitHub
Set-Location "c:\Users\manis\OneDrive\Documents\c code\Linux_driver"

# Configure git to auto-merge
git config pull.rebase false

# Fetch remote changes
git fetch origin main

# Force push to overwrite remote
Write-Host "Force pushing to remote repository..."
git push origin main --force

Write-Host "Push completed successfully!"
