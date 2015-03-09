foreach ($file in (ls *.ts))
{
  tsc $file.Name
}