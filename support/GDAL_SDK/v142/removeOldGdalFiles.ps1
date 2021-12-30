Get-ChildItem bin\* -Force -Exclude !!!*.txt |  Remove-Item -force -recurse
Get-ChildItem include\* -Force -Exclude !!!*.txt |  Remove-Item -force -recurse
Get-ChildItem lib\* -Force -Exclude !!!*.txt |  Remove-Item -force -recurse